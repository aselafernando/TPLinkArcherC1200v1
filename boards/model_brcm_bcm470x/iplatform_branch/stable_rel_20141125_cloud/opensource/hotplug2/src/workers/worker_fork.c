#include "worker_fork.h"

static struct worker_fork_ctx_t *global_ctx;

/**
 * Destroys data structures related to the given child ID (not PID).
 *
 * @1 Worker context
 * @2 Child id
 *
 * Returns: void
 */
static void worker_fork_destroy(struct worker_fork_ctx_t *ctx, int destroy_id) {
	int rv;

	PRINTFUNC();
	if (ctx->children == NULL)
		return;

	rv = close(ctx->children[destroy_id]->event_fd);
	rv = close(ctx->children[destroy_id]->feedback_fd);
	free(ctx->children[destroy_id]);
}

/**
 * Forcibly terminates the given child process.
 *
 * @1 Worker context
 * @2 Child id
 *
 * Returns: return value of kill()
 */
inline static int worker_fork_terminate(struct worker_fork_ctx_t *ctx, int destroy_id) {
	PRINTFUNC();
	return kill(ctx->children[destroy_id]->pid, SIGKILL);
}

/**
 * The main body of child process. It keeps reading
 * events from event_fd, and writing dots to feedback_fd
 * to notify the parent that the event is processed.
 *
 * @1 Settings for action processing
 * @2 Event fd
 * @3 Feedback fd
 *
 * Returns: void
 */
static void worker_fork_child_process(struct settings_t *settings, int event_fd, int feedback_fd) {
	char *buffer;
	fd_set readfds;
	int rv;
	ssize_t read_size;
	struct timeval timeout;
	struct uevent_t *uevent;
	uint32_t event_size;

	PRINTFUNC();


	/* Endless loop... */
	while (1) {	
		/* If the child gets bored, it commits suicide. */
		timeout.tv_sec = 7;
		timeout.tv_usec = 0;

		FD_ZERO(&readfds);
		FD_SET(event_fd, &readfds);

		do {
			rv = select(event_fd+1, &readfds, NULL, NULL, &timeout);
		} while (rv == -1 && errno == EINTR);
		if (rv <= 0)
			break;

		if (!FD_ISSET(event_fd, &readfds))
			continue;

		if (read(event_fd, &event_size, sizeof(event_size)) != sizeof(event_size)) {
			/* Apparently the pipe is closed. */
			break;
		}

		buffer = malloc(event_size);
		read_size = read(event_fd, buffer, event_size);
		if (read_size != event_size) {
			free(buffer);
			continue;
		}

		uevent = uevent_deserialize(buffer, read_size);
		if (uevent == NULL) {
			free(buffer);
			continue;
		}

		action_perform(settings, uevent);

		uevent_free(uevent);
		free(buffer);
		write(feedback_fd, ".", 1);
	}
	close(feedback_fd);
	close(event_fd);
	exit(0);
}

/**
 * Creates a new child worker process.
 *
 * @1 Worker context
 *
 * Returns: Pointer to new child structure on success, NULL otherwise.
 */
static struct worker_fork_child_t *worker_fork_spawn(struct worker_fork_ctx_t *ctx) {
	int event_fds[2];
	int feedback_fds[2];
	int i;
	pid_t pid;
	struct worker_fork_child_t *child;

	PRINTFUNC();

	child = NULL;
	pipe(event_fds);
	pipe(feedback_fds);
	pid = fork();
	switch (pid) {
		case 0:
			signal(SIGCHLD, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			signal(SIGTERM, SIG_DFL);
			signal(SIGPIPE, SIG_DFL);
			signal(SIGUSR1, SIG_DFL);

			/* 
			 * Close and release everything that can be closed and released.
			 */
			close(ctx->settings->netlink_socket);
			for (i = 0; i < ctx->children_count; i++) {
				worker_fork_destroy(ctx, i);
			}
			free(ctx->children);

			/* Close the wrong side of the pipe */
			close(event_fds[1]);
			close(feedback_fds[0]);

			worker_fork_child_process(ctx->settings, event_fds[0], feedback_fds[1]);
			break;

		case -1:
			return NULL;
			break;

		default:
			/* Close the wrong side of the pipe */
			close(event_fds[0]);
			close(feedback_fds[1]);

			/*
			 * Store data about the newly created child process.
			 */
			child = malloc(sizeof(struct worker_fork_child_t));
			child->pid = pid;
			child->busy = child->processed = 0;
			child->event_fd = event_fds[1];
			child->feedback_fd = feedback_fds[0];

			ctx->children_count++;
			ctx->children = realloc(ctx->children,
			  sizeof(struct worker_fork_child_t*) * ctx->children_count);
			ctx->children[ctx->children_count-1] = child;

			break;
	}

	return child;
}


/**
 * Looks up child id by process id.
 *
 * @1 Worker context
 * @2 The pid to be looked up
 *
 * Returns: Child id if success, -1 if error.
 */
static int worker_fork_find_pid(struct worker_fork_ctx_t *ctx, pid_t pid) {
	int i;
	PRINTFUNC();
	
	for (i = 0; i < ctx->children_count; i++) {
		if (ctx->children[i]->pid == pid)
			return i;
	}

	return -1;
}

/**
 * Handles SIGCHLD.
 *
 * @1 Signal number according to sighandler specification.
 *
 * Returns: Void according to sighandler specification.
 */
static void worker_fork_sighandler(int signum) {
	int destroy_id;
	pid_t pid;

	PRINTFUNC();

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
		destroy_id = worker_fork_find_pid(global_ctx, pid);
		if (destroy_id < 0)
			continue;

		worker_fork_destroy(global_ctx, destroy_id);
		if (global_ctx->children_count > 1) {
			if (destroy_id != global_ctx->children_count - 1)
				global_ctx->children[destroy_id] = CURRENT_CHILD(global_ctx);
			global_ctx->children_count--;
			global_ctx->children = realloc(global_ctx->children,
				sizeof(struct worker_fork_child_t*) * global_ctx->children_count);
		} else {
			free(global_ctx->children);
			global_ctx->children = NULL;
			global_ctx->children_count = 0;
		}
	}
}

/**
 * Selects on all feedback file descriptors of all children
 * and retrieves updates.
 *
 * @1 Worker context
 *
 * Returns: void
 */
static void worker_fork_update_children(struct worker_fork_ctx_t *ctx) {
	int i;
	int maxfd;
	int rv;
	fd_set readfds;
	char buffer[128];
	struct timeval timeout;

	PRINTFUNC();
	
	if (ctx->children_count == 0)
		return;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO(&readfds);
	maxfd = -1;
	for (i = 0; i <ctx->children_count; i++) {
		if (ctx->children[i]->feedback_fd > maxfd)
			maxfd = ctx->children[i]->feedback_fd;
		FD_SET(ctx->children[i]->feedback_fd, &readfds);
	}

	/* 
	 * We have SIGINT that doesn't do SA_RESTART,
	 * that's why we do this loop.
	 */
	do {
		rv = select(maxfd+1, &readfds, NULL, NULL, &timeout);
	} while (rv == -1 && errno == EINTR);

	if (rv <= 0)
		return;
	
	/*
	 * Update status for every child process that reported something.
	 */
	for (i = 0; i <ctx->children_count; i++) {
		if (FD_ISSET(ctx->children[i]->feedback_fd, &readfds)) {
			read(ctx->children[i]->feedback_fd, buffer, 1);

			ctx->children[i]->processed++;
			ctx->children[i]->busy = 0;
		}
	}
}

static int worker_fork_relay_event(int fd, struct uevent_t *uevent) {
	ssize_t written;

	written = write(fd, &uevent->plain_s, sizeof(uint32_t));
	if (written != sizeof(uint32_t)) {
		fprintf(stderr, "Didn't write all of the size.\n");
		return -1;
	}

	written = write(fd, uevent->plain, uevent->plain_s);
	if (written != uevent->plain_s) {
		fprintf(stderr, "Didn't write all of the data.\n");
		return -1;
	}
	return 0;
}

/*
 * Exported module functions
 */
static void *worker_fork_init(struct settings_t *settings) {
	sigset_t sigset;
	struct sigaction sigact_new;
	struct sigaction sigact_ignore;
	struct worker_fork_ctx_t *ctx;
	PRINTFUNC();

	ctx = malloc(sizeof(struct worker_fork_ctx_t));
	ctx->children = NULL;
	ctx->children_count = 0;
	ctx->settings = settings;

	ctx->max_children = 15;
	ctx->always_fork = 0;

	ctx->settings->worker_argc--;
	while (ctx->settings->worker_argc > 0) {
		if (!strcmp(*ctx->settings->worker_argv, "--max-children")) {
			ctx->settings->worker_argv++;
			ctx->settings->worker_argc--;
			if (ctx->settings->worker_argc < 0)
				break;
			
			ctx->max_children = atoi(*ctx->settings->worker_argv);
		} else if (!strcmp(*ctx->settings->worker_argv, "--always-fork")) {
			ctx->always_fork = 1;
		}
		ctx->settings->worker_argc--;
		ctx->settings->worker_argv++;
	}

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGPIPE);

	sigact_new.sa_mask = sigset;
	sigact_new.sa_handler = worker_fork_sighandler;
	sigact_new.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &sigact_new, NULL);

	sigact_ignore.sa_mask = sigset;
	sigact_ignore.sa_handler = SIG_IGN;
	sigact_ignore.sa_flags = SA_RESTART;
	sigaction(SIGPIPE, &sigact_ignore, NULL);

	global_ctx = ctx;
	return ctx;
}

static void worker_fork_deinit(void *in_ctx) {
	int i;
	struct worker_fork_ctx_t *ctx = in_ctx;

	PRINTFUNC();

	signal(SIGCHLD, SIG_DFL);

	/*
	 * Slaughter all working children.
	 */
	for (i = 0; i < ctx->children_count; i++) {
		if (worker_fork_terminate(ctx, i))
			break;
		worker_fork_destroy(ctx, i);
	}
	free(ctx->children);
	free(ctx);
	global_ctx = NULL;
}


static int worker_fork_process(void *in_ctx, struct uevent_t *uevent) {
	int i;
	struct worker_fork_child_t *child;
	struct worker_fork_ctx_t *ctx = in_ctx;

	/*
	 * A big loop, because if we fail to process the event,
	 * we don't want to give up.
	 *
	 * TODO: Decide if we want to limit the number of attempts
	 * or set a time limit before reporting terminal failure.
	 */
	do {
		worker_fork_update_children(ctx);

		child = NULL;
		for (i = 0; i < ctx->children_count; i++) {
			if (ctx->children[i]->busy == 0) {
				child = ctx->children[i];
				break;
			}
		}

		/*
		 * No child process is currently available.
		 */
		if (child == NULL) {
			/*
			 * Are the matching rules trivial enough that we
			 * can execute them in the main process?
			 */
			if (ctx->always_fork == 0 && ctx->settings->dumb == 0 && 
			(ruleset_flags(&ctx->settings->rules, uevent) & FLAG_MASK_SLOW) == 0) {
				action_perform(ctx->settings, uevent);
				break;
			}
			
			/*
			 * We have to fork off a new child.
			 */
			if (ctx->children_count < ctx->max_children)
				child = worker_fork_spawn(ctx);
		}

		/*
		 * If we have a worker available, relay the event.
		 */
		if (child != NULL) {
			child->busy = 1;
			if (!worker_fork_relay_event(child->event_fd, uevent));
				break;
			child->busy = 0;
		}

		/* 
		 * Something went wrong. Sleep for the given grace time and
		 * try again.
		 */
		usleep(ctx->settings->grace);
	} while(1);

	return 0;
}

struct worker_module_t worker_module = {
	"Hotplug2 forking module",
	worker_fork_init,
	worker_fork_deinit,
	worker_fork_process
};
