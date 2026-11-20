/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/media_probe.h
 *
 * PURPOSE:
 *   Read independent FFprobe metadata for exported media and its streams.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_MEDIA_PROBE_H
#define UMICOM_CREATIVE_WORKSPACE_MEDIA_PROBE_H

#include "umicom/base/status.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_MEDIA_PROBE_MAX_BYTES 65536U
#define UMI_MEDIA_PROBE_MAX_STREAMS 32U

typedef enum UmiMediaProbeKind {
    UMI_MEDIA_PROBE_OTHER,
    UMI_MEDIA_PROBE_VIDEO,
    UMI_MEDIA_PROBE_AUDIO,
    UMI_MEDIA_PROBE_SUBTITLE
} UmiMediaProbeKind;

typedef struct UmiMediaProbeTime {
    int64_t microseconds;
    bool known;
} UmiMediaProbeTime;

typedef struct UmiMediaProbeStream {
    uint32_t index;
    UmiMediaProbeKind kind;
    uint32_t width;
    uint32_t height;
    UmiMediaProbeTime start;
    UmiMediaProbeTime duration;
} UmiMediaProbeStream;

typedef struct UmiMediaProbeReport {
    UmiMediaProbeTime duration;
    size_t stream_count;
    UmiMediaProbeStream streams[UMI_MEDIA_PROBE_MAX_STREAMS];
} UmiMediaProbeReport;

/* Consume the bounded JSON output of FFprobe with entries selected from
 * stream=index,codec_type,width,height,start_time,duration:format=duration.
 * The streams array is required. Missing and "N/A" times remain unknown,
 * distinct from an explicit zero. A video with no dimensions reports zero
 * for both fields. Stream indexes are unique and order is retained.
 * Output is unchanged on refusal. The API performs no file access, process
 * execution or media decoding. Metadata alone cannot verify rendered frames,
 * audible timing, caption contents or watermark appearance. */
UmiStatus UmiMediaProbeParse(const char *bytes, size_t length, UmiMediaProbeReport *out);

#ifdef __cplusplus
}
#endif

#endif
