/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_output_channels.c
 *
 * PURPOSE:
 *   Verify named output-channel aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/developer_productivity/output_channels.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperOutputChannels channels;
    UmiDeveloperOutputChannel channel;

    umi_developer_output_channels_init(&channels);
    assert(umi_developer_output_channel_append(
        &channels, "build", "Build", "hello") == UMI_STATUS_OK);
    assert(umi_developer_output_channel_find(
        &channels, "build", &channel) == UMI_STATUS_OK);
    assert(strstr(channel.text, "hello") != NULL);
    return 0;
}
