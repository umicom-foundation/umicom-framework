/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/enablement.c
 *
 * PURPOSE:
 *   Produce a consistent command availability decision and a human-readable
 *   reason suitable for menus, command palettes and accessibility output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/enablement.h"

#include <string.h>

void umi_application_experience_context_default(UmiApplicationExperienceContext *context) {
  if (context != NULL) {
    (void)memset(context, 0, sizeof(*context));
    context->online = 1;
    context->component_visible = 1;
  }
}

UmiApplicationExperienceEnablement umi_application_experience_command_enablement(
    const UmiApplicationExperienceCommand *command,
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceContext *context) {
  UmiApplicationExperienceEnablement result = {UMI_APPLICATION_EXPERIENCE_COMMAND_DISABLED,
                                               "Command metadata is invalid."};
  if (command == NULL || context == NULL)
    return result;
  if (!context->component_visible) {
    result.availability = UMI_APPLICATION_EXPERIENCE_COMMAND_HIDDEN;
    result.reason = "The owning component is not visible.";
  } else if (!umi_application_experience_permission_set_allows(permissions, command)) {
    result.reason = "The current user does not have the required permission.";
  } else if (context->busy) {
    result.availability = UMI_APPLICATION_EXPERIENCE_COMMAND_BUSY;
    result.reason = "Another operation is still running.";
  } else if (umi_application_experience_command_has_flag(
                 command, UMI_APPLICATION_EXPERIENCE_COMMAND_REQUIRES_SELECTION) &&
             context->selected_item_count == 0U) {
    result.reason = "Select an item before running this command.";
  } else if (umi_application_experience_command_has_flag(
                 command, UMI_APPLICATION_EXPERIENCE_COMMAND_REQUIRES_ONLINE) &&
             !context->online) {
    result.reason = "Connect to a service before running this command.";
  } else if (context->read_only &&
             umi_application_experience_command_has_flag(
                 command, UMI_APPLICATION_EXPERIENCE_COMMAND_CHANGES_DATA) &&
             !umi_application_experience_command_has_flag(
                 command, UMI_APPLICATION_EXPERIENCE_COMMAND_ALLOWED_READ_ONLY)) {
    result.reason = "The current workspace is read-only.";
  } else {
    result.availability = UMI_APPLICATION_EXPERIENCE_COMMAND_ENABLED;
    result.reason = "Ready.";
  }
  return result;
}
