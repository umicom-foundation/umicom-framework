# Application Experience Profile Quick Reference

## The short version

Use an experience profile when a workspace needs consistent command, form and
UI-state behavior.

| Audience | Validation | Layout feel | Guidance |
| --- | --- | --- | --- |
| Learning | When leaving a field | Comfortable | Detailed |
| Standard | As values change | Comfortable | Balanced |
| Focus | When submitting | Compact | Minimal |

All three retain destructive-action confirmation, undo and accessible state
announcements.

## Main public include

~~~c
#include "umicom/application/experience/experience.h"
~~~

## Useful entry points

~~~c
umi_application_experience_profile_catalogue_validate();
umi_application_experience_profile_catalogue_for_recipe(recipe_id);
umi_application_experience_command_enablement(command, permissions, context);
umi_application_experience_invocation_validate(command, invocation);
umi_application_experience_form_validate(model, report);
umi_application_experience_form_history_undo(history, model);
umi_application_experience_state_store_transition(store, next, reason);
~~~

## Rule of thumb

Put reusable behavior in Framework. An application should select commands,
forms, policies and layouts; it should not copy their implementation.
