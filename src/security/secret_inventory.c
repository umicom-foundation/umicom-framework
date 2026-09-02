/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/secret_inventory.c
 *
 * PURPOSE:
 *   Implement the secret inventory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Secret-reference inventory | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/secret_inventory.h"
#include <stdio.h>
#include <string.h>
/*
 * Read security secret reference into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_security_secret_reference_parse(const char *reference,UmiSecuritySecretReference *out_reference)
{
    const char *separator;
    size_t provider_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (reference == NULL || out_reference == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    separator = strstr(reference,"://");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL || separator == reference || separator[3] == '\0') return UMI_STATUS_PARSE_ERROR;
    provider_length = (size_t)(separator - reference);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (provider_length >= sizeof(out_reference->provider) || strlen(separator + 3) >= sizeof(out_reference->name) || strlen(reference) >= sizeof(out_reference->reference)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_reference,0,sizeof(*out_reference));
    (void)memcpy(out_reference->provider,reference,provider_length);
    (void)snprintf(out_reference->name,sizeof(out_reference->name),"%s",separator + 3);
    (void)snprintf(out_reference->reference,sizeof(out_reference->reference),"%s",reference);
    return UMI_STATUS_OK;
}
/*
 * Find security secret inventory while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiSecuritySecretMetadata *umi_security_secret_inventory_find(const UmiSecuritySecretInventory *inventory,const char *reference)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || reference == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(inventory->items[index].secret.reference,reference) == 0) return &inventory->items[index];
    return NULL;
}
/*
 * Add security secret inventory only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_secret_inventory_add(UmiSecuritySecretInventory *inventory,const UmiSecuritySecretMetadata *metadata)
{
    UmiSecuritySecretReference parsed;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || metadata == NULL || metadata->secret.reference[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_security_secret_reference_parse(metadata->secret.reference,&parsed) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_security_secret_inventory_find(inventory,metadata->secret.reference) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inventory->count >= UMI_SECURITY_GOVERNANCE_MAX_SECRETS) return UMI_STATUS_CAPACITY_EXCEEDED;
    inventory->items[inventory->count] = *metadata;
    inventory->items[inventory->count].secret = parsed;
    inventory->count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the security secret rotation due operation used by this module and its client
 * applications.
 */
bool umi_security_secret_rotation_due(const UmiSecuritySecretMetadata *metadata,uint64_t now_ns) { return metadata != NULL && metadata->enabled && metadata->rotation_due_ns != 0U && now_ns >= metadata->rotation_due_ns; }
