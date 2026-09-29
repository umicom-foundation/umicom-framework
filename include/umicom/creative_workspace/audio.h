/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/audio.h
 * PURPOSE: Own, inspect and trim PCM-WAVE clips without changing their source.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_AUDIO_H
#define UMICOM_CREATIVE_WORKSPACE_AUDIO_H
#include "umicom/creative_workspace/export.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CREATIVE_AUDIO_MAX_BINS 512U
#define UMI_CREATIVE_AUDIO_MAX_CHUNKS 256U

typedef struct UmiCreativeAudioClip UmiCreativeAudioClip;
typedef struct UmiCreativeAudioInfo {
    uint32_t sampleRate;
    uint16_t channels;
    uint64_t frames;
    size_t pcmBytes;
    size_t ignoredChunks; /* Optional metadata is not copied to rendered WAVE. */
} UmiCreativeAudioInfo;
typedef struct UmiCreativeAudioEdit {
    uint64_t beginFrame, endFrame; /* Half-open: begin is included, end is not. */
    unsigned gainPermille;         /* 0..1000; attenuation only, never amplification. */
    uint64_t fadeInFrames, fadeOutFrames;
} UmiCreativeAudioEdit;
typedef struct UmiCreativeAudioBin {
    uint64_t beginFrame, endFrame;
    int16_t minimum[2], maximum[2]; /* Unused mono channel remains zero. */
} UmiCreativeAudioBin;
typedef struct UmiCreativeAudioOverview {
    UmiCreativeAudioInfo source;
    uint64_t beginFrame, endFrame;
    size_t binCount;
    uint32_t peakMagnitude[2]; /* 32768 is valid for -32768. */
    double rms[2];             /* Linear PCM magnitude, not loudness or dB. */
    UmiCreativeAudioBin bins[UMI_CREATIVE_AUDIO_MAX_BINS];
} UmiCreativeAudioOverview;

/* Input: RIFF/WAVE, format tag 1, signed little-endian 16-bit PCM, one or two
 * channels, 8000..192000 Hz; at most UMI_CREATIVE_EXPORT_LIMIT file bytes.
 * Exactly one fmt and data chunk, whole frames and exact RIFF length required.
 * Unknown bounded chunks are counted and ignored. RF64, RIFX, extensible,
 * compressed and floating formats are not silently converted.
 * Initialise *outClip = NULL; ownership transfers only on success. The clip
 * copies PCM bytes and is immutable. Concurrent reads require the host to keep
 * it alive; Destroy must never race with a reader. No Data Server is opened. */
UmiStatus UmiCreativeAudioDecode(const void *bytes, size_t length,
    UmiCreativeAudioClip **outClip);
void UmiCreativeAudioDestroy(UmiCreativeAudioClip *clip);
UmiStatus UmiCreativeAudioGetInfo(const UmiCreativeAudioClip *clip,
    UmiCreativeAudioInfo *outInfo);
UmiStatus UmiCreativeAudioSample(const UmiCreativeAudioClip *clip,
    uint64_t frame, unsigned channel, int16_t *outSample);
/* A nonempty range is divided into min(requestedBins, frameCount) nonempty,
 * contiguous bins. No allocation; output is unchanged on failure. */
UmiStatus UmiCreativeAudioInspect(const UmiCreativeAudioClip *clip,
    uint64_t beginFrame, uint64_t endFrame, size_t requestedBins,
    UmiCreativeAudioOverview *outOverview);
/* Produces a new minimal fmt/data WAVE, retaining rate/channel order. Full-range
 * unity rendering is sample-exact (not metadata-exact). Gains are applied in
 * order: attenuation, fade-in, fade-out; each signed division truncates toward
 * zero. A fade-in of F>1 frames goes from zero to unity in F-1 intervals;
 * fade-out follows the reverse progression. F=1 silences only its endpoint. Overlapping fades multiply in that order.
 * Initialise outExport to {0}; live output is refused, not freed or replaced.
 * No file, process or device I/O. Use existing UmiCreativeExportWriteNew for an
 * explicit new-file export. No source is edited and no musical notes are saved. */
UmiStatus UmiCreativeAudioRender(const UmiCreativeAudioClip *clip,
    const UmiCreativeAudioEdit *edit, UmiCreativeExport *outExport);
/* Explicit synchronous local-file read. Absolute path, regular disk file,
 * bounded size; final symlinks/reparse points and special devices are refused.
 * Parent-directory symlinks/mounts are not a security sandbox. POSIX mutation
 * checks are best effort, not protection against a hostile same-user writer.
 * This copies the source once; later source changes do not update the clip. */
UmiStatus UmiCreativeAudioLoadFile(const char *absolutePath,
    UmiCreativeAudioClip **outClip);
#ifdef __cplusplus
}
#endif
#endif
