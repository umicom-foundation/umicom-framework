/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/captions_webvtt.c
 * PURPOSE: Import portable WebVTT captions through the shared immutable caption owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "captions_internal.h"
#include <string.h>

typedef struct VttReader
{
    const unsigned char *bytes;
    size_t size, cursor;
} VttReader;

/* Borrow bounded lines so file extensions and NUL-termination never determine
 * what is parsed. Normalise CRLF, while refusing lone CR at the document edge. */
static bool VttLine(VttReader *reader, const unsigned char **out, size_t *size)
{
    if (reader->cursor == reader->size)
        return false;
    size_t first = reader->cursor;
    while (reader->cursor < reader->size && reader->bytes[reader->cursor] != '\n')
        ++reader->cursor;
    size_t last = reader->cursor;
    if (reader->cursor < reader->size)
    {
        ++reader->cursor;
        if (last > first && reader->bytes[last - 1U] == '\r')
            --last;
    }
    *out = reader->bytes + first;
    *size = last - first;
    return true;
}

/* Millisecond integers avoid locale-dependent decimal parsing and rounding.
 * Check the bound before multiplying even when an input has many hour digits. */
static bool Digits(const unsigned char *text, size_t size, uint32_t limit, uint32_t *out)
{
    if (!size)
        return false;
    uint32_t value = 0;
    for (size_t i = 0; i < size; ++i)
    {
        if (text[i] < '0' || text[i] > '9')
            return false;
        uint32_t digit = (uint32_t)(text[i] - '0');
        if (digit > limit || value > (limit - digit) / 10U)
            return false;
        value = value * 10U + digit;
    }
    *out = value;
    return true;
}
static bool VttTime(const unsigned char *text, size_t size, uint32_t *out)
{
    uint32_t hours = 0, minutes = 0, seconds = 0, millis = 0;
    size_t offset = 0;
    if (size != 9U)
    {
        /* MM:SS.mmm has nine bytes; a present hour field has at least two. */
        if (size < 12U)
            return false;
        offset = size - 9U;
        if (text[offset - 1U] != ':' || !Digits(text, offset - 1U, 99U, &hours))
            return false;
    }
    if (text[offset + 2U] != ':' || text[offset + 5U] != '.' ||
        !Digits(text + offset, 2U, 59U, &minutes) ||
        !Digits(text + offset + 3U, 2U, 59U, &seconds) ||
        !Digits(text + offset + 6U, 3U, 999U, &millis))
        return false;
    *out = hours * 3600000U + minutes * 60000U + seconds * 1000U + millis;
    return true;
}
static bool Space(unsigned char c) { return c == ' ' || c == '\t'; }
/* Accept spaces or tabs around the arrow, but do not discard positioning
 * settings: the current caption owner has no place to preserve those fields. */
static bool VttTiming(const unsigned char *line, size_t size, UmiCaptionCue *cue)
{
    size_t at = 0;
    while (at < size && !Space(line[at]))
        ++at;
    if (!VttTime(line, at, &cue->begin_ms))
        return false;
    size_t first_gap = at;
    while (at < size && Space(line[at]))
        ++at;
    if (at == first_gap || size - at < 3U || memcmp(line + at, "-->", 3U))
        return false;
    at += 3U;
    size_t second_gap = at;
    while (at < size && Space(line[at]))
        ++at;
    if (at == second_gap)
        return false;
    size_t last = at;
    while (at < size && !Space(line[at]))
        ++at;
    if (!VttTime(line + last, at - last, &cue->end_ms))
        return false;
    while (at < size && Space(line[at]))
        ++at;
    return at == size;
}
UmiStatus UmiCaptionTrackParseWebVtt(const char *title, const void *bytes, size_t length,
                                     UmiCaptionTrack **out)
{
    if (!out || *out || (length && !bytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_CAPTION_DOCUMENT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!length || memchr(bytes, 0, length))
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    for (size_t i = 0; i < length; ++i)
        if (input[i] == '\r' && (i + 1U == length || input[i + 1U] != '\n'))
            return UMI_STATUS_PARSE_ERROR;
    VttReader reader = {input, length, 0};
    if (length >= 3U && !memcmp(input, "\xef\xbb\xbf", 3U))
        reader.cursor = 3U;
    const unsigned char *line = NULL;
    size_t size = 0;
    if (!VttLine(&reader, &line, &size) || size != 6U || memcmp(line, "WEBVTT", 6U))
        return UMI_STATUS_PARSE_ERROR;
    if (VttLine(&reader, &line, &size) && size)
        return UMI_STATUS_PARSE_ERROR;
    UmiCaptionTrack *track = NULL;
    UmiStatus status = UmiCaptionTrackAllocate(title, &track);
    if (status != UMI_STATUS_OK)
        return status;
    uint32_t previous_label = 0;
    /* Build privately and publish once. A damaged final cue must not leave the
     * caller holding a seemingly successful import of only the earlier cues. */
    while (VttLine(&reader, &line, &size))
    {
        if (!size)
            continue;
        if (track->info.cue_count == UMI_CAPTION_CUE_LIMIT)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        uint32_t label = 0;
        if (Digits(line, size, UINT32_MAX, &label))
        {
            if (!label || label <= previous_label || !VttLine(&reader, &line, &size))
            {
                status = UMI_STATUS_PARSE_ERROR;
                break;
            }
            previous_label = label;
        }
        UmiCaptionCue *cue = &track->cues[track->info.cue_count];
        if (!VttTiming(line, size, cue))
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        size_t used = 0;
        while (VttLine(&reader, &line, &size) && size)
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
UmiStatus UmiCaptionTrackParse(const char *title, const void *bytes, size_t length,
                               UmiCaptionTrack **out)
{
    if (!out || *out || (length && !bytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_CAPTION_DOCUMENT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = bytes;
    size_t first = length >= 3U && !memcmp(input, "\xef\xbb\xbf", 3U) ? 3U : 0U;
    /* An identified format owns its errors. Falling back after a WebVTT error
     * could misinterpret a damaged document instead of showing a useful refusal. */
    if (length - first >= 6U && !memcmp(input + first, "WEBVTT", 6U))
        return UmiCaptionTrackParseWebVtt(title, bytes, length, out);
    return UmiCaptionTrackParseSrt(title, bytes, length, out);
}
