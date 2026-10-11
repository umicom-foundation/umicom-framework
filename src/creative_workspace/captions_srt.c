/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions_srt.c
 * PURPOSE: Parse complete bounded subtitle documents without dropping malformed cues.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "captions_internal.h"
#include <stdlib.h>
#include <string.h>
typedef struct CaptionReader
{
    const unsigned char *bytes;
    size_t size, cursor;
} CaptionReader;
/* Each line borrows the original byte span. CRLF is normalised at the boundary;
 * lone carriage returns remain in the span and are refused by the caller. */
static bool Line(CaptionReader *reader, const unsigned char **out, size_t *length)
{
    if (reader->cursor == reader->size)
        return false;
    size_t start = reader->cursor;
    while (reader->cursor < reader->size && reader->bytes[reader->cursor] != '\n')
        ++reader->cursor;
    size_t end = reader->cursor;
    if (reader->cursor < reader->size)
    {
        ++reader->cursor;
        if (end > start && reader->bytes[end - 1U] == '\r')
            --end;
    }
    *out = reader->bytes + start;
    *length = end - start;
    return true;
}
/* Numeric identifiers are labels, not array indices. Checked accumulation
 * accepts nonconsecutive identifiers without accepting overflow or signs. */
static bool Identifier(const unsigned char *text, size_t length, uint32_t *out)
{
    if (!length)
        return false;
    uint32_t value = 0;
    for (size_t i = 0; i < length; ++i)
    {
        if (text[i] < '0' || text[i] > '9')
            return false;
        uint32_t digit = (uint32_t)(text[i] - '0');
        if (value > (UINT32_MAX - digit) / 10U)
            return false;
        value = value * 10U + digit;
    }
    if (!value)
        return false;
    *out = value;
    return true;
}
/* SRT stores integer milliseconds. Exact separators prevent locale-sensitive
 * parsing and forbid accidentally accepting malformed minutes or seconds. */
static bool Time(const unsigned char *text, uint32_t *out)
{
    if (text[2] != ':' || text[5] != ':' || text[8] != ',')
        return false;
    unsigned parts[4] = {0};
    const size_t starts[] = {0, 3, 6, 9}, counts[] = {2, 2, 2, 3};
    for (size_t p = 0; p < 4; ++p)
        for (size_t i = 0; i < counts[p]; ++i)
        {
            unsigned char c = text[starts[p] + i];
            if (c < '0' || c > '9')
                return false;
            parts[p] = parts[p] * 10U + (unsigned)(c - '0');
        }
    if (parts[1] > 59U || parts[2] > 59U)
        return false;
    *out = (uint32_t)(parts[0] * 3600000U + parts[1] * 60000U + parts[2] * 1000U + parts[3]);
    return true;
}
UmiStatus UmiCaptionTrackParseSrt(const char *title, const void *bytes, size_t length,
                                  UmiCaptionTrack **out)
{
    if (!out || *out || (length && !bytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_CAPTION_DOCUMENT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length && memchr(bytes, 0, length))
        return UMI_STATUS_PARSE_ERROR;
    UmiCaptionTrack *track = NULL;
    UmiStatus status = UmiCaptionTrackAllocate(title, &track);
    if (status != UMI_STATUS_OK)
        return status;
    CaptionReader reader = {bytes, length, 0};
    if (length >= 3U && !memcmp(bytes, "\xef\xbb\xbf", 3U))
        reader.cursor = 3U;
    const unsigned char *line = NULL;
    size_t size = 0;
    uint32_t previous = 0;
    /* Nothing becomes visible until every cue is parsed and validated. This
     * staging also keeps a damaged import from replacing a user's current edit. */
    while (Line(&reader, &line, &size))
    {
        if (!size)
            continue;
        if (track->info.cue_count == UMI_CAPTION_CUE_LIMIT)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        uint32_t number = 0;
        if (!Identifier(line, size, &number) || number <= previous)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        previous = number;
        UmiCaptionCue *cue = &track->cues[track->info.cue_count];
        if (!Line(&reader, &line, &size) || size != 29U || memcmp(line + 12U, " --> ", 5U) ||
            !Time(line, &cue->begin_ms) || !Time(line + 17U, &cue->end_ms))
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        size_t used = 0;
        while (Line(&reader, &line, &size) && size)
        {
            size_t separator = used ? 1U : 0U;
            if (separator > sizeof(cue->text) - 1U - used ||
                size > sizeof(cue->text) - 1U - used - separator)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            if (separator)
                cue->text[used++] = '\n';
            memcpy(cue->text + used, line, size);
            used += size;
            cue->text[used] = 0;
        }
        if (status != UMI_STATUS_OK)
            break;
        if (!UmiCaptionCueValid(cue) ||
            (track->info.cue_count &&
             cue->begin_ms < track->cues[track->info.cue_count - 1U].begin_ms))
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        ++track->info.cue_count;
    }
    if (status == UMI_STATUS_OK)
    {
        UmiCaptionTrackMeasure(track);
        *out = track;
    }
    else
        UmiCaptionTrackDestroy(track);
    return status;
}
