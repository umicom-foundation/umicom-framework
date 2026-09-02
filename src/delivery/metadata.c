/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/metadata.c
 *
 * PURPOSE:
 *   Store bounded key/value release metadata used by providers and Studio views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Small metadata fields such as build host or package label remain structured instead of being concatenated into one free-form string.
 */

#include "umicom/delivery/metadata.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise delivery metadata from caller-provided values so later operations receive a
 * known state.
 */
void umi_delivery_metadata_init(UmiDeliveryMetadata *metadata)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metadata != NULL) (void)memset(metadata, 0, sizeof(*metadata));
}
/*
 * Copy delivery metadata into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_delivery_metadata_set(UmiDeliveryMetadata *metadata,
                                    const char *key,
                                    const char *value)
{
    size_t i;
    UmiDeliveryMetadataEntry *entry = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metadata == NULL || key == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < metadata->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(metadata->entries[i].key, key) == 0) {
            entry = &metadata->entries[i];
            break;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (metadata->count >= UMI_DELIVERY_MAX_METADATA) return UMI_STATUS_CAPACITY_EXCEEDED;
        entry = &metadata->entries[metadata->count++];
        (void)memset(entry, 0, sizeof(*entry));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_delivery_copy_text(entry->key, sizeof(entry->key), key) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    return umi_delivery_copy_text(entry->value, sizeof(entry->value), value);
}
/*
 * Provide the delivery metadata get operation used by this module and its client
 * applications.
 */
const char *umi_delivery_metadata_get(const UmiDeliveryMetadata *metadata,
                                      const char *key)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metadata == NULL || key == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < metadata->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(metadata->entries[i].key, key) == 0) return metadata->entries[i].value;
    }
    return NULL;
}
