/* Umicom Framework output filtering. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTICS_OUTPUT_FILTER_H
#define UMICOM_DIAGNOSTICS_OUTPUT_FILTER_H

#include "umicom/diagnostics/output_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OUTPUT_FILTER_RESULT_MAX 128U

typedef struct UmiOutputFilter {
    uint32_t stream_mask;
    char channel_id[UMI_OUTPUT_CHANNEL_ID_CAPACITY];
    char source[UMI_OUTPUT_SOURCE_CAPACITY];
    char text[256];
    uint64_t minimum_sequence;
} UmiOutputFilter;

typedef struct UmiOutputFilterResult {
    UmiOutputRecord items[UMI_OUTPUT_FILTER_RESULT_MAX];
    size_t count;
    size_t total_matches;
    int truncated;
} UmiOutputFilterResult;

void umi_output_filter_init(UmiOutputFilter *filter);
int umi_output_filter_matches(const UmiOutputFilter *filter,
                              const UmiOutputRecord *record);
UmiStatus umi_output_filter_execute(const UmiOutputBuffer *buffer,
                                    const UmiOutputFilter *filter,
                                    UmiOutputFilterResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
