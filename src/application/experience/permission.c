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

/*
 * Initialise application experience permission set from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_permission_set_init(
    UmiApplicationExperiencePermissionSet *permissions) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (permissions != NULL) {
    (void)memset(permissions, 0, sizeof(*permissions));
    permissions->revision = 1U;
  }
}

/*
 * Provide the application experience permission set has operation used by this module and
 * its client applications.
 */
int umi_application_experience_permission_set_has(
    const UmiApplicationExperiencePermissionSet *permissions, const char *permission_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (permissions == NULL || permission_id == NULL ||
      permissions->count > UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY)
    return 0;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < permissions->count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(permissions->items[index], permission_id) == 0)
      return 1;
  }
  return 0;
}

/*
 * Provide the application experience permission set grant operation used by this module
 * and its client applications.
 */
UmiStatus
umi_application_experience_permission_set_grant(UmiApplicationExperiencePermissionSet *permissions,
                                                const char *permission_id) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (permissions == NULL || !umi_application_experience_identifier_valid(permission_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (permissions->count > UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (umi_application_experience_permission_set_has(permissions, permission_id))
    return UMI_STATUS_ALREADY_EXISTS;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (permissions->count >= UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  status = umi_text_copy(permissions->items[permissions->count],
                         sizeof(permissions->items[permissions->count]), permission_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    permissions->count += 1U;
    permissions->revision += 1U;
  }
  return status;
}

/*
 * Provide the application experience permission set allows operation used by this module
 * and its client applications.
 */
int umi_application_experience_permission_set_allows(
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceCommand *command) {
  /* Use the shared build helper when it is available from the parent composition. */
  if (command == NULL)
    return 0;
  return command->permission_id == NULL || command->permission_id[0] == '\0' ||
         umi_application_experience_permission_set_has(permissions, command->permission_id);
}
