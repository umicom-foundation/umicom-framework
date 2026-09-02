/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_output_buffer.c
 *
 * PURPOSE:
 *   Implement the test output buffer behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/diagnostics/output_buffer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
