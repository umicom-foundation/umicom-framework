/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/metadata.c
 *
 * PURPOSE:
 *   Store bounded key/value release metadata used by providers and Studio views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Small metadata fields such as build host or package label remain structured instead of being concatenated into one free-form string.
 */

#include "umicom/delivery/metadata.h"
#include "delivery_internal.h"
#include <string.h>
void umi_delivery_metadata_init(UmiDeliveryMetadata *metadata)
{
    if (metadata != NULL) (void)memset(metadata, 0, sizeof(*metadata));
}
UmiStatus umi_delivery_metadata_set(UmiDeliveryMetadata *metadata,
                                    const char *key,
                                    const char *value)
{
    size_t i;
    UmiDeliveryMetadataEntry *entry = NULL;
    if (metadata == NULL || key == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < metadata->count; ++i) {
        if (strcmp(metadata->entries[i].key, key) == 0) {
            entry = &metadata->entries[i];
            break;
        }
    }
    if (entry == NULL) {
        if (metadata->count >= UMI_DELIVERY_MAX_METADATA) return UMI_STATUS_CAPACITY_EXCEEDED;
        entry = &metadata->entries[metadata->count++];
        (void)memset(entry, 0, sizeof(*entry));
        if (umi_delivery_copy_text(entry->key, sizeof(entry->key), key) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    return umi_delivery_copy_text(entry->value, sizeof(entry->value), value);
}
const char *umi_delivery_metadata_get(const UmiDeliveryMetadata *metadata,
                                      const char *key)
{
    size_t i;
    if (metadata == NULL || key == NULL) return NULL;
    for (i = 0U; i < metadata->count; ++i) {
        if (strcmp(metadata->entries[i].key, key) == 0) return metadata->entries[i].value;
    }
    return NULL;
}
