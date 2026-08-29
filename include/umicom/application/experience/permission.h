/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/permission.h
 *
 * PURPOSE:
 *   Represent the permissions available to a command caller without coupling
 *   reusable Framework commands to a particular identity provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_PERMISSION_H
#define UMICOM_APPLICATION_EXPERIENCE_PERMISSION_H

#include "umicom/application/experience/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperiencePermissionSet {
  char items[UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY]
            [UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  size_t count;
  uint64_t revision;
} UmiApplicationExperiencePermissionSet;

void umi_application_experience_permission_set_init(
    UmiApplicationExperiencePermissionSet *permissions);
UmiStatus
umi_application_experience_permission_set_grant(UmiApplicationExperiencePermissionSet *permissions,
                                                const char *permission_id);
int umi_application_experience_permission_set_has(
    const UmiApplicationExperiencePermissionSet *permissions, const char *permission_id);
int umi_application_experience_permission_set_allows(
    const UmiApplicationExperiencePermissionSet *permissions,
    const UmiApplicationExperienceCommand *command);

#ifdef __cplusplus
}
#endif

#endif
