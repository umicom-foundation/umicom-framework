/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/identity_directory.h
 *
 * PURPOSE:
 *   Publish the public identity directory contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_IDENTITY_DIRECTORY_H
#define UMICOM_SECURITY_IDENTITY_DIRECTORY_H
#include "umicom/security/governance.h"
#include "umicom/security/identity.h"
typedef struct UmiSecurityIdentityProfile {
    char identity_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char display_name[UMI_SECURITY_GOVERNANCE_NAME_CAPACITY];
    char organisation[UMI_SECURITY_GOVERNANCE_NAME_CAPACITY];
    UmiIdentityKind kind;
    unsigned assurance_level;
    bool enabled;
    uint64_t revision;
} UmiSecurityIdentityProfile;
typedef struct UmiSecurityIdentityDirectory { UmiSecurityIdentityProfile items[UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES]; size_t count; uint64_t revision; } UmiSecurityIdentityDirectory;
UmiStatus umi_security_identity_directory_add(UmiSecurityIdentityDirectory *directory,const UmiSecurityIdentityProfile *identity);
const UmiSecurityIdentityProfile *umi_security_identity_directory_find(const UmiSecurityIdentityDirectory *directory,const char *identity_id);
UmiStatus umi_security_identity_directory_set_enabled(UmiSecurityIdentityDirectory *directory,const char *identity_id,bool enabled);
#endif
