/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_server_profile.h
 *
 * PURPOSE:
 *   Describe logical Data Server operating limits and consistency defaults for deployment profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_SERVER_PROFILE_H
#define UMICOM_DATA_ENTERPRISE_DATA_SERVER_PROFILE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data server profile data shared with callers of this public contract.
 */
typedef struct UmiDataServerProfile {
    char profile_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    size_t minimum_pool_size;
    size_t maximum_pool_size;
    uint64_t query_row_limit;
    UmiDataConsistency default_consistency;
    bool migrations_enabled;
} UmiDataServerProfile;

/* Initialise a validated data server profile descriptor. */
UmiStatus umi_data_data_server_profile_init(UmiDataServerProfile *item, const char *profile_id, size_t minimum_pool_size, size_t maximum_pool_size, uint64_t query_row_limit);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_data_server_profile_validate(const UmiDataServerProfile *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_data_server_profile_archive_encode(const UmiDataServerProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_data_server_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiDataServerProfile *value);

#ifdef __cplusplus
}
#endif
#endif
