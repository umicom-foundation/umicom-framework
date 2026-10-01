/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/checkpoint_drawing_visibility.inc
 * PURPOSE: Round-trip visibility with explicit record versions and strict legacy decoding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included by checkpoint_codec.c to reuse exact integer/double encoding,
 * duplicate-key rejection and the existing transaction digest. The outer
 * checkpoint container is unchanged; each new drawing record declares v2. */
UmiStatus UmiChartCheckpointEncodeDrawing(UmiWorkbenchLayoutDataFieldSet *fields,
    const UmiChartDrawingSnapshot *drawing, char output[UMI_CHART_CHECKPOINT_VALUE])
{
    if (fields == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiChartDrawingValidateGeometry(drawing);
    if (status != UMI_STATUS_OK) return status;
    umi_workbench_layout_data_field_set_init(fields);
    STEP(Number(fields, "drawing_schema", 2U));
    STEP(Number(fields, "visibility_flags", drawing->visibility_flags));
    STEP(Text(fields, "id", drawing->id)); STEP(Text(fields, "pane", drawing->pane_id));
    STEP(Text(fields, "tool", drawing->tool)); STEP(Text(fields, "style", drawing->style));
    STEP(Number(fields, "time1", (uint64_t)drawing->time1)); STEP(Number(fields, "time2", (uint64_t)drawing->time2));
    STEP(Real(fields, "value1", drawing->value1)); STEP(Real(fields, "value2", drawing->value2));
    STEP(Number(fields, "selected", (uint64_t)drawing->selected)); STEP(Number(fields, "locked", (uint64_t)drawing->locked));
    STEP(Number(fields, "revision", drawing->revision));
    STEP(umi_workbench_layout_data_value_encode(fields, output, UMI_CHART_CHECKPOINT_VALUE, NULL));
    return status;
}

/* Legacy records lack visibility fields and load visible. A partial v2 record
 * is never treated as v1: required keys and exact field counts must match. */
UmiStatus UmiChartCheckpointDecodeDrawing(UmiWorkbenchLayoutDataFieldSet *fields,
    const char *text, UmiChartDrawingSnapshot *outDrawing)
{
    if (fields == NULL || text == NULL || outDrawing == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDrawingSnapshot drawing = {0};
    uint64_t time1 = 0U, time2 = 0U, selected = 0U, locked = 0U, schema = 0U;
    UmiStatus status = DecodeFields(text, 13U, fields);
    int versionTwo = status == UMI_STATUS_OK;
    if (!versionTwo) status = DecodeFields(text, 11U, fields);
    if (versionTwo) {
        STEP(ReadNumber(fields, "drawing_schema", &schema));
        STEP(ReadNumber(fields, "visibility_flags", &drawing.visibility_flags));
        if (status == UMI_STATUS_OK && schema != 2U) status = UMI_STATUS_PARSE_ERROR;
    }
    STEP(ReadText(fields, "id", drawing.id, sizeof drawing.id));
    STEP(ReadText(fields, "pane", drawing.pane_id, sizeof drawing.pane_id));
    STEP(ReadText(fields, "tool", drawing.tool, sizeof drawing.tool));
    STEP(ReadText(fields, "style", drawing.style, sizeof drawing.style));
    STEP(ReadNumber(fields, "time1", &time1)); STEP(ReadNumber(fields, "time2", &time2));
    STEP(ReadReal(fields, "value1", &drawing.value1)); STEP(ReadReal(fields, "value2", &drawing.value2));
    STEP(ReadNumber(fields, "selected", &selected)); STEP(ReadNumber(fields, "locked", &locked));
    STEP(ReadNumber(fields, "revision", &drawing.revision));
    if (status != UMI_STATUS_OK) return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
    if (selected > 1U || locked > 1U) return UMI_STATUS_PARSE_ERROR;
    drawing.time1 = Signed(time1); drawing.time2 = Signed(time2);
    drawing.selected = (int)selected; drawing.locked = (int)locked;
    drawing.struct_size = (uint32_t)sizeof drawing; drawing.api_version = 1U;
    if (UmiChartDrawingValidateGeometry(&drawing) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *outDrawing = drawing;
    return UMI_STATUS_OK;
}
