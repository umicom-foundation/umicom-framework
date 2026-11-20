/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_media_probe.c
 *
 * PURPOSE:
 *   Verify independent media metadata shape, exact timing and refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/media_probe.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static int Streams(void)
{
    const char json[] =
        "{\"streams\":[{\"index\":2,\"codec_type\":\"subtitle\",\"start_time\":\"0.100000\"},"
        "{\"index\":0,\"codec_type\":\"video\",\"width\":256,\"height\":144,\"duration\":\"1.000000\"},"
        "{\"index\":1,\"codec_type\":\"audio\",\"start_time\":\"-0.125\",\"duration\":\"1.234567\"},"
        "{\"index\":3,\"codec_type\":\"attachment\"}],\"format\":{\"duration\":\"1.234567\"}}";
    UmiMediaProbeReport report;
    CHECK(UmiMediaProbeParse(json, sizeof json - 1U, &report) == UMI_STATUS_OK);
    CHECK(report.duration.known && report.duration.microseconds == 1234567 && report.stream_count == 4U);
    CHECK(report.streams[0].kind == UMI_MEDIA_PROBE_SUBTITLE && report.streams[0].index == 2U);
    CHECK(report.streams[0].start.known && report.streams[0].start.microseconds == 100000);
    CHECK(report.streams[1].kind == UMI_MEDIA_PROBE_VIDEO && report.streams[1].index == 0U);
    CHECK(report.streams[1].width == 256U && report.streams[1].height == 144U);
    CHECK(report.streams[1].duration.known && report.streams[1].duration.microseconds == 1000000);
    CHECK(report.streams[2].kind == UMI_MEDIA_PROBE_AUDIO && report.streams[2].index == 1U);
    CHECK(report.streams[2].start.known && report.streams[2].start.microseconds == -125000);
    CHECK(report.streams[2].duration.known && report.streams[2].duration.microseconds == 1234567);
    CHECK(report.streams[3].kind == UMI_MEDIA_PROBE_OTHER && report.streams[3].index == 3U);
    return 0;
}

static int Unknown(void)
{
    const char json[] =
        "{\"streams\":[{\"index\":0,\"codec_type\":\"video\","
        "\"start_time\":\"0.000000\",\"duration\":\"N/A\"}]}";
    UmiMediaProbeReport report;
    memset(&report, 0xa5, sizeof report);
    CHECK(UmiMediaProbeParse(json, sizeof json - 1U, &report) == UMI_STATUS_OK);
    CHECK(report.stream_count == 1U && !report.duration.known);
    CHECK(report.streams[0].start.known && report.streams[0].start.microseconds == 0);
    CHECK(!report.streams[0].duration.known && report.streams[0].duration.microseconds == 0);
    CHECK(report.streams[0].width == 0U && report.streams[0].height == 0U);
    return 0;
}

static int TimeRange(void)
{
    const char json[] =
        "{\"streams\":[{\"index\":0,\"codec_type\":\"audio\",\"start_time\":\"-9223372036854.775808\"}],"
        "\"format\":{\"duration\":\"9223372036854.775807\"}}";
    UmiMediaProbeReport report;
    CHECK(UmiMediaProbeParse(json, sizeof json - 1U, &report) == UMI_STATUS_OK);
    CHECK(report.duration.known && report.duration.microseconds == INT64_MAX);
    CHECK(report.streams[0].start.known && report.streams[0].start.microseconds == INT64_MIN);
    return 0;
}

static int StreamLimit(void)
{
    char json[2048] = "{\"streams\":[";
    size_t written = strlen(json);
    for (unsigned i = 0U; i <= UMI_MEDIA_PROBE_MAX_STREAMS; ++i) {
        if (i == UMI_MEDIA_PROBE_MAX_STREAMS) {
            memcpy(json + written, "]}", 3U);
            UmiMediaProbeReport allowed;
            CHECK(UmiMediaProbeParse(json, written + 2U, &allowed) == UMI_STATUS_OK);
            CHECK(allowed.stream_count == UMI_MEDIA_PROBE_MAX_STREAMS);
        }
        const int count = snprintf(json + written, sizeof json - written,
            "%s{\"index\":%u,\"codec_type\":\"audio\"}", i == 0U ? "" : ",", i);
        CHECK(count > 0 && (size_t)count < sizeof json - written - 2U);
        written += (size_t)count;
    }
    memcpy(json + written, "]}", 3U);
    UmiMediaProbeReport report;
    unsigned char before[sizeof report];
    memset(&report, 0xa5, sizeof report);
    memcpy(before, &report, sizeof before);
    CHECK(UmiMediaProbeParse(json, written + 2U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&report, before, sizeof before) == 0);
    return 0;
}

static int Refusals(const char *mode)
{
    static const struct {
        const char *mode;
        const char *json;
        UmiStatus status;
    } cases[] = {
        {"wrong-root", "[]", UMI_STATUS_PARSE_ERROR},
        {"missing-streams", "{}", UMI_STATUS_PARSE_ERROR},
        {"wrong-streams", "{\"streams\":{}}", UMI_STATUS_PARSE_ERROR},
        {"wrong-kind", "{\"streams\":[{\"index\":0,\"codec_type\":false}]}", UMI_STATUS_PARSE_ERROR},
        {"missing-index", "{\"streams\":[{\"codec_type\":\"audio\"}]}", UMI_STATUS_PARSE_ERROR},
        {"fractional-index", "{\"streams\":[{\"index\":1.0,\"codec_type\":\"audio\"}]}", UMI_STATUS_PARSE_ERROR},
        {"repeated-index", "{\"streams\":[{\"index\":1,\"codec_type\":\"audio\"},"
                           "{\"index\":1,\"codec_type\":\"video\"}]}", UMI_STATUS_ALREADY_EXISTS},
        {"duplicate-key", "{\"streams\":[],\"str\\u0065ams\":[]}", UMI_STATUS_ALREADY_EXISTS},
        {"incomplete-dimensions", "{\"streams\":[{\"index\":0,\"codec_type\":\"video\",\"height\":144}]}",
            UMI_STATUS_PARSE_ERROR},
        {"zero-dimensions", "{\"streams\":[{\"index\":0,\"codec_type\":\"video\",\"width\":0,\"height\":144}]}",
            UMI_STATUS_PARSE_ERROR},
        {"negative-duration", "{\"streams\":[],\"format\":{\"duration\":\"-1.000000\"}}", UMI_STATUS_PARSE_ERROR},
        {"time-overflow", "{\"streams\":[],\"format\":{\"duration\":\"9223372036854.775808\"}}",
            UMI_STATUS_CAPACITY_EXCEEDED},
        {"precision", "{\"streams\":[],\"format\":{\"duration\":\"0.0000001\"}}", UMI_STATUS_PARSE_ERROR},
        {"duration-type", "{\"streams\":[],\"format\":{\"duration\":1.0}}", UMI_STATUS_PARSE_ERROR},
        {"truncated", "{\"streams\":[", UMI_STATUS_PARSE_ERROR},
    };
    UmiMediaProbeReport report;
    unsigned char before[sizeof report];
    memset(&report, 0xa5, sizeof report);
    memcpy(before, &report, sizeof before);
    if (strcmp(mode, "byte-limit") == 0) {
        CHECK(UmiMediaProbeParse("", UMI_MEDIA_PROBE_MAX_BYTES + 1U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(mode, "invalid-arguments") == 0) {
        CHECK(UmiMediaProbeParse(NULL, 0U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaProbeParse("{}", 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        size_t selected = 0U;
        while (selected < sizeof cases / sizeof cases[0] && strcmp(cases[selected].mode, mode) != 0) {
            ++selected;
        }
        CHECK(selected < sizeof cases / sizeof cases[0]);
        const char *json = cases[selected].json;
        CHECK(UmiMediaProbeParse(json, strlen(json), &report) == cases[selected].status);
    }
    CHECK(memcmp(&report, before, sizeof before) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strcmp(argv[1], "streams") == 0) return Streams();
    if (strcmp(argv[1], "unknown") == 0) return Unknown();
    if (strcmp(argv[1], "time-range") == 0) return TimeRange();
    if (strcmp(argv[1], "stream-limit") == 0) return StreamLimit();
    if (strcmp(argv[1], "empty") == 0) {
        const char json[] = "{\"streams\":[],\"format\":{\"duration\":\"N/A\"}}";
        UmiMediaProbeReport report;
        CHECK(UmiMediaProbeParse(json, sizeof json - 1U, &report) == UMI_STATUS_OK);
        CHECK(report.stream_count == 0U && !report.duration.known);
        return 0;
    }
    return Refusals(argv[1]);
}
