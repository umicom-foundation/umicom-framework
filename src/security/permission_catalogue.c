/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/permission_catalogue.c
 *
 * PURPOSE:
 *   Implement the permission catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Governed permission catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/permission_catalogue.h"
#include <string.h>
const UmiSecurityPermissionDescriptor *umi_security_permission_catalogue_find(const UmiSecurityPermissionCatalogue *catalogue,const char *name)
{
    size_t index;
    if (catalogue == NULL || name == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->items[index].name,name) == 0) return &catalogue->items[index];
    return NULL;
}
UmiStatus umi_security_permission_catalogue_register(UmiSecurityPermissionCatalogue *catalogue,const UmiSecurityPermissionDescriptor *permission)
{
    if (catalogue == NULL || permission == NULL || permission->name[0] == '\0' || permission->risk < UMI_SECURITY_RISK_LOW || permission->risk > UMI_SECURITY_RISK_CRITICAL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_security_permission_catalogue_find(catalogue,permission->name) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_SECURITY_GOVERNANCE_MAX_PERMISSIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *permission;
    return UMI_STATUS_OK;
}
