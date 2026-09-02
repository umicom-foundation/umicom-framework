/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/permission_catalogue.h
 *
 * PURPOSE:
 *   Publish the public permission catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_PERMISSION_CATALOGUE_H
#define UMICOM_SECURITY_PERMISSION_CATALOGUE_H
#include "umicom/security/governance.h"
/**
 * Represent the security permission descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityPermissionDescriptor {
    char name[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char description[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
    UmiSecurityRisk risk;
    bool trusted_workspace_required;
    bool approval_required;
} UmiSecurityPermissionDescriptor;
/**
 * Represent the security permission catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityPermissionCatalogue { UmiSecurityPermissionDescriptor items[UMI_SECURITY_GOVERNANCE_MAX_PERMISSIONS]; size_t count; } UmiSecurityPermissionCatalogue;
/**
 * Add security permission catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_permission_catalogue_register(UmiSecurityPermissionCatalogue *catalogue,const UmiSecurityPermissionDescriptor *permission);
/**
 * Find security permission catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiSecurityPermissionDescriptor *umi_security_permission_catalogue_find(const UmiSecurityPermissionCatalogue *catalogue,const char *name);
#endif
