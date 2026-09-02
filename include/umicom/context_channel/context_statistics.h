/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_statistics.h
 *
 * PURPOSE:
 *   Aggregate routing and delivery statistics by channel and schema.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_STATISTICS_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_STATISTICS_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context statistics data shared with callers of this public contract.
 */
typedef struct UmiContextStatistics {
    uint32_t structure_size;
    char statistics_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char application_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextStatistics;
/**
 * Initialise context statistics from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_statistics_init(UmiContextStatistics *state);
/**
 * Provide the context statistics set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_statistics_set_field(UmiContextStatistics *state,size_t field_index,const char *value);
/**
 * Provide the context statistics field operation used by this module and its client
 * applications.
 */
const char *umi_context_statistics_field(const UmiContextStatistics *state,size_t field_index);
/**
 * Provide the context statistics record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_statistics_record_success(UmiContextStatistics *state,uint64_t sequence);
/**
 * Provide the context statistics record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_statistics_record_failure(UmiContextStatistics *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context statistics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_statistics_validate(const UmiContextStatistics *state);
/**
 * Provide the context statistics covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_statistics_covers_sequence(const UmiContextStatistics *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
