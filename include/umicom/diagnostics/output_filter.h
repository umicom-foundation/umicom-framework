/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/output_filter.h
 *
 * PURPOSE:
 *   Publish the public output filter contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_OUTPUT_FILTER_H
#define UMICOM_DIAGNOSTICS_OUTPUT_FILTER_H

#include "umicom/diagnostics/output_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OUTPUT_FILTER_RESULT_MAX 128U

/**
 * Represent the output filter data shared with callers of this public contract.
 */
typedef struct UmiOutputFilter {
    uint32_t stream_mask;
    char channel_id[UMI_OUTPUT_CHANNEL_ID_CAPACITY];
    char source[UMI_OUTPUT_SOURCE_CAPACITY];
    char text[256];
    uint64_t minimum_sequence;
} UmiOutputFilter;

/**
 * Represent the output filter result data shared with callers of this public contract.
 */
typedef struct UmiOutputFilterResult {
    UmiOutputRecord items[UMI_OUTPUT_FILTER_RESULT_MAX];
    size_t count;
    size_t total_matches;
    int truncated;
} UmiOutputFilterResult;

/**
 * Initialise output filter from caller-provided values so later operations receive a known
 * state.
 */
void umi_output_filter_init(UmiOutputFilter *filter);
/**
 * Provide the output filter matches operation used by this module and its client
 * applications.
 */
int umi_output_filter_matches(const UmiOutputFilter *filter,
                              const UmiOutputRecord *record);
/**
 * Perform output filter through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_output_filter_execute(const UmiOutputBuffer *buffer,
                                    const UmiOutputFilter *filter,
                                    UmiOutputFilterResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
