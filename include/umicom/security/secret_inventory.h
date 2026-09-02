/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/secret_inventory.h
 *
 * PURPOSE:
 *   Publish the public secret inventory contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_SECRET_INVENTORY_H
#define UMICOM_SECURITY_SECRET_INVENTORY_H
#include "umicom/security/governance.h"
/**
 * Represent the security secret reference data shared with callers of this public
 * contract.
 */
typedef struct UmiSecuritySecretReference {
    char reference[UMI_SECURITY_GOVERNANCE_PATH_CAPACITY];
    char provider[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char name[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
} UmiSecuritySecretReference;
/**
 * Represent the security secret metadata data shared with callers of this public contract.
 */
typedef struct UmiSecuritySecretMetadata {
    UmiSecuritySecretReference secret;
    char purpose[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
    uint64_t rotation_due_ns;
    bool enabled;
} UmiSecuritySecretMetadata;
/**
 * Represent the security secret inventory data shared with callers of this public
 * contract.
 */
typedef struct UmiSecuritySecretInventory { UmiSecuritySecretMetadata items[UMI_SECURITY_GOVERNANCE_MAX_SECRETS]; size_t count; } UmiSecuritySecretInventory;
/**
 * Read security secret reference into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_security_secret_reference_parse(const char *reference,UmiSecuritySecretReference *out_reference);
/**
 * Add security secret inventory only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_secret_inventory_add(UmiSecuritySecretInventory *inventory,const UmiSecuritySecretMetadata *metadata);
/**
 * Find security secret inventory while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiSecuritySecretMetadata *umi_security_secret_inventory_find(const UmiSecuritySecretInventory *inventory,const char *reference);
/**
 * Provide the security secret rotation due operation used by this module and its client
 * applications.
 */
bool umi_security_secret_rotation_due(const UmiSecuritySecretMetadata *metadata,uint64_t now_ns);
#endif
