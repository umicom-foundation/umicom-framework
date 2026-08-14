/* Umicom Framework | Secret-reference inventory | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_SECRET_INVENTORY_H
#define UMICOM_SECURITY_SECRET_INVENTORY_H
#include "umicom/security/governance.h"
typedef struct UmiSecuritySecretReference {
    char reference[UMI_SECURITY_GOVERNANCE_PATH_CAPACITY];
    char provider[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char name[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
} UmiSecuritySecretReference;
typedef struct UmiSecuritySecretMetadata {
    UmiSecuritySecretReference secret;
    char purpose[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
    uint64_t rotation_due_ns;
    bool enabled;
} UmiSecuritySecretMetadata;
typedef struct UmiSecuritySecretInventory { UmiSecuritySecretMetadata items[UMI_SECURITY_GOVERNANCE_MAX_SECRETS]; size_t count; } UmiSecuritySecretInventory;
UmiStatus umi_security_secret_reference_parse(const char *reference,UmiSecuritySecretReference *out_reference);
UmiStatus umi_security_secret_inventory_add(UmiSecuritySecretInventory *inventory,const UmiSecuritySecretMetadata *metadata);
const UmiSecuritySecretMetadata *umi_security_secret_inventory_find(const UmiSecuritySecretInventory *inventory,const char *reference);
bool umi_security_secret_rotation_due(const UmiSecuritySecretMetadata *metadata,uint64_t now_ns);
#endif
