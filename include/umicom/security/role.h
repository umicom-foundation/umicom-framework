/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/role.h
 *
 * PURPOSE:
 *   Define role metadata and permission grants that applications can reuse
 *   without embedding product-specific access rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_ROLE_H
#define UMICOM_SECURITY_ROLE_H

#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_ROLE_ID_CAPACITY 96U
#define UMI_ROLE_DESCRIPTION_CAPACITY 256U
#define UMI_ROLE_MAX 128U
#define UMI_ROLE_PERMISSION_MAX 64U
/**
 * Represent the role snapshot data shared with callers of this public contract.
 */
typedef struct UmiRoleSnapshot {
    char role_id[UMI_ROLE_ID_CAPACITY];
    char description[UMI_ROLE_DESCRIPTION_CAPACITY];
    size_t permission_count;
} UmiRoleSnapshot;
/**
 * Represent the role registry data shared with callers of this public contract.
 */
typedef struct UmiRoleRegistry UmiRoleRegistry;
/**
 * Initialise role registry from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_role_registry_create(UmiRoleRegistry **out_registry);
/**
 * Release or reset state held by role registry so the same storage can be reused safely.
 */
void umi_role_registry_destroy(UmiRoleRegistry *registry);
/**
 * Provide the role registry define operation used by this module and its client
 * applications.
 */
UmiStatus umi_role_registry_define(UmiRoleRegistry *registry,
                                   const char *role_id,
                                   const char *description);
/**
 * Provide the role registry grant operation used by this module and its client
 * applications.
 */
UmiStatus umi_role_registry_grant(UmiRoleRegistry *registry,
                                  const char *role_id,
                                  const char *permission);
/**
 * Provide the role registry revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_role_registry_revoke(UmiRoleRegistry *registry,
                                   const char *role_id,
                                   const char *permission);
/**
 * Provide the role registry allows operation used by this module and its client
 * applications.
 */
int umi_role_registry_allows(const UmiRoleRegistry *registry,
                             const char *role_id,
                             const char *permission);
/**
 * Return the number of records represented by role registry without changing their state.
 */
size_t umi_role_registry_count(const UmiRoleRegistry *registry);
/**
 * Find role registry while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_role_registry_at(const UmiRoleRegistry *registry,
                               size_t index,
                               UmiRoleSnapshot *out_role);
#ifdef __cplusplus
}
#endif

#endif
