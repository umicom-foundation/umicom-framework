/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/dock_target_model.c
 *
 * PURPOSE:
 *   Define toolkit-neutral dock target model contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/dock_target_model.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic dock target model from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_dock_target_model_init(UmiUiMosaicDockTargetModel *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->zone = UMI_UI_MOSAIC_DOCK_CENTRE;
    value->allowed = true;
}

/*
 * Copy ui mosaic dock target model into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_dock_target_model_set(UmiUiMosaicDockTargetModel *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->source_id, sizeof(value->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->target_id, sizeof(value->target_id), target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->zone = zone;
    return UMI_STATUS_OK;
}

/*
 * Check that ui mosaic dock target model satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_dock_target_model_validate(const UmiUiMosaicDockTargetModel *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->source_id, '\0', sizeof(value->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->target_id, '\0', sizeof(value->target_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->allowed || !umi_ui_mosaic_id_is_valid(value->source_id) || !umi_ui_mosaic_id_is_valid(value->target_id)) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(value->source_id, value->target_id) == 0) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->zone < UMI_UI_MOSAIC_DOCK_LEFT || value->zone > UMI_UI_MOSAIC_DOCK_FLOAT) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic dock target model is centre operation used by this module and its
 * client applications.
 */
int umi_ui_mosaic_dock_target_model_is_centre(const UmiUiMosaicDockTargetModel *value) {
    return value != NULL && value->zone == UMI_UI_MOSAIC_DOCK_CENTRE;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicDockTargetModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x677ccea643d69d70);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicDockTargetModel *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicDockTargetModel *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicDockTargetModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicDockTargetModel *)0)->source_id) - 1U +
        8U + sizeof(((UmiUiMosaicDockTargetModel *)0)->target_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicDockTargetModelArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicDockTargetModel *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->zone);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allowed);
}
static void UmiUiMosaicDockTargetModelArchiveRead(UmiArchiveReader *reader, UmiUiMosaicDockTargetModel *value)
{
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->zone = (UmiUiMosaicDockZone)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicDockTargetModelArchiveValidate(const UmiUiMosaicDockTargetModel *value)
{
    return umi_ui_mosaic_dock_target_model_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_dock_target_model_archive_encode, umi_ui_mosaic_dock_target_model_archive_decode,
    UmiUiMosaicDockTargetModel, UmiUiMosaicDockTargetModelArchiveSchema, UmiUiMosaicDockTargetModelArchiveBound, UmiUiMosaicDockTargetModelArchiveWrite, UmiUiMosaicDockTargetModelArchiveRead, UmiUiMosaicDockTargetModelArchiveValidate)
