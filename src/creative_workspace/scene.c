/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/scene.c
 * PURPOSE: Sample scene positions through canonical animation and perform exact bounded hit tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <math.h>
#include <string.h>
static UmiStatus SampleOffset(const UmiCreativeElement * e, uint32_t timeMs, bool vertical, double * out)
{
    if (e -> keyCount == 0U){
        * out = 0.0;
        return UMI_STATUS_OK;
    }
    UmiMediaAnimationTrack * track = NULL;
    UmiStatus status = umi_media_animation_track_create(UMI_CREATIVE_MAX_KEYS, & track);
    for (size_t i = 0U;status == UMI_STATUS_OK && i < e -> keyCount; ++ i){
        UmiMediaAnimationKeyframe key = {
            (double)e -> keys[i].timeMs / 1000.0,
            (double)(vertical ? e -> keys[i].offsetY : e -> keys[i].offsetX), e -> keys[i].interpolation
        };
        status = umi_media_animation_track_add(track, & key);
    }
    if (status == UMI_STATUS_OK)status = umi_media_animation_track_sample(track, (double)timeMs / 1000.0, out);
    umi_media_animation_track_destroy(track);
    return status;
}

UmiStatus UmiCreativeSceneSample(const UmiCreativeProject * p, uint32_t timeMs, UmiCreativeFrame * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiCreativeProjectValidate(p)!= UMI_STATUS_OK || timeMs > p -> settings.durationMs)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeFrame frame = {0};
    frame.sourceRevision = p -> revision;
    frame.timeMs = timeMs;
    frame.elementCount = p -> elementCount;
    for (size_t i = 0U;i < p -> elementCount; ++ i){
        const UmiCreativeElement * e = & p -> elements[i];
        UmiCreativeFrameElement * f = & frame.elements[i];
        double dx, dy;
        UmiStatus status = SampleOffset(e, timeMs, false, & dx);
        if (status == UMI_STATUS_OK)status = SampleOffset(e, timeMs, true, & dy);
        if (status!= UMI_STATUS_OK)return status;
        memcpy(f -> id, e -> id, sizeof(f -> id));
        memcpy(f -> label, e -> label, sizeof(f -> label));
        f -> kind = e -> kind;
        f -> x = (double)e -> x + dx;
        f -> y = (double)e -> y + dy;
        f -> width = e -> width;
        f -> height = e -> height;
        f -> colourRgb = e -> colourRgb;
        f -> visible = e -> visible;
    }
    * out = frame;
    return UMI_STATUS_OK;
}

static bool FrameValid(const UmiCreativeFrame * f)
{
    if (f == NULL || f -> elementCount > UMI_CREATIVE_MAX_ELEMENTS)return false;
    for (size_t i = 0U;i < f -> elementCount; ++ i){
        const UmiCreativeFrameElement * e = & f -> elements[i];
        if (!UmiCreativeIdValid(e -> id) || !UmiCreativeTextValid(e -> label, sizeof(e -> label), false) ||
        !isfinite(e -> x) || !isfinite(e -> y) || !isfinite(e -> width) || !isfinite(e -> height) ||
        e -> x < 0.0 || e -> y < 0.0 || e -> width <= 0.0 || e -> height <= 0.0 ||
        e -> x + e -> width > UMI_CREATIVE_MAX_EXTENT || e -> y + e -> height > UMI_CREATIVE_MAX_EXTENT ||
        e -> kind < UMI_CREATIVE_RECTANGLE || e -> kind > UMI_CREATIVE_OBSTACLE)return false;
    }
    return true;
}

UmiStatus UmiCreativeSceneHitTest(const UmiCreativeFrame * f, double x, double y, size_t * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * out = SIZE_MAX;
    if (!FrameValid(f) || !isfinite(x) || !isfinite(y))return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t n = f -> elementCount;n > 0U; -- n){
        const UmiCreativeFrameElement * e = & f -> elements[n-1U];
        if (e -> visible && x >= e -> x && x < e -> x + e -> width && y >= e -> y && y < e -> y + e -> height){
            * out = n-1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiCreativeSceneOverlaps(const UmiCreativeFrame * f, UmiCreativeOverlap * pairs, size_t capacity, size_t * outCount)
{
    if (outCount == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * outCount = 0U;
    if (!FrameValid(f) || (capacity!= 0U && pairs == NULL))return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = 0U;
    for (size_t i = 0U;i < f -> elementCount; ++ i){
        const UmiCreativeFrameElement * a = & f -> elements[i];
        if (!a -> visible || a -> kind == UMI_CREATIVE_TEXT)continue;
        for (size_t j = i + 1U;j < f -> elementCount; ++ j){
            const UmiCreativeFrameElement * b = & f -> elements[j];
            if (!b -> visible || b -> kind == UMI_CREATIVE_TEXT)continue;
            if (a -> x < b -> x + b -> width && b -> x < a -> x + a -> width && a -> y < b -> y + b -> height && b -> y < a -> y + a -> height){
                if (count < capacity){
                    pairs[count].first = i;
                    pairs[count].second = j;
                }
                ++ count;
            }
        }
    }
    * outCount = count;
    return count > capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

