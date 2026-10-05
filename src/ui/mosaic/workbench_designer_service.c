/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/workbench_designer_service.c
 *
 * PURPOSE:
 *   Aggregate selection, drag, drop-preview and edit-session state for a renderer-neutral Layout Designer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/workbench_designer_service.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic workbench designer service from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_mosaic_workbench_designer_service_init(UmiUiMosaicWorkbenchDesignerService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->revision = 1U;
    value->mode = UMI_UI_MOSAIC_EDIT_LOCKED;
    value->valid = true;
}

/*
 * Provide the ui mosaic workbench designer service bind operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_mosaic_workbench_designer_service_bind(UmiUiMosaicWorkbenchDesignerService *value, const char *workspace_id, const char *active_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->workspace_id, sizeof(value->workspace_id), workspace_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->active_id, sizeof(value->active_id), active_id);
}

/*
 * Check that ui mosaic workbench designer service satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_ui_mosaic_workbench_designer_service_validate(const UmiUiMosaicWorkbenchDesignerService *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->workspace_id, '\0', sizeof(value->workspace_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->active_id, '\0', sizeof(value->active_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->valid || !umi_ui_mosaic_id_is_valid(value->workspace_id) || !umi_ui_mosaic_id_is_valid(value->active_id)) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->selection_count > UMI_UI_MOSAIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic workbench designer service advance operation used by this module
 * and its client applications.
 */
UmiStatus umi_ui_mosaic_workbench_designer_service_advance(UmiUiMosaicWorkbenchDesignerService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return UMI_STATUS_PERMISSION_DENIED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->revision == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicWorkbenchDesignerServiceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe2fa4926d755a68b);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicWorkbenchDesignerService *)0)->workspace_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicWorkbenchDesignerService *)0)->active_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicWorkbenchDesignerServiceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicWorkbenchDesignerService *)0)->workspace_id) - 1U +
        8U + sizeof(((UmiUiMosaicWorkbenchDesignerService *)0)->active_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicWorkbenchDesignerServiceArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicWorkbenchDesignerService *value)
{
    UmiArchiveWriteText(writer, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveWriteText(writer, value->active_id, sizeof(value->active_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selection_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->valid);
}
static void UmiUiMosaicWorkbenchDesignerServiceArchiveRead(UmiArchiveReader *reader, UmiUiMosaicWorkbenchDesignerService *value)
{
    UmiArchiveReadText(reader, value->workspace_id, sizeof(value->workspace_id));
    UmiArchiveReadText(reader, value->active_id, sizeof(value->active_id));
    value->revision = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->selection_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->mode = (UmiUiMosaicEditMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->valid = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicWorkbenchDesignerServiceArchiveValidate(const UmiUiMosaicWorkbenchDesignerService *value)
{
    return umi_ui_mosaic_workbench_designer_service_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_workbench_designer_service_archive_encode, umi_ui_mosaic_workbench_designer_service_archive_decode,
    UmiUiMosaicWorkbenchDesignerService, UmiUiMosaicWorkbenchDesignerServiceArchiveSchema, UmiUiMosaicWorkbenchDesignerServiceArchiveBound, UmiUiMosaicWorkbenchDesignerServiceArchiveWrite, UmiUiMosaicWorkbenchDesignerServiceArchiveRead, UmiUiMosaicWorkbenchDesignerServiceArchiveValidate)
