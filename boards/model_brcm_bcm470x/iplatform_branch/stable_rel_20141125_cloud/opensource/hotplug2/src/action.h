#ifndef ACTION_H
#define ACTION_H 1

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rules/execution.h"

#include "uevent.h"
#include "settings.h"

void action_perform(struct settings_t *, struct uevent_t *);
#endif /* ifndef ACTION_H */

