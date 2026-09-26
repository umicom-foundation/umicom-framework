/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/project.c
 * PURPOSE: Apply validated edits to caller-owned draft values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiCreativeProjectInit(UmiCreativeProject * out, const char * id, const UmiCreativeSettings * settings)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Take copies first so settings may refer to the output's former value. */
    UmiCreativeSettings copied = {0};
    char copiedId[UMI_CREATIVE_ID_CAPACITY] = {0};
    if (settings!= NULL)copied = * settings;
    UmiStatus status = UmiCreativeCopyText(copiedId, sizeof(copiedId), id);
    memset(out, 0, sizeof( * out));
    if (status!= UMI_STATUS_OK || settings == NULL || !UmiCreativeIdValid(copiedId))return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(out -> id, copiedId, sizeof(copiedId));
    out -> settings = copied;
    status = UmiCreativeProjectValidate(out);
    if (status!= UMI_STATUS_OK)memset(out, 0, sizeof( * out));
    return status;
}

UmiStatus UmiCreativeElementInit(UmiCreativeElement * out, const char * id, const char * label,
UmiCreativeElementKind kind, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t colourRgb)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeElement value = {0};
    UmiStatus status = UmiCreativeCopyText(value.id, sizeof(value.id), id);
    if (status == UMI_STATUS_OK)status = UmiCreativeCopyText(value.label, sizeof(value.label), label);
    memset(out, 0, sizeof( * out));
    if (status!= UMI_STATUS_OK || !UmiCreativeIdValid(value.id) || !UmiCreativeTextValid(value.label, sizeof(value.label), false) ||
    kind < UMI_CREATIVE_RECTANGLE || kind > UMI_CREATIVE_OBSTACLE || x < 0 || y < 0 || width <= 0 || height <= 0 ||
    (int64_t)x + width > UMI_CREATIVE_MAX_EXTENT || (int64_t)y + height > UMI_CREATIVE_MAX_EXTENT || colourRgb > 0xFFFFFFU)
    return UMI_STATUS_INVALID_ARGUMENT;
    value.kind = kind;
    value.x = x;
    value.y = y;
    value.width = width;
    value.height = height;
    value.colourRgb = colourRgb;
    value.visible = true;
    * out = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeProjectConfigure(UmiCreativeProject * p, const UmiCreativeSettings * settings)
{
    if (settings == NULL || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    if (p -> elementCount!= 0U && p -> settings.unit!= settings -> unit)return UMI_STATUS_INVALID_STATE;
    UmiCreativeSettings before = p -> settings;
    p -> settings = * settings;
    UmiStatus status = UmiCreativeProjectValidate(p);
    if (status!= UMI_STATUS_OK)p -> settings = before;
    return status;
}

UmiStatus UmiCreativeProjectPutElement(UmiCreativeProject * p, const UmiCreativeElement * element, bool replace)
{
    if (element == NULL || !UmiCreativeIdValid(element -> id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, element -> id);
    if (index!= SIZE_MAX && !replace)return UMI_STATUS_ALREADY_EXISTS;
    if (index == SIZE_MAX && replace)return UMI_STATUS_NOT_FOUND;
    bool adding = index == SIZE_MAX;
    if (adding && p -> elementCount >= UMI_CREATIVE_MAX_ELEMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCreativeElement copied = * element;
    if (adding)index = p -> elementCount ++ ;
    UmiCreativeElement before = p -> elements[index];
    p -> elements[index] = copied;
    UmiStatus status = UmiCreativeProjectValidate(p);
    if (status!= UMI_STATUS_OK){
        p -> elements[index] = before;
        if (adding) -- p -> elementCount;
    }
    return status;
}

UmiStatus UmiCreativeProjectFindElement(const UmiCreativeProject * p, const char * id, UmiCreativeElement * out)
{
    if (out == NULL || !UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    * out = p -> elements[index];
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeProjectRemoveElement(UmiCreativeProject * p, const char * id)
{
    if (!UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    memmove( & p -> elements[index], & p -> elements[index + 1U], (p -> elementCount - index-1U) * sizeof(p -> elements[0]));
    -- p -> elementCount;
    memset( & p -> elements[p -> elementCount], 0, sizeof(p -> elements[0]));
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeProjectMoveLayer(UmiCreativeProject * p, const char * id, size_t newIndex)
{
    if (!UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    if (newIndex >= p -> elementCount)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeElement moved = p -> elements[index];
    if (newIndex < index)memmove( & p -> elements[newIndex + 1U], & p -> elements[newIndex], (index - newIndex) * sizeof(moved));
    else if (newIndex > index)memmove( & p -> elements[index], & p -> elements[index + 1U], (newIndex - index) * sizeof(moved));
    p -> elements[newIndex] = moved;
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeProjectPutKey(UmiCreativeProject * p, const char * id, const UmiCreativeKey * key)
{
    if (key == NULL || !UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    UmiCreativeElement before = p -> elements[index];
    UmiCreativeElement * e = & p -> elements[index];
    UmiCreativeKey copied = * key;
    size_t at = 0U;
    while (at < e -> keyCount && e -> keys[at].timeMs < copied.timeMs) ++ at;
    if (at == e -> keyCount || e -> keys[at].timeMs!= copied.timeMs){
        if (e -> keyCount >= UMI_CREATIVE_MAX_KEYS)return UMI_STATUS_CAPACITY_EXCEEDED;
        memmove( & e -> keys[at + 1U], & e -> keys[at], (e -> keyCount - at) * sizeof(e -> keys[0]));
        ++ e -> keyCount;
    }
    e -> keys[at] = copied;
    UmiStatus status = UmiCreativeProjectValidate(p);
    if (status!= UMI_STATUS_OK) * e = before;
    return status;
}

UmiStatus UmiCreativeProjectRemoveKey(UmiCreativeProject * p, const char * id, uint32_t timeMs)
{
    if (!UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindElementIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    UmiCreativeElement before = p -> elements[index];
    UmiCreativeElement * e = & p -> elements[index];
    size_t at = 0U;
    while (at < e -> keyCount && e -> keys[at].timeMs!= timeMs) ++ at;
    if (at == e -> keyCount)return UMI_STATUS_NOT_FOUND;
    memmove( & e -> keys[at], & e -> keys[at + 1U], (e -> keyCount - at-1U) * sizeof(e -> keys[0]));
    -- e -> keyCount;
    memset( & e -> keys[e -> keyCount], 0, sizeof(e -> keys[0]));
    UmiStatus status = UmiCreativeProjectValidate(p);
    if (status!= UMI_STATUS_OK){
        * e = before;
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiCreativeProjectPutNote(UmiCreativeProject * p, const UmiCreativeNote * note, bool replace)
{
    if (note == NULL || !UmiCreativeIdValid(note -> id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindNoteIndex(p, note -> id);
    if (index!= SIZE_MAX && !replace)return UMI_STATUS_ALREADY_EXISTS;
    if (index == SIZE_MAX && replace)return UMI_STATUS_NOT_FOUND;
    bool adding = index == SIZE_MAX;
    if (adding && p -> noteCount >= UMI_CREATIVE_MAX_NOTES)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCreativeNote copied = * note;
    if (adding)index = p -> noteCount ++ ;
    UmiCreativeNote before = p -> notes[index];
    p -> notes[index] = copied;
    UmiStatus status = UmiCreativeProjectValidate(p);
    if (status!= UMI_STATUS_OK){
        p -> notes[index] = before;
        if (adding) -- p -> noteCount;
    }
    return status;
}

UmiStatus UmiCreativeProjectRemoveNote(UmiCreativeProject * p, const char * id)
{
    if (!UmiCreativeIdValid(id) || UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiCreativeFindNoteIndex(p, id);
    if (index == SIZE_MAX)return UMI_STATUS_NOT_FOUND;
    memmove( & p -> notes[index], & p -> notes[index + 1U], (p -> noteCount - index-1U) * sizeof(p -> notes[0]));
    -- p -> noteCount;
    memset( & p -> notes[p -> noteCount], 0, sizeof(p -> notes[0]));
    return UMI_STATUS_OK;
}

