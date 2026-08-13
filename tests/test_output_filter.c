#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/output_filter.h"
int main(void)
{
    UmiOutputBuffer *buffer = NULL;
    UmiOutputRecord record;
    UmiOutputFilter filter;
    UmiOutputFilterResult result;
    assert(umi_output_buffer_create(NULL, &buffer) == UMI_STATUS_OK);
    assert(umi_output_record_init(&record, "debug", "Debug", "adapter",
        UMI_OUTPUT_STREAM_DEBUG, "Breakpoint hit") == UMI_STATUS_OK);
    assert(umi_output_buffer_append(buffer, &record) == UMI_STATUS_OK);
    umi_output_filter_init(&filter);
    (void)strcpy(filter.text, "breakpoint");
    assert(umi_output_filter_execute(buffer, &filter, &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    umi_output_buffer_destroy(buffer);
    return 0;
}
