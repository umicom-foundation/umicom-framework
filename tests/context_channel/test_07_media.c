/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_07_media.c
 *
 * PURPOSE:
 *   Exercise media boundaries and stable status behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../value_archive/revision_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/media.h"
#include "umicom/context_channel/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char destination[32];
    assert(umi_context_copy_text(destination,sizeof(destination),"context") == UMI_STATUS_OK);
    assert(strcmp(destination,"context") == 0);
    assert(umi_context_text_is_valid(destination,sizeof(destination)));
    assert(umi_context_kind_text(UMI_CONTEXT_KIND_INSTRUMENT) != NULL);
    assert(umi_context_colour_text(UMI_CONTEXT_COLOUR_BLUE) != NULL);

    /* Exercise the context owner itself, including its revision publication. */
    UmiMediaContext context;
    umi_media_context_init(&context);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiMediaContext, &context, revision,
        umi_media_context_set_asset_id(&context, "asset-1"));
    assert(umi_media_context_set_asset_id(&context, "asset-1") == UMI_STATUS_OK);
    assert(strcmp(context.asset_id, "asset-1") == 0);
    assert(umi_media_context_set_asset_id(NULL, "asset-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiMediaContext, &context, revision,
        umi_media_context_set_timeline_id(&context, "timeline-1"));
    assert(umi_media_context_set_timeline_id(&context, "timeline-1") == UMI_STATUS_OK);
    assert(strcmp(context.timeline_id, "timeline-1") == 0);
    assert(umi_media_context_set_timeline_id(NULL, "timeline-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiMediaContext, &context, revision,
        umi_media_context_set_track_id(&context, "track-1"));
    assert(umi_media_context_set_track_id(&context, "track-1") == UMI_STATUS_OK);
    assert(strcmp(context.track_id, "track-1") == 0);
    assert(umi_media_context_set_track_id(NULL, "track-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiMediaContext, &context, revision,
        umi_media_context_set_media_type(&context, "video"));
    assert(umi_media_context_set_media_type(&context, "video") == UMI_STATUS_OK);
    assert(strcmp(context.media_type, "video") == 0);
    assert(umi_media_context_set_media_type(NULL, "video") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiMediaContext, &context, revision,
        umi_media_context_set_timecode_ms(&context, 500U));
    assert(umi_media_context_set_timecode_ms(&context, 500U) == UMI_STATUS_OK);
    assert(context.timecode_ms == 500U);
    assert(umi_media_context_set_timecode_ms(NULL, 500U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiMediaContext, &context, revision,
        umi_media_context_set_duration_ms(&context, 2000U));
    assert(umi_media_context_set_duration_ms(&context, 2000U) == UMI_STATUS_OK);
    assert(context.duration_ms == 2000U);
    assert(umi_media_context_set_duration_ms(NULL, 2000U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(context.revision == 7U);
    assert(umi_media_context_validate(&context) == UMI_STATUS_OK);
    return 0;
}
