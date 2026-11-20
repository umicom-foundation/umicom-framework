/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_probe.c
 *
 * PURPOSE:
 *   Project bounded independent media metadata without trusting encoder state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/media_probe.h"

#include "umicom/base/ascii_number.h"
#include "umicom/language_runtime/json_tree.h"
#include <string.h>

static UmiStatus ReadSeconds(const char *text, bool signed_time, int64_t *out)
{
    const bool negative = text[0] == '-';
    if (negative && !signed_time) {
        return UMI_STATUS_PARSE_ERROR;
    }
    const char *start = text + (negative ? 1U : 0U);
    const char *point = strchr(start, '.');
    const size_t integral_length = point == NULL ? strlen(start) : (size_t)(point - start);
    uint64_t seconds = 0U;
    if (integral_length == 0U) {
        return UMI_STATUS_PARSE_ERROR;
    }
    UmiStatus status = umi_ascii_uint64_parse(start, integral_length, &seconds);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    uint64_t microseconds = 0U;
    if (point != NULL) {
        const size_t fraction_length = strlen(point + 1);
        if (fraction_length == 0U || fraction_length > 6U) {
            return UMI_STATUS_PARSE_ERROR;
        }
        status = umi_ascii_uint64_parse(point + 1, fraction_length, &microseconds);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        for (size_t places = fraction_length; places < 6U; ++places) {
            microseconds *= 10U;
        }
    }

    /* Multiplication must fit before adding the fractional part. A negative
     * start is legitimate when presentation begins before the zero timestamp. */
    const uint64_t limit = (uint64_t)INT64_MAX + (negative ? 1U : 0U);
    if (seconds > (limit - microseconds) / UINT64_C(1000000)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    const uint64_t magnitude = seconds * UINT64_C(1000000) + microseconds;
    if (negative && magnitude == (uint64_t)INT64_MAX + 1U) {
        *out = INT64_MIN;
    } else {
        *out = negative ? -(int64_t)magnitude : (int64_t)magnitude;
    }
    return UMI_STATUS_OK;
}

static UmiStatus ReadText(const UmiJsonTree *tree, int object, const char *key,
    char *text, size_t capacity)
{
    int node = 0;
    const UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING) {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UmiJsonTreeText(tree, node, text, capacity);
}

static UmiStatus ReadTime(const UmiJsonTree *tree, int object, const char *key,
    bool signed_time, UmiMediaProbeTime *out)
{
    char text[40];
    UmiStatus status = ReadText(tree, object, key, text, sizeof text);
    if (status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK || strcmp(text, "N/A") == 0) {
        return status;
    }
    status = ReadSeconds(text, signed_time, &out->microseconds);
    out->known = status == UMI_STATUS_OK;
    return status;
}

static UmiStatus ReadUnsigned(const UmiJsonTree *tree, int object, const char *key, uint32_t *out)
{
    int node = 0;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    int64_t value = 0;
    if (status == UMI_STATUS_OK) {
        status = UmiJsonTreeInteger(tree, node, &value);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (value < 0 || (uint64_t)value > UINT32_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out = (uint32_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus ReadDimensions(const UmiJsonTree *tree, int object, UmiMediaProbeStream *stream)
{
    const UmiStatus width_status = ReadUnsigned(tree, object, "width", &stream->width);
    const UmiStatus height_status = ReadUnsigned(tree, object, "height", &stream->height);
    if (width_status == UMI_STATUS_NOT_FOUND && height_status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_OK;
    }
    if (width_status == UMI_STATUS_NOT_FOUND || height_status == UMI_STATUS_NOT_FOUND) {
        return UMI_STATUS_PARSE_ERROR;
    }
    if (width_status != UMI_STATUS_OK || height_status != UMI_STATUS_OK) {
        return width_status != UMI_STATUS_OK ? width_status : height_status;
    }
    return (stream->width == 0U) != (stream->height == 0U) ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
}

static UmiStatus ReadStream(const UmiJsonTree *tree, int object, UmiMediaProbeStream *stream)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        return UMI_STATUS_PARSE_ERROR;
    }
    UmiStatus status = ReadUnsigned(tree, object, "index", &stream->index);
    char type[32];
    if (status == UMI_STATUS_OK) {
        status = ReadText(tree, object, "codec_type", type, sizeof type);
    }
    if (status != UMI_STATUS_OK) {
        return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
    }
    if (strcmp(type, "video") == 0) {
        stream->kind = UMI_MEDIA_PROBE_VIDEO;
        status = ReadDimensions(tree, object, stream);
    } else if (strcmp(type, "audio") == 0) {
        stream->kind = UMI_MEDIA_PROBE_AUDIO;
    } else if (strcmp(type, "subtitle") == 0) {
        stream->kind = UMI_MEDIA_PROBE_SUBTITLE;
    }
    if (status == UMI_STATUS_OK) {
        status = ReadTime(tree, object, "start_time", true, &stream->start);
    }
    return status == UMI_STATUS_OK ? ReadTime(tree, object, "duration", false, &stream->duration) : status;
}

UmiStatus UmiMediaProbeParse(const char *bytes, size_t length, UmiMediaProbeReport *out)
{
    if (bytes == NULL || out == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length > UMI_MEDIA_PROBE_MAX_BYTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    const UmiJsonTreeLimits limits = {UMI_MEDIA_PROBE_MAX_BYTES, 2048U, 12U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UmiMediaProbeReport report = {0};
    int streams = 0;
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        status = UMI_STATUS_PARSE_ERROR;
    } else {
        status = UmiJsonTreeValidateUniqueKeys(tree, 0, NULL);
    }
    if (status == UMI_STATUS_OK) {
        status = UmiJsonTreeMember(tree, 0, "streams", &streams);
        if (status == UMI_STATUS_NOT_FOUND ||
            (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, streams) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)) {
            status = UMI_STATUS_PARSE_ERROR;
        }
    }
    if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, streams) > UMI_MEDIA_PROBE_MAX_STREAMS) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK) {
        for (int child = UmiJsonTreeFirst(tree, streams); status == UMI_STATUS_OK && child >= 0;
            child = UmiJsonTreeNext(tree, child), ++report.stream_count) {
            UmiMediaProbeStream *stream = &report.streams[report.stream_count];
            status = ReadStream(tree, child, stream);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < report.stream_count; ++i) {
                if (report.streams[i].index == stream->index) {
                    status = UMI_STATUS_ALREADY_EXISTS;
                }
            }
        }
    }
    if (status == UMI_STATUS_OK) {
        int format = 0;
        const UmiStatus format_status = UmiJsonTreeMember(tree, 0, "format", &format);
        if (format_status == UMI_STATUS_OK) {
            status = UmiJsonTreeKind(tree, format) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT
                ? ReadTime(tree, format, "duration", false, &report.duration) : UMI_STATUS_PARSE_ERROR;
        } else if (format_status != UMI_STATUS_NOT_FOUND) {
            status = format_status;
        }
    }
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK) {
        *out = report;
    }
    return status;
}
