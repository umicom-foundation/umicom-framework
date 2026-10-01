/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing_appearance.c
 * PURPOSE: Keep bounded appearance parsing and guarded edits independent of widget and file formats.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_appearance.h"
#include <stdio.h>
#include <string.h>

/* This namespace lives inside the existing opaque style field. Older and
 * future encodings remain round-trippable through the checkpoint service. */
#define APPEARANCE_PREFIX "umi-drawing:1:"
static int AppearanceHexDigit(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}
static int AppearanceLength(const char *text, size_t capacity, size_t *out)
{
    if (text == NULL) return 0;
    for (size_t i = 0U; i < capacity; ++i) {
        if (text[i] == '\0') { *out = i; return 1; }
    }
    return 0;
}
UmiStatus UmiChartDrawingAppearanceValidate(const UmiChartDrawingAppearance *a)
{
    if (a == NULL || a->red > 255U || a->green > 255U || a->blue > 255U ||
        a->width_tenths < 10U || a->width_tenths > 80U || a->fill_percent > 80U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingAppearanceFromHex(const char *hex, uint32_t widthTenths,
    uint32_t fillPercent, UmiChartDrawingAppearance *outAppearance)
{
    size_t length;
    if (outAppearance == NULL || !AppearanceLength(hex, 8U, &length) ||
        length != 7U || hex[0] != '#') return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t channels[3] = {0};
    for (size_t i = 0U; i < 6U; ++i) {
        int digit = AppearanceHexDigit(hex[i + 1U]);
        if (digit < 0) return UMI_STATUS_INVALID_ARGUMENT;
        channels[i / 2U] = channels[i / 2U] * 16U + (uint32_t)digit;
    }
    UmiChartDrawingAppearance value = {channels[0], channels[1], channels[2], widthTenths, fillPercent};
    UmiStatus status = UmiChartDrawingAppearanceValidate(&value);
    if (status == UMI_STATUS_OK) *outAppearance = value;
    return status;
}
UmiStatus UmiChartDrawingAppearanceEncode(const UmiChartDrawingAppearance *appearance,
    char *outText, size_t capacity)
{
    if (outText == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiChartDrawingAppearanceValidate(appearance);
    if (status != UMI_STATUS_OK) return status;
    char encoded[64];
    int length = snprintf(encoded, sizeof encoded, APPEARANCE_PREFIX "%02X%02X%02X:%02u:%02u",
        (unsigned)appearance->red, (unsigned)appearance->green, (unsigned)appearance->blue,
        (unsigned)appearance->width_tenths, (unsigned)appearance->fill_percent);
    if (length < 0 || (size_t)length >= sizeof encoded) return UMI_STATUS_INVALID_STATE;
    if ((size_t)length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outText, encoded, (size_t)length + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingAppearanceDecode(const char *text, size_t capacity,
    UmiChartDrawingAppearance *outAppearance)
{
    size_t length;
    if (outAppearance == NULL || !AppearanceLength(text, capacity, &length)) return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U) return UMI_STATUS_NOT_FOUND;
    const size_t prefix = sizeof APPEARANCE_PREFIX - 1U;
    if (length < prefix || memcmp(text, APPEARANCE_PREFIX, prefix) != 0) return UMI_STATUS_UNAVAILABLE;
    if (length != prefix + 12U || text[prefix + 6U] != ':' || text[prefix + 9U] != ':')
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 7U; i < 12U; ++i) {
        if (i == 9U) continue;
        if (text[prefix + i] < '0' || text[prefix + i] > '9') return UMI_STATUS_PARSE_ERROR;
    }
    char hex[8] = "#000000";
    memcpy(hex + 1U, text + prefix, 6U);
    uint32_t width = (uint32_t)(text[prefix + 7U] - '0') * 10U + (uint32_t)(text[prefix + 8U] - '0');
    uint32_t fill = (uint32_t)(text[prefix + 10U] - '0') * 10U + (uint32_t)(text[prefix + 11U] - '0');
    UmiChartDrawingAppearance value;
    if (UmiChartDrawingAppearanceFromHex(hex, width, fill, &value) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *outAppearance = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingAppearanceResolve(const UmiChartDrawingSnapshot *drawing,
    const UmiChartPlotStyle *theme, UmiChartDrawingResolvedAppearance *outAppearance)
{
    if (theme == NULL || outAppearance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiChartDrawingToolValidate(drawing);
    if (status != UMI_STATUS_OK) return status;
    UmiChartDrawingKind kind;
    (void)UmiChartDrawingKindParse(drawing->tool, &kind);
    int level = kind == UMI_CHART_DRAWING_SUPPORT || kind == UMI_CHART_DRAWING_RESISTANCE;
    int box = kind == UMI_CHART_DRAWING_RANGE || kind == UMI_CHART_DRAWING_LIQUIDITY_ZONE;
    UmiChartDrawingResolvedAppearance value = {0};
    value.outline = kind == UMI_CHART_DRAWING_SUPPORT ? theme->positive_color :
        kind == UMI_CHART_DRAWING_RESISTANCE ? theme->negative_color : (UmiChartColor){0.98, 0.72, 0.20, 1};
    value.width = level || box ? 1.4 : 2.0;
    if (kind == UMI_CHART_DRAWING_LIQUIDITY_ZONE) {
        value.outline = (UmiChartColor){0.22, 0.65, 0.9, 1};
        value.fill = (UmiChartColor){0.22, 0.65, 0.9, 0.16};
    }
    UmiChartDrawingAppearance appearance;
    if (UmiChartDrawingAppearanceDecode(drawing->style, sizeof drawing->style, &appearance) == UMI_STATUS_OK) {
        value.outline = (UmiChartColor){appearance.red / 255.0, appearance.green / 255.0, appearance.blue / 255.0, 1};
        value.width = appearance.width_tenths / 10.0;
        value.fill = value.outline;
        value.fill.alpha = box ? appearance.fill_percent / 100.0 : 0;
    }
    *outAppearance = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingSetAppearance(UmiChartDrawingRegistry *registry,
    const char *pane, const char *id, uint64_t expectedRevision,
    const UmiChartDrawingAppearance *appearance)
{
    size_t length;
    if (registry == NULL || !AppearanceLength(pane, 128U, &length) || length == 0U ||
        !AppearanceLength(id, 128U, &length) || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    char encoded[256] = {0};
    if (appearance != NULL) {
        UmiStatus status = UmiChartDrawingAppearanceEncode(appearance, encoded, sizeof encoded);
        if (status != UMI_STATUS_OK) return status;
    }
    UmiChartDrawingSnapshot drawing;
    UmiStatus status = umi_chart_drawing_registry_find(registry, id, &drawing);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(drawing.pane_id, pane) != 0) return UMI_STATUS_INVALID_STATE;
    if (drawing.revision != expectedRevision) return UMI_STATUS_BUSY;
    status = UmiChartDrawingToolValidate(&drawing);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(drawing.style, encoded) == 0) return UMI_STATUS_OK;
    memcpy(drawing.style, encoded, sizeof drawing.style);
    return umi_chart_drawing_registry_upsert(registry, &drawing);
}
