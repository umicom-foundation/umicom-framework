/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/permission.c
 *
 * PURPOSE:
 *   Maintain a bounded permission snapshot and make command authorization a
 *   deterministic step that can be tested without a graphical frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/permission.h"

#include <string.h>

#include "umicom/base/text.h"

void umi_application_experience_permission_set_init(
    UmiApplicationExperiencePermissionSet *permissions) {
  if (permissions != NULL) {
    (void)memset(permissions, 0, sizeof(*permissions));
    permissions->revision = 1U;
  }
}

int umi_application_experience_permission_set_has(
    const UmiApplicationExperiencePermissionSet *permissions, const char *permission_id) {
  size_t index;
  if (permissions == NULL || permission_id == NULL ||
      permissions->count > UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY)
    return 0;
  for (index = 0U; index < permissions->count; ++index) {
    if (strcmp(permissions->items[index], permission_id) == 0)
      return 1;
  }
  return 0;
}

UmiStatus
umi_application_experience_permission_set_grant(UmiApplicationExperiencePermissionSet *permissions,
                                                const char *permission_id) {
  UmiStatus status;
  if (permissions == NULL || !umi_application_experience_identifier_valid(permission_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (permissions->count > UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  if (umi_application_experience_permission_set_has(permissions, permission_id))
    return UMI_STATUS_ALREADY_EXISTS;
  if (permissions->count >= UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  status = umi_text_copy(permissions->items[permissions->count],
                         sizeof(permissions->items[permissions->count]), permission_id);
  if (status == UMI_STATUS_OK) {
    permissions->count += 1U;
    permissions->revision += 1U;
  }
  return status;
}

int umi_application_experience_permission_set_allows(
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceCommand *command) {
  if (command == NULL)
    return 0;
  return command->permission_id == NULL || command->permission_id[0] == '\0' ||
         umi_application_experience_permission_set_has(permissions, command->permission_id);
}
