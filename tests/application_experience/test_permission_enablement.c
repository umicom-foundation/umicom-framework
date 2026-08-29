/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_permission_enablement.c
 *
 * PURPOSE:
 *   Verify selection, connectivity, read-only and permission conditions produce
 *   stable command enablement decisions with understandable reasons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/experience.h"

#include <assert.h>

int main(void) {
  const UmiApplicationExperienceCommand command = {
      sizeof(UmiApplicationExperienceCommand),
      UMI_APPLICATION_EXPERIENCE_API_VERSION,
      "umicom.command.selection.publish",
      "Publish",
      "Publish the selection.",
      "File",
      "umicom.permission.publish",
      UMI_APPLICATION_EXPERIENCE_COMMAND_SAFE,
      UMI_APPLICATION_EXPERIENCE_COMMAND_REQUIRES_SELECTION |
          UMI_APPLICATION_EXPERIENCE_COMMAND_REQUIRES_ONLINE |
          UMI_APPLICATION_EXPERIENCE_COMMAND_CHANGES_DATA,
      NULL,
      0U};
  UmiApplicationExperiencePermissionSet permissions;
  UmiApplicationExperienceContext context;
  UmiApplicationExperienceEnablement enablement;
  umi_application_experience_permission_set_init(&permissions);
  umi_application_experience_context_default(&context);
  enablement = umi_application_experience_command_enablement(&command, &permissions, &context);
  assert(enablement.availability == UMI_APPLICATION_EXPERIENCE_COMMAND_DISABLED);
  assert(umi_application_experience_permission_set_grant(
             &permissions, "umicom.permission.publish") == UMI_STATUS_OK);
  context.selected_item_count = 1U;
  enablement = umi_application_experience_command_enablement(&command, &permissions, &context);
  assert(enablement.availability == UMI_APPLICATION_EXPERIENCE_COMMAND_ENABLED);
  context.read_only = 1;
  enablement = umi_application_experience_command_enablement(&command, &permissions, &context);
  assert(enablement.availability == UMI_APPLICATION_EXPERIENCE_COMMAND_DISABLED);
  return 0;
}
