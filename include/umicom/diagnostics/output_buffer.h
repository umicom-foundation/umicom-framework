/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/output_buffer.h
 *
 * PURPOSE:
 *   Publish the public output buffer contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_OUTPUT_BUFFER_H
#define UMICOM_DIAGNOSTICS_OUTPUT_BUFFER_H

#include "umicom/diagnostics/output.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OUTPUT_BUFFER_DEFAULT_CAPACITY 2048U
#define UMI_OUTPUT_BUFFER_MAX_CAPACITY 65536U

/**
 * Represent the output buffer data shared with callers of this public contract.
 */
typedef struct UmiOutputBuffer UmiOutputBuffer;
/**
 * Represent the output buffer config data shared with callers of this public contract.
 */
typedef struct UmiOutputBufferConfig { size_t capacity; } UmiOutputBufferConfig;
/**
 * Represent the output buffer summary data shared with callers of this public contract.
 */
typedef struct UmiOutputBufferSummary {
    size_t retained_count;
    size_t channel_count;
    size_t by_stream[UMI_OUTPUT_STREAM_COUNT];
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t revision;
} UmiOutputBufferSummary;

/**
 * Provide the output buffer config default operation used by this module and its client
 * applications.
 */
UmiOutputBufferConfig umi_output_buffer_config_default(void);
/**
 * Initialise output buffer from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_output_buffer_create(const UmiOutputBufferConfig *config,
                                   UmiOutputBuffer **out_buffer);
/**
 * Release or reset state held by output buffer so the same storage can be reused safely.
 */
void umi_output_buffer_destroy(UmiOutputBuffer *buffer);
/**
 * Release or reset state held by output buffer so the same storage can be reused safely.
 */
void umi_output_buffer_clear(UmiOutputBuffer *buffer);
/**
 * Add output buffer only after its inputs and available capacity have been checked.
 */
UmiStatus umi_output_buffer_append(UmiOutputBuffer *buffer,
                                   const UmiOutputRecord *record);
/**
 * Find output buffer while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_output_buffer_at(const UmiOutputBuffer *buffer,
                               size_t chronological_index,
                               UmiOutputRecord *out_record);
/**
 * Provide the output buffer summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_output_buffer_summary(const UmiOutputBuffer *buffer,
                                    UmiOutputBufferSummary *out_summary);
/**
 * Return the number of records represented by output buffer without changing their state.
 */
size_t umi_output_buffer_count(const UmiOutputBuffer *buffer);
/**
 * Provide the output buffer revision operation used by this module and its client
 * applications.
 */
uint64_t umi_output_buffer_revision(const UmiOutputBuffer *buffer);

#ifdef __cplusplus
}
#endif
#endif
