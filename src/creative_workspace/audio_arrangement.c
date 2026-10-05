/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/audio_arrangement.c
 * PURPOSE: Validate reusable audio placements and render complete stereo mixes from owned library captures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/audio_arrangement.h"
#include "umicom/creative_workspace/audio.h"
#include "asset_internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

/* Validate time arithmetic before subtraction or addition. Keeping the plan
 * independent of source decoding lets native and future web editors share the
 * same form rules while rendering separately checks captured source ranges. */
static bool PlacementValid(const UmiCreativeAudioPlacement *clip)
{
    if (clip == NULL || !UmiCreativeIdValid(clip->id) || !UmiCreativeIdValid(clip->asset_id) ||
        clip->source_begin_ms >= clip->source_end_ms ||
        clip->source_end_ms > UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_TIME_MS || clip->gain_permille > 1000U)
        return false;
    uint32_t duration = clip->source_end_ms - clip->source_begin_ms;
    return clip->start_ms <= UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_TIME_MS - duration &&
           clip->fade_in_ms <= duration && clip->fade_out_ms <= duration;
}
UmiStatus UmiCreativeAudioArrangementInit(const char *title, uint32_t sample_rate,
                                          UmiCreativeAudioArrangement *out)
{
    if (out == NULL || sample_rate < 8000U || sample_rate > 192000U ||
        !UmiCreativeTextValid(title, UMI_CREATIVE_LABEL_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeAudioArrangement plan = {0};
    memcpy(plan.title, title, strlen(title) + 1U);
    plan.sample_rate = sample_rate;
    *out = plan;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioArrangementValidate(const UmiCreativeAudioArrangement *plan)
{
    if (plan == NULL || !UmiCreativeTextValid(plan->title, sizeof(plan->title), false) ||
        plan->sample_rate < 8000U || plan->sample_rate > 192000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->clip_count > UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < plan->clip_count; ++i)
    {
        if (!PlacementValid(&plan->clips[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(plan->clips[i].id, plan->clips[j].id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioArrangementPut(UmiCreativeAudioArrangement *plan,
                                         const UmiCreativeAudioPlacement *placement, bool replace_existing)
{
    UmiStatus status = UmiCreativeAudioArrangementValidate(plan);
    if (status != UMI_STATUS_OK)
        return status;
    if (!PlacementValid(placement))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = 0U;
    while (index < plan->clip_count && strcmp(plan->clips[index].id, placement->id) != 0)
        ++index;
    if (index < plan->clip_count && !replace_existing)
        return UMI_STATUS_ALREADY_EXISTS;
    if (index == plan->clip_count && replace_existing)
        return UMI_STATUS_NOT_FOUND;
    if (index == UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A value copy keeps controls and other hosts independent of caller buffers.
     * Replacing requires an explicit existing-ID choice, never an upsert. */
    plan->clips[index] = *placement;
    if (index == plan->clip_count)
        ++plan->clip_count;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioArrangementRemove(UmiCreativeAudioArrangement *plan, const char *id)
{
    UmiStatus status = UmiCreativeAudioArrangementValidate(plan);
    if (status != UMI_STATUS_OK)
        return status;
    if (!UmiCreativeIdValid(id))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = 0U;
    while (index < plan->clip_count && strcmp(plan->clips[index].id, id) != 0)
        ++index;
    if (index == plan->clip_count)
        return UMI_STATUS_NOT_FOUND;
    --plan->clip_count;
    memmove(&plan->clips[index], &plan->clips[index + 1U],
            (plan->clip_count - index) * sizeof(plan->clips[0]));
    memset(&plan->clips[plan->clip_count], 0, sizeof(plan->clips[0]));
    return UMI_STATUS_OK;
}
typedef struct Source
{
    char id[UMI_CREATIVE_ID_CAPACITY];
    UmiCreativeAudioClip *clip;
    UmiCreativeAudioInfo info;
} Source;
typedef struct Prepared
{
    size_t source;
    uint64_t start, begin, end, fade_in, fade_out;
    unsigned gain;
} Prepared;
static uint64_t Frames(uint32_t milliseconds, uint32_t rate) { return (uint64_t)milliseconds * rate / 1000U; }
static void Put16(unsigned char *out, uint16_t value)
{
    out[0] = (unsigned char)(value & 255U);
    out[1] = (unsigned char)(value >> 8U);
}
static void Put32(unsigned char *out, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i)
        out[i] = (unsigned char)((value >> (i * 8U)) & 255U);
}
/* Decode each referenced asset once, even when several placements reuse it.
 * Cache ownership stays local to this render and is released on every outcome;
 * no decoder or persistent cache is added to the application layer. */
static UmiStatus Prepare(const UmiCreativeAudioArrangement *plan, const UmiCreativeAssetLibrary *library,
                         const UmiCancellationToken *cancel, Source *sources, size_t *source_count,
                         Prepared *prepared, uint64_t *frames)
{
    for (size_t i = 0U; i < plan->clip_count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        const UmiCreativeAudioPlacement *placement = &plan->clips[i];
        size_t found = 0U;
        while (found < *source_count && strcmp(sources[found].id, placement->asset_id) != 0)
            ++found;
        if (found == *source_count)
        {
            const UmiCreativeAsset *asset = NULL;
            const void *bytes = NULL;
            size_t size = 0U;
            UmiStatus status = UmiCreativeAssetLibraryBorrow(library, placement->asset_id, &asset);
            if (status == UMI_STATUS_OK)
                status = UmiCreativeAssetBytes(asset, &bytes, &size);
            if (status == UMI_STATUS_OK)
                status = UmiCreativeAudioDecode(bytes, size, &sources[found].clip);
            if (status != UMI_STATUS_OK)
                return status;
            /* Register the new owner before any subsequent failure so cleanup
             * always releases every successfully decoded source. */
            ++*source_count;
            memcpy(sources[found].id, placement->asset_id, strlen(placement->asset_id) + 1U);
            status = UmiCreativeAudioGetInfo(sources[found].clip, &sources[found].info);
            if (status != UMI_STATUS_OK)
                return status;
            if (sources[found].info.sampleRate != plan->sample_rate)
                return UMI_STATUS_INVALID_ARGUMENT;
        }
        Prepared item = {found,
                         Frames(placement->start_ms, plan->sample_rate),
                         Frames(placement->source_begin_ms, plan->sample_rate),
                         Frames(placement->source_end_ms, plan->sample_rate),
                         Frames(placement->fade_in_ms, plan->sample_rate),
                         Frames(placement->fade_out_ms, plan->sample_rate),
                         placement->gain_permille};
        if (item.begin >= item.end || item.end > sources[found].info.frames ||
            item.fade_in > item.end - item.begin || item.fade_out > item.end - item.begin)
            return UMI_STATUS_INVALID_ARGUMENT;
        prepared[i] = item;
        uint64_t end = item.start + item.end - item.begin;
        if (end > *frames)
            *frames = end;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAudioArrangementRender(const UmiCreativeAudioArrangement *plan,
                                            const UmiCreativeAssetLibrary *library,
                                            const UmiCancellationToken *cancel, UmiCreativeAsset **out,
                                            UmiCreativeAudioArrangementReport *report)
{
    if (library == NULL || out == NULL || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiCreativeAudioArrangementValidate(plan);
    if (status != UMI_STATUS_OK)
        return status;
    if (plan->clip_count == 0U)
        return UMI_STATUS_INVALID_STATE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    Source sources[UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS] = {0};
    Prepared prepared[UMI_CREATIVE_AUDIO_ARRANGEMENT_MAX_CLIPS] = {0};
    size_t source_count = 0U;
    uint64_t frames = 0U;
    status = Prepare(plan, library, cancel, sources, &source_count, prepared, &frames);
    if (status == UMI_STATUS_OK && frames > (UMI_CREATIVE_ASSET_MAX_BYTES - 44U) / 4U)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCreativeAsset *rendered = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetPrepare(plan->title, UMI_CREATIVE_ASSET_AUDIO, cancel, &rendered);
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
    {
        size = 44U + (size_t)frames * 4U;
        rendered->bytes = calloc(size + 1U, 1U);
        if (rendered->bytes == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t clipped = 0U;
    if (status == UMI_STATUS_OK)
    {
        unsigned char *bytes = rendered->bytes;
        memcpy(bytes, "RIFF", 4U);
        Put32(bytes + 4U, (uint32_t)(size - 8U));
        memcpy(bytes + 8U, "WAVEfmt ", 8U);
        Put32(bytes + 16U, 16U);
        Put16(bytes + 20U, 1U);
        Put16(bytes + 22U, 2U);
        Put32(bytes + 24U, plan->sample_rate);
        Put32(bytes + 28U, plan->sample_rate * 4U);
        Put16(bytes + 32U, 4U);
        Put16(bytes + 34U, 16U);
        memcpy(bytes + 36U, "data", 4U);
        Put32(bytes + 40U, (uint32_t)(size - 44U));
        for (uint64_t frame = 0U; frame < frames && status == UMI_STATUS_OK; ++frame)
        {
            if (frame % 1024U == 0U && umi_cancellation_token_is_requested(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            int64_t mixed[2] = {0, 0};
            for (size_t i = 0U; i < plan->clip_count; ++i)
            {
                const Prepared *item = &prepared[i];
                uint64_t length = item->end - item->begin;
                if (frame < item->start || frame - item->start >= length)
                    continue;
                uint64_t local = frame - item->start, remaining = length - 1U - local;
                const Source *source = &sources[item->source];
                for (unsigned channel = 0U; channel < 2U; ++channel)
                {
                    int16_t sample = 0;
                    status = UmiCreativeAudioSample(source->clip, item->begin + local,
                                                    source->info.channels == 1U ? 0U : channel, &sample);
                    if (status != UMI_STATUS_OK)
                        break;
                    int64_t value = (int64_t)sample * item->gain / 1000;
                    if (local < item->fade_in)
                        value =
                            item->fade_in == 1U ? 0 : value * (int64_t)local / (int64_t)(item->fade_in - 1U);
                    if (remaining < item->fade_out)
                        value = item->fade_out == 1U
                                    ? 0
                                    : value * (int64_t)remaining / (int64_t)(item->fade_out - 1U);
                    mixed[channel] += value;
                }
                if (status != UMI_STATUS_OK)
                    break;
            }
            /* Summing before clipping makes the result independent of placement
             * order. Opposing signals can cancel before saturation is applied. */
            for (size_t channel = 0U; channel < 2U; ++channel)
            {
                int64_t value = mixed[channel];
                if (value > INT16_MAX)
                {
                    value = INT16_MAX;
                    ++clipped;
                }
                else if (value < INT16_MIN)
                {
                    value = INT16_MIN;
                    ++clipped;
                }
                Put16(bytes + 44U + (size_t)frame * 4U + channel * 2U, (uint16_t)(int16_t)value);
            }
        }
    }
    for (size_t i = 0U; i < source_count; ++i)
        UmiCreativeAudioDestroy(sources[i].clip);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        UmiCreativeAssetDestroy(rendered);
        return status;
    }
    rendered->info.byte_count = size;
    *report = (UmiCreativeAudioArrangementReport){plan->sample_rate, frames, clipped, plan->clip_count,
                                                  source_count};
    *out = rendered;
    return UMI_STATUS_OK;
}
