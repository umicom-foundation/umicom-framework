/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/dock_direction.c
 *
 * PURPOSE:
 *   Define toolkit-neutral dock direction contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/dock_direction.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic dock direction from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_dock_direction_init(UmiUiMosaicDockDirection *value) {
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
 * Copy ui mosaic dock direction into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_dock_direction_set(UmiUiMosaicDockDirection *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone) {
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
 * Check that ui mosaic dock direction satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ui_mosaic_dock_direction_validate(const UmiUiMosaicDockDirection *value) {
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
 * Provide the ui mosaic dock direction is centre operation used by this module and its
 * client applications.
 */
int umi_ui_mosaic_dock_direction_is_centre(const UmiUiMosaicDockDirection *value) {
    return value != NULL && value->zone == UMI_UI_MOSAIC_DOCK_CENTRE;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicDockDirectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdd48aba0d18c4e11);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicDockDirection *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicDockDirection *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicDockDirectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicDockDirection *)0)->source_id) - 1U +
        8U + sizeof(((UmiUiMosaicDockDirection *)0)->target_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicDockDirectionArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicDockDirection *value)
{
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->zone);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allowed);
}
static void UmiUiMosaicDockDirectionArchiveRead(UmiArchiveReader *reader, UmiUiMosaicDockDirection *value)
{
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->zone = (UmiUiMosaicDockZone)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicDockDirectionArchiveValidate(const UmiUiMosaicDockDirection *value)
{
    return umi_ui_mosaic_dock_direction_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_dock_direction_archive_encode, umi_ui_mosaic_dock_direction_archive_decode,
    UmiUiMosaicDockDirection, UmiUiMosaicDockDirectionArchiveSchema, UmiUiMosaicDockDirectionArchiveBound, UmiUiMosaicDockDirectionArchiveWrite, UmiUiMosaicDockDirectionArchiveRead, UmiUiMosaicDockDirectionArchiveValidate)
