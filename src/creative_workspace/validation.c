/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/validation.c
 * PURPOSE: Protect scene bounds, note timing, identity and complete-document invariants.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
static bool SettingsValid(const UmiCreativeSettings * s)
{
    return s!= NULL && UmiCreativeTextValid(s -> title, sizeof(s -> title), false) &&
    (s -> unit == UMI_CREATIVE_PIXELS || s -> unit == UMI_CREATIVE_MILLIMETRES) &&
    s -> width > 0 && s -> width <= UMI_CREATIVE_MAX_EXTENT && s -> height > 0 && s -> height <= UMI_CREATIVE_MAX_EXTENT &&
    s -> durationMs > 0U && s -> durationMs <= UMI_CREATIVE_MAX_DURATION_MS && s -> tempoBpm >= 20U && s -> tempoBpm <= 300U;
}

static bool Fits(const UmiCreativeSettings * s, const UmiCreativeElement * e, int32_t dx, int32_t dy)
{
    int64_t x = (int64_t)e -> x + dx, y = (int64_t)e -> y + dy;
    return x >= 0 && y >= 0 && x + e -> width <= s -> width && y + e -> height <= s -> height;
}

static bool ElementValid(const UmiCreativeSettings * s, const UmiCreativeElement * e)
{
    if (!UmiCreativeIdValid(e -> id) || !UmiCreativeTextValid(e -> label, sizeof(e -> label), false) ||
    e -> kind < UMI_CREATIVE_RECTANGLE || e -> kind > UMI_CREATIVE_OBSTACLE ||
    e -> width <= 0 || e -> height <= 0 || e -> colourRgb > 0xFFFFFFU ||
    e -> keyCount > UMI_CREATIVE_MAX_KEYS || !Fits(s, e, 0, 0))return false;
    for (size_t i = 0U;i < e -> keyCount; ++ i){
        const UmiCreativeKey * k = & e -> keys[i];
        if (k -> timeMs > s -> durationMs || (i == 0U && k -> timeMs!= 0U) ||
        (i > 0U && k -> timeMs <= e -> keys[i-1U].timeMs) ||
        (k -> interpolation!= UMI_MEDIA_ANIMATION_STEP && k -> interpolation!= UMI_MEDIA_ANIMATION_LINEAR) ||
        !Fits(s, e, k -> offsetX, k -> offsetY))return false;
    }
    return true;
}

UmiStatus UmiCreativeProjectValidate(const UmiCreativeProject * p)
{
    if (p == NULL || !UmiCreativeIdValid(p -> id) || !SettingsValid( & p -> settings) ||
    p -> elementCount > UMI_CREATIVE_MAX_ELEMENTS || p -> noteCount > UMI_CREATIVE_MAX_NOTES)
    return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U;i < p -> elementCount; ++ i){
        if (!ElementValid( & p -> settings, & p -> elements[i]))return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U;j < i; ++ j)if (strcmp(p -> elements[i].id, p -> elements[j].id) == 0)return UMI_STATUS_ALREADY_EXISTS;
    }
    for (size_t i = 0U;i < p -> noteCount; ++ i){
        const UmiCreativeNote * n = & p -> notes[i];
        uint64_t end = (uint64_t)n -> startTick + n -> durationTicks;
        if (!UmiCreativeIdValid(n -> id) || n -> pitch > 127U || n -> velocity == 0U || n -> velocity > 127U ||
        n -> durationTicks == 0U || end > UINT32_MAX ||
        end * 60000U > (uint64_t)p -> settings.durationMs * p -> settings.tempoBpm * UMI_CREATIVE_TICKS_PER_BEAT)
        return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U;j < i; ++ j)if (strcmp(n -> id, p -> notes[j].id) == 0)return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}

size_t UmiCreativeFindElementIndex(const UmiCreativeProject * p, const char * id)
{
    for (size_t i = 0U;i < p -> elementCount; ++ i)if (strcmp(p -> elements[i].id, id) == 0)return i;
    return SIZE_MAX;
}

size_t UmiCreativeFindNoteIndex(const UmiCreativeProject * p, const char * id)
{
    for (size_t i = 0U;i < p -> noteCount; ++ i)if (strcmp(p -> notes[i].id, id) == 0)return i;
    return SIZE_MAX;
}

/* Semantic equality deliberately ignores C padding, unused array tails and
 * storage revision. Repeating an unchanged edit does not create a new history. */
bool UmiCreativeProjectEqual(const UmiCreativeProject * a, const UmiCreativeProject * b)
{
    if (strcmp(a -> id, b -> id)!= 0 || strcmp(a -> settings.title, b -> settings.title)!= 0 ||
    a -> settings.unit!= b -> settings.unit || a -> settings.width!= b -> settings.width ||
    a -> settings.height!= b -> settings.height || a -> settings.durationMs!= b -> settings.durationMs ||
    a -> settings.tempoBpm!= b -> settings.tempoBpm || a -> elementCount!= b -> elementCount || a -> noteCount!= b -> noteCount)return false;
    for (size_t i = 0U;i < a -> elementCount; ++ i){
        const UmiCreativeElement * x = & a -> elements[i], * y = & b -> elements[i];
        if (strcmp(x -> id, y -> id)!= 0 || strcmp(x -> label, y -> label)!= 0 || x -> kind!= y -> kind ||
        x -> x!= y -> x || x -> y!= y -> y || x -> width!= y -> width || x -> height!= y -> height ||
        x -> colourRgb!= y -> colourRgb || x -> visible!= y -> visible || x -> keyCount!= y -> keyCount)return false;
        for (size_t j = 0U;j < x -> keyCount; ++ j)if (x -> keys[j].timeMs!= y -> keys[j].timeMs ||
        x -> keys[j].offsetX!= y -> keys[j].offsetX || x -> keys[j].offsetY!= y -> keys[j].offsetY ||
        x -> keys[j].interpolation!= y -> keys[j].interpolation)return false;
    }
    for (size_t i = 0U;i < a -> noteCount; ++ i){
        const UmiCreativeNote * x = & a -> notes[i], * y = & b -> notes[i];
        if (strcmp(x -> id, y -> id)!= 0 || x -> startTick!= y -> startTick || x -> durationTicks!= y -> durationTicks ||
        x -> pitch!= y -> pitch || x -> velocity!= y -> velocity)return false;
    }
    return true;
}

