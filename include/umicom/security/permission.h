/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/permission.h
 *
 * PURPOSE:
 *   Define deterministic permission sets used by identities, roles, sessions,
 *   plug-ins, commands and AI tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_PERMISSION_H
#define UMICOM_SECURITY_PERMISSION_H

#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PERMISSION_CAPACITY 160U
#define UMI_PERMISSION_SET_MAX 256U
typedef struct UmiPermissionSet UmiPermissionSet;
UmiStatus umi_permission_set_create(UmiPermissionSet **out_set);
void umi_permission_set_destroy(UmiPermissionSet *set);
UmiStatus umi_permission_set_add(UmiPermissionSet *set, const char *permission);
UmiStatus umi_permission_set_remove(UmiPermissionSet *set, const char *permission);
int umi_permission_set_contains(const UmiPermissionSet *set, const char *permission);
size_t umi_permission_set_count(const UmiPermissionSet *set);
UmiStatus umi_permission_set_at(const UmiPermissionSet *set, size_t index,
                                char *out_permission, size_t capacity);
int umi_permission_name_valid(const char *permission);
#ifdef __cplusplus
}
#endif

#endif
