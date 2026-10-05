/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/dock_operation.c
 *
 * PURPOSE:
 *   Define toolkit-neutral dock operation contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/dock_operation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic dock operation from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_dock_operation_init(UmiUiMosaicDockOperation *value) {
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
 * Copy ui mosaic dock operation into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_dock_operation_set(UmiUiMosaicDockOperation *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone) {
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
 * Check that ui mosaic dock operation satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ui_mosaic_dock_operation_validate(const UmiUiMosaicDockOperation *value) {
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
 * Provide the ui mosaic dock operation is centre operation used by this module and its
 * client applications.
 */
int umi_ui_mosaic_dock_operation_is_centre(const UmiUiMosaicDockOperation *value) {
    return value != NULL && value->zone == UMI_UI_MOSAIC_DOCK_CENTRE;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicDockOperationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3484bf27d5ed7c8b);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicDockOperation *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicDockOperation *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicDockOperationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicDockOperation *)0)->source_id) - 1U +
        8U + sizeof(((UmiUiMosaicDockOperation *)0)->target_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicDockOperationArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicDockOperation *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->zone);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allowed);
}
static void UmiUiMosaicDockOperationArchiveRead(UmiArchiveReader *reader, UmiUiMosaicDockOperation *value)
{
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->zone = (UmiUiMosaicDockZone)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicDockOperationArchiveValidate(const UmiUiMosaicDockOperation *value)
{
    return umi_ui_mosaic_dock_operation_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_dock_operation_archive_encode, umi_ui_mosaic_dock_operation_archive_decode,
    UmiUiMosaicDockOperation, UmiUiMosaicDockOperationArchiveSchema, UmiUiMosaicDockOperationArchiveBound, UmiUiMosaicDockOperationArchiveWrite, UmiUiMosaicDockOperationArchiveRead, UmiUiMosaicDockOperationArchiveValidate)
