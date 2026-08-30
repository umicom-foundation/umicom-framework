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
typedef struct UmiRoleSnapshot {
    char role_id[UMI_ROLE_ID_CAPACITY];
    char description[UMI_ROLE_DESCRIPTION_CAPACITY];
    size_t permission_count;
} UmiRoleSnapshot;
typedef struct UmiRoleRegistry UmiRoleRegistry;
UmiStatus umi_role_registry_create(UmiRoleRegistry **out_registry);
void umi_role_registry_destroy(UmiRoleRegistry *registry);
UmiStatus umi_role_registry_define(UmiRoleRegistry *registry,
                                   const char *role_id,
                                   const char *description);
UmiStatus umi_role_registry_grant(UmiRoleRegistry *registry,
                                  const char *role_id,
                                  const char *permission);
UmiStatus umi_role_registry_revoke(UmiRoleRegistry *registry,
                                   const char *role_id,
                                   const char *permission);
int umi_role_registry_allows(const UmiRoleRegistry *registry,
                             const char *role_id,
                             const char *permission);
size_t umi_role_registry_count(const UmiRoleRegistry *registry);
UmiStatus umi_role_registry_at(const UmiRoleRegistry *registry,
                               size_t index,
                               UmiRoleSnapshot *out_role);
#ifdef __cplusplus
}
#endif

#endif
