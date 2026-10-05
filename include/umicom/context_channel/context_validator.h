/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_validator.h
 *
 * PURPOSE:
 *   Aggregate structural, schema, policy and expiry validation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_VALIDATOR_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_VALIDATOR_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context validator data shared with callers of this public contract.
 */
typedef struct UmiContextValidator {
    uint32_t structure_size;
    char validation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextValidator;
/**
 * Initialise context validator from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_validator_init(UmiContextValidator *state);
/**
 * Provide the context validator set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_validator_set_field(UmiContextValidator *state,size_t field_index,const char *value);
/**
 * Provide the context validator field operation used by this module and its client
 * applications.
 */
const char *umi_context_validator_field(const UmiContextValidator *state,size_t field_index);
/**
 * Provide the context validator record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_validator_record_success(UmiContextValidator *state,uint64_t sequence);
/**
 * Provide the context validator record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_validator_record_failure(UmiContextValidator *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context validator satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_validator_validate(const UmiContextValidator *state);
/**
 * Provide the context validator covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_validator_covers_sequence(const UmiContextValidator *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_validator_archive_encode(const UmiContextValidator *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_validator_archive_decode(const void *bytes, size_t byte_count,
    UmiContextValidator *value);

#ifdef __cplusplus
}
#endif
#endif
