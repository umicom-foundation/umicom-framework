/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/checkpoint_codec.c
 * PURPOSE: Preserve exact drawing coordinates and identities in bounded versioned records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "checkpoint_private.h"
#include "umicom/chart/drawing_validation.h"
#include <float.h>
#include <math.h>
#include <string.h>

int UmiChartCheckpointIdentityValid(const char *identity)
{
    if (identity == NULL || identity[0] == '\0') return 0;
    for (size_t i = 1U; i < 128U; ++i) if (identity[i] == '\0') return 1;
    return 0;
}
static UmiStatus Text(UmiWorkbenchLayoutDataFieldSet *fields, const char *key, const char *value)
{ return umi_workbench_layout_data_field_set_put(fields, key, value); }
static UmiStatus Number(UmiWorkbenchLayoutDataFieldSet *fields, const char *key, uint64_t value)
{ return umi_workbench_layout_data_field_set_put_u64(fields, key, value); }
static UmiStatus ReadNumber(const UmiWorkbenchLayoutDataFieldSet *fields, const char *key, uint64_t *value)
{
    const char *text = umi_workbench_layout_data_field_set_get(fields, key);
    if (text == NULL || text[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    uint64_t candidate = 0U;
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < '0' || *p > '9') return UMI_STATUS_PARSE_ERROR;
        unsigned int digit = (unsigned int)(*p - '0');
        if (candidate > (UINT64_MAX - digit) / 10U) return UMI_STATUS_PARSE_ERROR;
        candidate = candidate * 10U + digit;
    }
    *value = candidate; return UMI_STATUS_OK;
}
static UmiStatus ReadText(const UmiWorkbenchLayoutDataFieldSet *fields, const char *key, char *value, size_t capacity)
{
    const char *text = umi_workbench_layout_data_field_set_get(fields, key);
    if (text == NULL || strlen(text) >= capacity) return UMI_STATUS_PARSE_ERROR;
    memcpy(value, text, strlen(text) + 1U); return UMI_STATUS_OK;
}
/* Defined modulo conversion avoids signed overflow, including INT64_MIN. */
static int64_t Signed(uint64_t bits)
{ return bits <= INT64_MAX ? (int64_t)bits : -(int64_t)(UINT64_MAX - bits) - 1; }
static int Binary64(void)
{ return sizeof(double) == sizeof(uint64_t) && FLT_RADIX == 2 && DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024; }
static UmiStatus Real(UmiWorkbenchLayoutDataFieldSet *fields, const char *key, double value)
{
    if (!Binary64()) return UMI_STATUS_UNAVAILABLE;
    if (!isfinite(value)) return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t bits = 0U; memcpy(&bits, &value, sizeof(bits)); return Number(fields, key, bits);
}
static UmiStatus ReadReal(const UmiWorkbenchLayoutDataFieldSet *fields, const char *key, double *value)
{
    if (!Binary64()) return UMI_STATUS_UNAVAILABLE;
    uint64_t bits; UmiStatus status = ReadNumber(fields, key, &bits);
    if (status != UMI_STATUS_OK) return status;
    double candidate; memcpy(&candidate, &bits, sizeof(candidate));
    if (!isfinite(candidate)) return UMI_STATUS_PARSE_ERROR;
    *value = candidate; return UMI_STATUS_OK;
}
/* A checkpoint has exactly one physical line per field. The general-purpose
 * field set supports updates; persisted documents must reject duplicate keys,
 * a missing final newline and encoded NULs rather than silently normalise them. */
static UmiStatus DecodeFields(const char *text, size_t expected, UmiWorkbenchLayoutDataFieldSet *fields)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t lines = 0U, length = strlen(text);
    if (length == 0U || text[length - 1U] != '\n' || strstr(text, "%00") != NULL)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < length; ++i) if (text[i] == '\n') ++lines;
    if (lines != expected) return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = umi_workbench_layout_data_value_decode(text, fields);
    if (status == UMI_STATUS_OK && fields->count != expected) return UMI_STATUS_PARSE_ERROR;
    return status;
}
#define STEP(call) do { if (status == UMI_STATUS_OK) status = (call); } while (0)
/* Version-two view metadata records the fixed UTC timeframe. The decoder accepts the original nine-field view as source mode; strict field counts and version validation prevent ambiguous restoration. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiChartCheckpointEncodeMetadata(UmiWorkbenchLayoutDataFieldSet *fields,
    const ChartCheckpointMetadata *metadata, char output[UMI_CHART_CHECKPOINT_VALUE])
{
    umi_workbench_layout_data_field_set_init(fields);
    UmiStatus status = Text(fields, "scope", metadata->scope);
    STEP(Text(fields, "pane", metadata->summary.pane_id));
    STEP(Number(fields, "revision", metadata->storageRevision));
    STEP(Number(fields, "saved_at_ms", metadata->savedAtMs));
    STEP(Number(fields, "source_revision", metadata->summary.source_revision));
    STEP(Number(fields, "count", metadata->summary.drawing_count));
    STEP(Number(fields, "visible_bars", metadata->summary.navigation.visible_bars));
    STEP(Number(fields, "anchor", (uint64_t)metadata->summary.navigation.anchor_ms));
    STEP(Number(fields, "pinned", (uint64_t)metadata->summary.navigation.pinned));
    STEP(umi_workbench_layout_data_value_encode(fields, output, UMI_CHART_CHECKPOINT_VALUE, NULL));
    return status;
}
UmiStatus UmiChartCheckpointDecodeMetadata(UmiWorkbenchLayoutDataFieldSet *fields,
    const char *text, ChartCheckpointMetadata *outMetadata)
{
    ChartCheckpointMetadata metadata = {0}; uint64_t count = 0, bars = 0, anchor = 0, pinned = 0;
    UmiStatus status = DecodeFields(text, 9U, fields);
    STEP(ReadText(fields, "scope", metadata.scope, sizeof(metadata.scope)));
    STEP(ReadText(fields, "pane", metadata.summary.pane_id, sizeof(metadata.summary.pane_id)));
    STEP(ReadNumber(fields, "revision", &metadata.storageRevision));
    STEP(ReadNumber(fields, "saved_at_ms", &metadata.savedAtMs));
    STEP(ReadNumber(fields, "source_revision", &metadata.summary.source_revision));
    STEP(ReadNumber(fields, "count", &count));
    STEP(ReadNumber(fields, "visible_bars", &bars));
    STEP(ReadNumber(fields, "anchor", &anchor));
    STEP(ReadNumber(fields, "pinned", &pinned));
    if (status != UMI_STATUS_OK) return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
    if (!UmiChartCheckpointIdentityValid(metadata.scope) || !UmiChartCheckpointIdentityValid(metadata.summary.pane_id) ||
        metadata.storageRevision == 0U || count > UMI_CHART_DRAWING_CAPACITY || bars > UMI_CHART_MAX_POINTS || pinned > 1U)
        return UMI_STATUS_PARSE_ERROR;
    metadata.summary.drawing_count = (size_t)count;
    metadata.summary.navigation.visible_bars = (size_t)bars;
    metadata.summary.navigation.anchor_ms = Signed(anchor);
    metadata.summary.navigation.pinned = (int)pinned;
    *outMetadata = metadata; return UMI_STATUS_OK;
}
#endif
UmiStatus UmiChartCheckpointEncodeMetadata(UmiWorkbenchLayoutDataFieldSet *fields,
    const ChartCheckpointMetadata *metadata, char output[UMI_CHART_CHECKPOINT_VALUE])
{
    if (fields == NULL || metadata == NULL || output == NULL ||
        !UmiChartTimeframeValid(metadata->summary.navigation.interval_ms)) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_layout_data_field_set_init(fields);
    UmiStatus status = Text(fields, "scope", metadata->scope);
    STEP(Text(fields, "pane", metadata->summary.pane_id));
    STEP(Number(fields, "revision", metadata->storageRevision));
    STEP(Number(fields, "saved_at_ms", metadata->savedAtMs));
    STEP(Number(fields, "source_revision", metadata->summary.source_revision));
    STEP(Number(fields, "count", metadata->summary.drawing_count));
    STEP(Number(fields, "visible_bars", metadata->summary.navigation.visible_bars));
    STEP(Number(fields, "anchor", (uint64_t)metadata->summary.navigation.anchor_ms));
    STEP(Number(fields, "pinned", (uint64_t)metadata->summary.navigation.pinned));
    STEP(Number(fields, "view_version", 2U));
    STEP(Number(fields, "interval_ms", metadata->summary.navigation.interval_ms));
    STEP(umi_workbench_layout_data_value_encode(fields, output, UMI_CHART_CHECKPOINT_VALUE, NULL));
    return status;
}
UmiStatus UmiChartCheckpointDecodeMetadata(UmiWorkbenchLayoutDataFieldSet *fields,
    const char *text, ChartCheckpointMetadata *outMetadata)
{
    ChartCheckpointMetadata metadata = {0}; uint64_t count = 0, bars = 0, anchor = 0, pinned = 0;
    if (fields == NULL || text == NULL || outMetadata == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t lines = 0U; for (const char *p = text; *p != '\0'; ++p) if (*p == '\n') ++lines;
    if (lines != 9U && lines != 11U) return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = DecodeFields(text, lines, fields);
    uint64_t version = 1U, interval = 0U;
    if (lines == 11U) {
        STEP(ReadNumber(fields, "view_version", &version));
        STEP(ReadNumber(fields, "interval_ms", &interval));
        if (status == UMI_STATUS_OK && (version != 2U || interval > UINT32_MAX ||
            !UmiChartTimeframeValid((uint32_t)interval))) status = UMI_STATUS_PARSE_ERROR;
    }
    STEP(ReadText(fields, "scope", metadata.scope, sizeof(metadata.scope)));
    STEP(ReadText(fields, "pane", metadata.summary.pane_id, sizeof(metadata.summary.pane_id)));
    STEP(ReadNumber(fields, "revision", &metadata.storageRevision));
    STEP(ReadNumber(fields, "saved_at_ms", &metadata.savedAtMs));
    STEP(ReadNumber(fields, "source_revision", &metadata.summary.source_revision));
    STEP(ReadNumber(fields, "count", &count));
    STEP(ReadNumber(fields, "visible_bars", &bars));
    STEP(ReadNumber(fields, "anchor", &anchor));
    STEP(ReadNumber(fields, "pinned", &pinned));
    if (status != UMI_STATUS_OK) return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
    if (!UmiChartCheckpointIdentityValid(metadata.scope) || !UmiChartCheckpointIdentityValid(metadata.summary.pane_id) ||
        metadata.storageRevision == 0U || count > UMI_CHART_DRAWING_CAPACITY || bars > UMI_CHART_MAX_POINTS || pinned > 1U)
        return UMI_STATUS_PARSE_ERROR;
    metadata.summary.drawing_count = (size_t)count;
    metadata.summary.navigation.visible_bars = (size_t)bars;
    metadata.summary.navigation.anchor_ms = Signed(anchor);
    metadata.summary.navigation.pinned = (int)pinned;
    metadata.summary.navigation.interval_ms = (uint32_t)interval;
    *outMetadata = metadata; return UMI_STATUS_OK;
}
/* Drawing records now explicitly version their visibility metadata. The decoder accepts the original eleven-field format with visible defaults, while the writer stores complete version-two records. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiChartCheckpointEncodeDrawing(UmiWorkbenchLayoutDataFieldSet *fields,
    const UmiChartDrawingSnapshot *drawing, char output[UMI_CHART_CHECKPOINT_VALUE])
{
    UmiStatus status = UmiChartDrawingValidateGeometry(drawing);
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_layout_data_field_set_init(fields);
    STEP(Text(fields, "id", drawing->id)); STEP(Text(fields, "pane", drawing->pane_id));
    STEP(Text(fields, "tool", drawing->tool)); STEP(Text(fields, "style", drawing->style));
    STEP(Number(fields, "time1", (uint64_t)drawing->time1)); STEP(Number(fields, "time2", (uint64_t)drawing->time2));
    STEP(Real(fields, "value1", drawing->value1)); STEP(Real(fields, "value2", drawing->value2));
    STEP(Number(fields, "selected", (uint64_t)drawing->selected)); STEP(Number(fields, "locked", (uint64_t)drawing->locked));
    STEP(Number(fields, "revision", drawing->revision));
    STEP(umi_workbench_layout_data_value_encode(fields, output, UMI_CHART_CHECKPOINT_VALUE, NULL));
    return status;
}
UmiStatus UmiChartCheckpointDecodeDrawing(UmiWorkbenchLayoutDataFieldSet *fields,
    const char *text, UmiChartDrawingSnapshot *outDrawing)
{
    UmiChartDrawingSnapshot drawing = {0}; uint64_t time1 = 0, time2 = 0, selected = 0, locked = 0;
    UmiStatus status = DecodeFields(text, 11U, fields);
    STEP(ReadText(fields, "id", drawing.id, sizeof(drawing.id)));
    STEP(ReadText(fields, "pane", drawing.pane_id, sizeof(drawing.pane_id)));
    STEP(ReadText(fields, "tool", drawing.tool, sizeof(drawing.tool)));
    STEP(ReadText(fields, "style", drawing.style, sizeof(drawing.style)));
    STEP(ReadNumber(fields, "time1", &time1)); STEP(ReadNumber(fields, "time2", &time2));
    STEP(ReadReal(fields, "value1", &drawing.value1)); STEP(ReadReal(fields, "value2", &drawing.value2));
    STEP(ReadNumber(fields, "selected", &selected)); STEP(ReadNumber(fields, "locked", &locked));
    STEP(ReadNumber(fields, "revision", &drawing.revision));
    if (status != UMI_STATUS_OK) return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
    if (selected > 1U || locked > 1U) return UMI_STATUS_PARSE_ERROR;
    drawing.time1 = Signed(time1); drawing.time2 = Signed(time2);
    drawing.selected = (int)selected; drawing.locked = (int)locked;
    drawing.struct_size = (uint32_t)sizeof(drawing); drawing.api_version = 1U;
    if (UmiChartDrawingValidateGeometry(&drawing) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *outDrawing = drawing; return UMI_STATUS_OK;
}
#endif
#include "checkpoint_drawing_visibility.inc"
#undef STEP
