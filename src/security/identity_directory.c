/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/identity_directory.c
 *
 * PURPOSE:
 *   Implement the identity directory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Governed identity directory | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/identity_directory.h"
#include <string.h>
const UmiSecurityIdentityProfile *umi_security_identity_directory_find(const UmiSecurityIdentityDirectory *directory,const char *identity_id)
{
    size_t index;
    if (directory == NULL || identity_id == NULL) return NULL;
    for (index = 0U; index < directory->count; ++index) if (strcmp(directory->items[index].identity_id,identity_id) == 0) return &directory->items[index];
    return NULL;
}
UmiStatus umi_security_identity_directory_add(UmiSecurityIdentityDirectory *directory,const UmiSecurityIdentityProfile *identity)
{
    if (directory == NULL || identity == NULL || identity->identity_id[0] == '\0' || identity->display_name[0] == '\0' || identity->kind < UMI_IDENTITY_HUMAN || identity->kind > UMI_IDENTITY_AGENT) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_security_identity_directory_find(directory,identity->identity_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (directory->count >= UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    directory->items[directory->count] = *identity;
    directory->items[directory->count].revision = 1U;
    directory->count += 1U; directory->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_security_identity_directory_set_enabled(UmiSecurityIdentityDirectory *directory,const char *identity_id,bool enabled)
{
    size_t index;
    if (directory == NULL || identity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < directory->count; ++index) if (strcmp(directory->items[index].identity_id,identity_id) == 0) { directory->items[index].enabled = enabled; directory->items[index].revision += 1U; directory->revision += 1U; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
