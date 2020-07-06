#include "action.h"

/**
 * Blindly modprobe the modalias, nothing more.
 *
 * @1 Hotplug settings
 * @2 Event structure
 *
 * Returns: void
 */
static void action_dumb(const struct settings_t *settings, const struct uevent_t *uevent) {
	char *modalias;
	pid_t child;

	modalias = uevent_getvalue(uevent, "MODALIAS");
	if (modalias == NULL)
		return;

	child = fork();
	switch (child) {
		case 0:
			execl(settings->modprobe_command, settings->modprobe_command, "-q", modalias, NULL);
			exit(1);

		case -1:
			return;

		default:
			waitpid(child, NULL, 0);
	}
}

/**
 * Choose what action should be taken according to passed settings.
 *
 * @1 Hotplug settings
 * @2 Event structure
 *
 * Returns: void
 *
 */
void action_perform(struct settings_t *settings, struct uevent_t *event) {
	int i;

	for (i = 0; i < event->env_vars_c; i++)
		setenv(event->env_vars[i].key, event->env_vars[i].value, 1);
	
	if (settings->dumb == 0) {
		ruleset_execute(&settings->rules, event, settings);
	} else {
		action_dumb(settings, event);
	}

	for (i = 0; i < event->env_vars_c; i++)
		unsetenv(event->env_vars[i].key);
}
