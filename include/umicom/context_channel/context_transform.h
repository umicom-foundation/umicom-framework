/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_transform.h
 *
 * PURPOSE:
 *   Record deterministic schema transformation plans for context values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSFORM_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSFORM_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context transform data shared with callers of this public contract.
 */
typedef struct UmiContextTransform {
    uint32_t structure_size;
    char plan_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char target_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char transformer_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextTransform;
/**
 * Initialise context transform from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_transform_init(UmiContextTransform *state);
/**
 * Provide the context transform set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_transform_set_field(UmiContextTransform *state,size_t field_index,const char *value);
/**
 * Provide the context transform field operation used by this module and its client
 * applications.
 */
const char *umi_context_transform_field(const UmiContextTransform *state,size_t field_index);
/**
 * Provide the context transform record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transform_record_success(UmiContextTransform *state,uint64_t sequence);
/**
 * Provide the context transform record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transform_record_failure(UmiContextTransform *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context transform satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_transform_validate(const UmiContextTransform *state);
/**
 * Provide the context transform covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_transform_covers_sequence(const UmiContextTransform *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
