/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/audio.c
 * PURPOSE: Decode bounded PCM clips and render sample-exact ranges in C23.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/audio.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct UmiCreativeAudioClip {
    UmiCreativeAudioInfo info;
    unsigned char *pcm;
};
static uint16_t Read16(const unsigned char *p)
{ return (uint16_t)((uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8U)); }
static uint32_t Read32(const unsigned char *p)
{ return (uint32_t)p[0] | (uint32_t)p[1] << 8U | (uint32_t)p[2] << 16U | (uint32_t)p[3] << 24U; }
static void Write16(unsigned char *p, uint16_t value)
{ p[0] = (unsigned char)(value & 255U); p[1] = (unsigned char)(value >> 8U); }
static void Write32(unsigned char *p, uint32_t value)
{ for (unsigned i = 0U; i < 4U; ++i) p[i] = (unsigned char)((value >> (i * 8U)) & 255U); }
static int16_t Sample(const UmiCreativeAudioClip *clip, uint64_t frame, unsigned channel)
{
    size_t index = ((size_t)frame * clip->info.channels + channel) * 2U;
    uint16_t bits = Read16(clip->pcm + index);
    /* Avoid implementation-defined conversion of an unsigned high-bit sample. */
    int32_t value = bits <= INT16_MAX ? (int32_t)bits : (int32_t)bits - 65536;
    return (int16_t)value;
}
static bool Range(const UmiCreativeAudioClip *clip, uint64_t begin, uint64_t end)
{ return clip != NULL && begin < end && end <= clip->info.frames; }

UmiStatus UmiCreativeAudioDecode(const void *input, size_t length, UmiCreativeAudioClip **out)
{
    if (out == NULL || input == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL) return UMI_STATUS_INVALID_STATE;
    if (length > UMI_CREATIVE_EXPORT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < 12U) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *bytes = input;
    if (memcmp(bytes, "RIFF", 4U) != 0 || memcmp(bytes + 8U, "WAVE", 4U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    if ((uint64_t)Read32(bytes + 4U) + 8U != length) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *format = NULL, *data = NULL;
    size_t formatSize = 0U, dataSize = 0U, chunks = 0U, ignored = 0U, at = 12U;
    while (at < length) {
        if (length - at < 8U) return UMI_STATUS_PARSE_ERROR;
        if (++chunks > UMI_CREATIVE_AUDIO_MAX_CHUNKS) return UMI_STATUS_CAPACITY_EXCEEDED;
        size_t size = Read32(bytes + at + 4U);
        const unsigned char *id = bytes + at;
        at += 8U;
        if (size > length - at) return UMI_STATUS_PARSE_ERROR;
        size_t padded = size + (size & 1U);
        if (padded > length - at) return UMI_STATUS_PARSE_ERROR;
        if (memcmp(id, "fmt ", 4U) == 0) {
            if (format != NULL) return UMI_STATUS_PARSE_ERROR;
            format = bytes + at; formatSize = size;
        } else if (memcmp(id, "data", 4U) == 0) {
            if (data != NULL) return UMI_STATUS_PARSE_ERROR;
            data = bytes + at; dataSize = size;
        } else ++ignored;
        at += padded;
    }
    if (format == NULL || data == NULL || (formatSize != 16U && formatSize != 18U))
        return UMI_STATUS_PARSE_ERROR;
    if (Read16(format) != 1U || Read16(format + 14U) != 16U ||
        (formatSize == 18U && Read16(format + 16U) != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    uint16_t channels = Read16(format + 2U);
    uint32_t rate = Read32(format + 4U);
    if ((channels != 1U && channels != 2U) || rate < 8000U || rate > 192000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint16_t block = (uint16_t)(channels * 2U);
    if (Read16(format + 12U) != block || Read32(format + 8U) != rate * block ||
        dataSize == 0U || dataSize % block != 0U) return UMI_STATUS_PARSE_ERROR;
    if (dataSize > UMI_CREATIVE_EXPORT_LIMIT - 44U) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCreativeAudioClip *clip = calloc(1U, sizeof(*clip));
    if (clip == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    clip->pcm = malloc(dataSize);
    if (clip->pcm == NULL) { free(clip); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(clip->pcm, data, dataSize);
    clip->info = (UmiCreativeAudioInfo){rate, channels, dataSize / block, dataSize, ignored};
    *out = clip;
    return UMI_STATUS_OK;
}
void UmiCreativeAudioDestroy(UmiCreativeAudioClip *clip)
{ if (clip != NULL) { free(clip->pcm); free(clip); } }
UmiStatus UmiCreativeAudioGetInfo(const UmiCreativeAudioClip *clip, UmiCreativeAudioInfo *out)
{
    if (clip == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = clip->info; return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioSample(const UmiCreativeAudioClip *clip, uint64_t frame,
    unsigned channel, int16_t *out)
{
    if (clip == NULL || out == NULL || frame >= clip->info.frames || channel >= clip->info.channels)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = Sample(clip, frame, channel); return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioInspect(const UmiCreativeAudioClip *clip, uint64_t begin,
    uint64_t end, size_t requestedBins, UmiCreativeAudioOverview *out)
{
    if (!Range(clip, begin, end) || out == NULL || requestedBins == 0U ||
        requestedBins > UMI_CREATIVE_AUDIO_MAX_BINS) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep array bounds explicit here as well as at the import boundary. */
    if (clip->info.channels == 0U || clip->info.channels > 2U) return UMI_STATUS_INVALID_STATE;
    UmiCreativeAudioOverview result = {0};
    uint64_t frames = end - begin, squares[2] = {0U, 0U};
    result.source = clip->info; result.beginFrame = begin; result.endFrame = end;
    result.binCount = frames < requestedBins ? (size_t)frames : requestedBins;
    for (size_t i = 0U; i < result.binCount; ++i) {
        UmiCreativeAudioBin *bin = &result.bins[i];
        /* PCM is bounded to 4 MiB, so frames*binCount cannot overflow uint64_t. */
        bin->beginFrame = begin + frames * i / result.binCount;
        bin->endFrame = begin + frames * (i + 1U) / result.binCount;
        for (unsigned c = 0U; c < clip->info.channels; ++c) {
            bin->minimum[c] = INT16_MAX; bin->maximum[c] = INT16_MIN;
            for (uint64_t f = bin->beginFrame; f < bin->endFrame; ++f) {
                int32_t value = Sample(clip, f, c);
                uint32_t magnitude = (uint32_t)(value < 0 ? -value : value);
                if (value < bin->minimum[c]) bin->minimum[c] = (int16_t)value;
                if (value > bin->maximum[c]) bin->maximum[c] = (int16_t)value;
                if (magnitude > result.peakMagnitude[c]) result.peakMagnitude[c] = magnitude;
                squares[c] += (uint64_t)magnitude * magnitude;
            }
        }
    }
    for (unsigned c = 0U; c < clip->info.channels; ++c)
        result.rms[c] = sqrt((double)squares[c] / (double)frames);
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioRender(const UmiCreativeAudioClip *clip,
    const UmiCreativeAudioEdit *edit, UmiCreativeExport *out)
{
    if (out == NULL || edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (out->bytes != NULL || out->size != 0U) return UMI_STATUS_INVALID_STATE;
    if (!Range(clip, edit->beginFrame, edit->endFrame) || edit->gainPermille > 1000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t frames = edit->endFrame - edit->beginFrame;
    if (edit->fadeInFrames > frames || edit->fadeOutFrames > frames) return UMI_STATUS_INVALID_ARGUMENT;
    size_t dataSize = (size_t)frames * clip->info.channels * 2U, total = dataSize + 44U;
    unsigned char *bytes = calloc(total, 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(bytes, "RIFF", 4U); Write32(bytes + 4U, (uint32_t)(total - 8U));
    memcpy(bytes + 8U, "WAVEfmt ", 8U); Write32(bytes + 16U, 16U);
    Write16(bytes + 20U, 1U); Write16(bytes + 22U, clip->info.channels);
    Write32(bytes + 24U, clip->info.sampleRate);
    Write32(bytes + 28U, clip->info.sampleRate * clip->info.channels * 2U);
    Write16(bytes + 32U, (uint16_t)(clip->info.channels * 2U)); Write16(bytes + 34U, 16U);
    memcpy(bytes + 36U, "data", 4U); Write32(bytes + 40U, (uint32_t)dataSize);
    for (uint64_t f = 0U; f < frames; ++f) {
        for (unsigned c = 0U; c < clip->info.channels; ++c) {
            int64_t value = (int64_t)Sample(clip, edit->beginFrame + f, c) * edit->gainPermille / 1000;
            if (f < edit->fadeInFrames)
                value = edit->fadeInFrames == 1U ? 0 : value * (int64_t)f / (int64_t)(edit->fadeInFrames - 1U);
            uint64_t remaining = frames - 1U - f;
            if (remaining < edit->fadeOutFrames)
                value = edit->fadeOutFrames == 1U ? 0 : value * (int64_t)remaining / (int64_t)(edit->fadeOutFrames - 1U);
            size_t offset = 44U + ((size_t)f * clip->info.channels + c) * 2U;
            Write16(bytes + offset, (uint16_t)(int16_t)value);
        }
    }
    out->bytes = bytes; out->size = total;
    return UMI_STATUS_OK;
}
