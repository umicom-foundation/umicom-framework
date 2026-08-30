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
typedef struct UmiSecurityPermissionDescriptor {
    char name[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char description[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
    UmiSecurityRisk risk;
    bool trusted_workspace_required;
    bool approval_required;
} UmiSecurityPermissionDescriptor;
typedef struct UmiSecurityPermissionCatalogue { UmiSecurityPermissionDescriptor items[UMI_SECURITY_GOVERNANCE_MAX_PERMISSIONS]; size_t count; } UmiSecurityPermissionCatalogue;
UmiStatus umi_security_permission_catalogue_register(UmiSecurityPermissionCatalogue *catalogue,const UmiSecurityPermissionDescriptor *permission);
const UmiSecurityPermissionDescriptor *umi_security_permission_catalogue_find(const UmiSecurityPermissionCatalogue *catalogue,const char *name);
#endif
