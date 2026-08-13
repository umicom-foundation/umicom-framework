#include <assert.h>
#include "umicom/diagnostics/output_buffer.h"
int main(void)
{
    UmiOutputBufferConfig config = {2U};
    UmiOutputBuffer *buffer = NULL;
    UmiOutputRecord record;
    UmiOutputBufferSummary summary;
    assert(umi_output_buffer_create(&config, &buffer) == UMI_STATUS_OK);
    assert(umi_output_record_init(&record, "build", "Build", "ninja",
        UMI_OUTPUT_STREAM_STANDARD, "one") == UMI_STATUS_OK);
    assert(umi_output_buffer_append(buffer, &record) == UMI_STATUS_OK);
    assert(umi_output_buffer_append(buffer, &record) == UMI_STATUS_OK);
    assert(umi_output_buffer_append(buffer, &record) == UMI_STATUS_OK);
    assert(umi_output_buffer_summary(buffer, &summary) == UMI_STATUS_OK);
    assert(summary.retained_count == 2U && summary.overwritten_count == 1U);
    assert(summary.channel_count == 1U);
    umi_output_buffer_destroy(buffer);
    return 0;
}
