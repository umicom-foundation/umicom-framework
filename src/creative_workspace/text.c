/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/text.c
 * PURPOSE: Validate bounded UTF-8 before persistence, drawing or export.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>
/* Validate scalar values rather than merely checking continuation bytes.
 * This also rejects embedded controls and Unicode noncharacters which are
 * unsuitable for single-line labels and XML output. No locale is consulted. */
bool UmiCreativeTextValid(const char * text, size_t capacity, bool allowEmpty)
{
    if (text == NULL || capacity == 0U)return false;
    size_t length = 0U;
    while (length < capacity && text[length]!= '\0') ++ length;
    if (length == capacity || (!allowEmpty && length == 0U))return false;
    bool nonSpace = false;
    for (size_t i = 0U;i < length;){
        unsigned char first = (unsigned char)text[i ++ ];
        uint32_t code;
        size_t extra;
        if (first < 0x80U){
            code = first;
            extra = 0U;
        }
        else if (first >= 0xC2U && first <= 0xDFU){
            code = first & 0x1FU;
            extra = 1U;
        }
        else if (first >= 0xE0U && first <= 0xEFU){
            code = first & 0x0FU;
            extra = 2U;
        }
        else if (first >= 0xF0U && first <= 0xF4U){
            code = first & 0x07U;
            extra = 3U;
        }
        else return false;
        if (extra > length - i)return false;
        for (size_t j = 0U;j < extra; ++ j){
            unsigned char next = (unsigned char)text[i ++ ];
            if ((next & 0xC0U)!= 0x80U)return false;
            code = (code << 6U) | (next & 0x3FU);
        }
        if ((extra == 1U && code < 0x80U) || (extra == 2U && code < 0x800U) ||
        (extra == 3U && code < 0x10000U) || code > 0x10FFFFU ||
        (code >= 0xD800U && code <= 0xDFFFU) || code < 0x20U ||
        (code >= 0x7FU && code <= 0x9FU) ||
        (code >= 0xFDD0U && code <= 0xFDEFU) || (code & 0xFFFFU) >= 0xFFFEU)
        return false;
        if (code!= 0x20U)nonSpace = true;
    }
    return allowEmpty || nonSpace;
}

static bool Alphanumeric(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

bool UmiCreativeIdValid(const char * id)
{
    if (id == NULL || !Alphanumeric((unsigned char)id[0]))return false;
    for (size_t i = 0U;i < UMI_CREATIVE_ID_CAPACITY; ++ i){
        unsigned char c = (unsigned char)id[i];
        if (c == 0U)return true;
        if (!Alphanumeric(c) && c!= '.' && c!= '_' && c!= '-')return false;
    }
    return false;
}

UmiStatus UmiCreativeCopyText(char * out, size_t capacity, const char * text)
{
    if (out == NULL || text == NULL || capacity == 0U)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < capacity && text[length]!= '\0') ++ length;
    if (length == capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    memmove(out, text, length + 1U);
    return UMI_STATUS_OK;
}

const char * UmiCreativeUnitText(UmiCreativeUnit unit)
{
    return unit == UMI_CREATIVE_PIXELS ? "pixels" : unit == UMI_CREATIVE_MILLIMETRES ? "millimetres" : "unknown";
}

const char * UmiCreativeElementKindText(UmiCreativeElementKind kind)
{
    switch (kind){
        case UMI_CREATIVE_RECTANGLE:return "Rectangle";
        case UMI_CREATIVE_TEXT:return "Text";
        case UMI_CREATIVE_PANEL:return "Panel";
        case UMI_CREATIVE_CABINET:return "Cabinet";
        case UMI_CREATIVE_ACTOR:return "Actor";
        case UMI_CREATIVE_OBSTACLE:return "Obstacle";
        default : return "Unknown";
    }
}

