/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/metadata.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_METADATA_H
#define INCLUDE_UMICOM_DELIVERY_METADATA_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the delivery metadata entry data shared with callers of this public contract.
 */
typedef struct UmiDeliveryMetadataEntry {
    char key[UMI_DELIVERY_ID_CAPACITY];
    char value[UMI_DELIVERY_TEXT_CAPACITY];
} UmiDeliveryMetadataEntry;
/**
 * Represent the delivery metadata data shared with callers of this public contract.
 */
typedef struct UmiDeliveryMetadata {
    UmiDeliveryMetadataEntry entries[UMI_DELIVERY_MAX_METADATA];
    size_t count;
} UmiDeliveryMetadata;
/**
 * Initialise delivery metadata from caller-provided values so later operations receive a
 * known state.
 */
void umi_delivery_metadata_init(UmiDeliveryMetadata *metadata);
/**
 * Copy delivery metadata into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_delivery_metadata_set(UmiDeliveryMetadata *metadata,
                                    const char *key,
                                    const char *value);
/**
 * Provide the delivery metadata get operation used by this module and its client
 * applications.
 */
const char *umi_delivery_metadata_get(const UmiDeliveryMetadata *metadata,
                                      const char *key);

#ifdef __cplusplus
}
#endif

#endif
