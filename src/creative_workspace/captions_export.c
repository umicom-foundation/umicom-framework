/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions_export.c
 * PURPOSE: Export portable caption assets through the existing bounded creative writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "captions_internal.h"
#include <stdio.h>
#include <stdlib.h>
/* Both formats use integer time. Only the decimal separator differs; hours
 * remain explicit so a browser and a desktop editor read the same timeline. */
static void Time(UmiCreativeWriter *writer, uint32_t value, char separator)
{
    char text[32];
    int size = snprintf(text, sizeof(text), "%02u:%02u:%02u%c%03u", (unsigned)(value / 3600000U),
                        (unsigned)((value / 60000U) % 60U), (unsigned)((value / 1000U) % 60U),
                        separator, (unsigned)(value % 1000U));
    if (size < 0 || (size_t)size >= sizeof(text))
        writer->status = UMI_STATUS_INTERNAL_ERROR;
    else
        UmiCreativeWriterAppend(writer, text, (size_t)size);
}
UmiStatus UmiCaptionTrackExport(const UmiCaptionTrack *track, UmiCaptionFormat format,
                                const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (!track || !out || *out || (format != UMI_CAPTION_SRT && format != UMI_CAPTION_WEBVTT))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiCreativeWriter writer;
    UmiCreativeWriterInit(&writer, UMI_CAPTION_DOCUMENT_LIMIT);
    if (format == UMI_CAPTION_WEBVTT)
        UmiCreativeWriterText(&writer, "WEBVTT\n\n");
    for (size_t i = 0; i < track->info.cue_count && writer.status == UMI_STATUS_OK; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            writer.status = UMI_STATUS_CANCELLED;
            break;
        }
        UmiCreativeWriterNumber(&writer, (uint64_t)i + 1U);
        UmiCreativeWriterText(&writer, "\n");
        Time(&writer, track->cues[i].begin_ms, format == UMI_CAPTION_SRT ? ',' : '.');
        UmiCreativeWriterText(&writer, " --> ");
        Time(&writer, track->cues[i].end_ms, format == UMI_CAPTION_SRT ? ',' : '.');
        UmiCreativeWriterText(&writer, "\n");
        UmiCreativeWriterText(&writer, track->cues[i].text);
        UmiCreativeWriterText(&writer, "\n\n");
    }
    UmiStatus status = writer.status;
    /* Capture copies the finished bytes. The same asset can then be saved by
     * a native worker or delivered by a future web adapter without a second
     * subtitle implementation or an implicit upload. */
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetCapture(track->info.title, UMI_CREATIVE_ASSET_DOCUMENT,
                                         writer.bytes, writer.size, cancel, out);
    free(writer.bytes);
    return status;
}
