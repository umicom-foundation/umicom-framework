/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/cross_app_surface.c
 *
 * PURPOSE:
 *   Identify an application-owned surface without making the application own the docking implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/cross_app_surface.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic cross app surface from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_cross_app_surface_init(UmiUiMosaicCrossAppSurface *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->row_span = 1U;
    value->column_span = 1U;
    value->active = true;
}

/*
 * Provide the ui mosaic cross app surface place operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_mosaic_cross_app_surface_place(UmiUiMosaicCrossAppSurface *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->panel_id, sizeof(value->panel_id), panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->application = application;
    value->row = row;
    value->column = column;
    return UMI_STATUS_OK;
}

/*
 * Check that ui mosaic cross app surface satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_cross_app_surface_validate(const UmiUiMosaicCrossAppSurface *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->panel_id, '\0', sizeof(value->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    uint32_t end_row; uint32_t end_column;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->active || !umi_ui_mosaic_id_is_valid(value->id) || !umi_ui_mosaic_id_is_valid(value->panel_id)) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->row_span == 0U || value->column_span == 0U) return UMI_STATUS_INVALID_STATE;
    end_row = (uint32_t)value->row + (uint32_t)value->row_span;
    end_column = (uint32_t)value->column + (uint32_t)value->column_span;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (end_row > UMI_UI_MOSAIC_MAX_CELLS || end_column > UMI_UI_MOSAIC_MAX_CELLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic cross app surface area operation used by this module and its
 * client applications.
 */
uint32_t umi_ui_mosaic_cross_app_surface_area(const UmiUiMosaicCrossAppSurface *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
    return (uint32_t)value->row_span * (uint32_t)value->column_span;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicCrossAppSurfaceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcc8f5cfcfa2cc2f0);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicCrossAppSurface *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicCrossAppSurface *)0)->panel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicCrossAppSurfaceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicCrossAppSurface *)0)->id) - 1U +
        8U + sizeof(((UmiUiMosaicCrossAppSurface *)0)->panel_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicCrossAppSurfaceArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicCrossAppSurface *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->application);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_span);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column_span);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiUiMosaicCrossAppSurfaceArchiveRead(UmiArchiveReader *reader, UmiUiMosaicCrossAppSurface *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    value->application = (UmiUiMosaicApplication)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->row = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->column = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->row_span = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->column_span = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicCrossAppSurfaceArchiveValidate(const UmiUiMosaicCrossAppSurface *value)
{
    return umi_ui_mosaic_cross_app_surface_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_cross_app_surface_archive_encode, umi_ui_mosaic_cross_app_surface_archive_decode,
    UmiUiMosaicCrossAppSurface, UmiUiMosaicCrossAppSurfaceArchiveSchema, UmiUiMosaicCrossAppSurfaceArchiveBound, UmiUiMosaicCrossAppSurfaceArchiveWrite, UmiUiMosaicCrossAppSurfaceArchiveRead, UmiUiMosaicCrossAppSurfaceArchiveValidate)
