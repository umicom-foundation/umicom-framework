/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/permission.h
 *
 * PURPOSE:
 *   Define deterministic permission sets used by identities, roles, sessions,
 *   plug-ins, commands and AI tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the permission set data shared with callers of this public contract.
 */
typedef struct UmiPermissionSet UmiPermissionSet;
/**
 * Initialise permission set from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_permission_set_create(UmiPermissionSet **out_set);
/**
 * Release or reset state held by permission set so the same storage can be reused safely.
 */
void umi_permission_set_destroy(UmiPermissionSet *set);
/**
 * Add permission set only after its inputs and available capacity have been checked.
 */
UmiStatus umi_permission_set_add(UmiPermissionSet *set, const char *permission);
/**
 * Remove permission set while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_permission_set_remove(UmiPermissionSet *set, const char *permission);
/**
 * Provide the permission set contains operation used by this module and its client
 * applications.
 */
int umi_permission_set_contains(const UmiPermissionSet *set, const char *permission);
/**
 * Return the number of records represented by permission set without changing their state.
 */
size_t umi_permission_set_count(const UmiPermissionSet *set);
/**
 * Find permission set while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_permission_set_at(const UmiPermissionSet *set, size_t index,
                                char *out_permission, size_t capacity);
/**
 * Check that permission name satisfies its contract before another service relies on it.
 */
int umi_permission_name_valid(const char *permission);
#ifdef __cplusplus
}
#endif

#endif
