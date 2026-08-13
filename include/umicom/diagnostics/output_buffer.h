/* Umicom Framework bounded output buffer. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTICS_OUTPUT_BUFFER_H
#define UMICOM_DIAGNOSTICS_OUTPUT_BUFFER_H

#include "umicom/diagnostics/output.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OUTPUT_BUFFER_DEFAULT_CAPACITY 2048U
#define UMI_OUTPUT_BUFFER_MAX_CAPACITY 65536U

typedef struct UmiOutputBuffer UmiOutputBuffer;
typedef struct UmiOutputBufferConfig { size_t capacity; } UmiOutputBufferConfig;
typedef struct UmiOutputBufferSummary {
    size_t retained_count;
    size_t channel_count;
    size_t by_stream[UMI_OUTPUT_STREAM_COUNT];
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t revision;
} UmiOutputBufferSummary;

UmiOutputBufferConfig umi_output_buffer_config_default(void);
UmiStatus umi_output_buffer_create(const UmiOutputBufferConfig *config,
                                   UmiOutputBuffer **out_buffer);
void umi_output_buffer_destroy(UmiOutputBuffer *buffer);
void umi_output_buffer_clear(UmiOutputBuffer *buffer);
UmiStatus umi_output_buffer_append(UmiOutputBuffer *buffer,
                                   const UmiOutputRecord *record);
UmiStatus umi_output_buffer_at(const UmiOutputBuffer *buffer,
                               size_t chronological_index,
                               UmiOutputRecord *out_record);
UmiStatus umi_output_buffer_summary(const UmiOutputBuffer *buffer,
                                    UmiOutputBufferSummary *out_summary);
size_t umi_output_buffer_count(const UmiOutputBuffer *buffer);
uint64_t umi_output_buffer_revision(const UmiOutputBuffer *buffer);

#ifdef __cplusplus
}
#endif
#endif
