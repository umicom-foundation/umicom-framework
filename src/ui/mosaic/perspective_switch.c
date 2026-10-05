/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/perspective_switch.c
 *
 * PURPOSE:
 *   Define toolkit-neutral perspective switch contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/perspective_switch.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic perspective switch from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_perspective_switch_init(UmiUiMosaicPerspectiveSwitch *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->available = true;
}

/*
 * Copy ui mosaic perspective switch into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_perspective_switch_set(UmiUiMosaicPerspectiveSwitch *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->name, sizeof(value->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->layout_id, sizeof(value->layout_id), layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->application = application;
    return UMI_STATUS_OK;
}

/*
 * Check that ui mosaic perspective switch satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_perspective_switch_validate(const UmiUiMosaicPerspectiveSwitch *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->layout_id, '\0', sizeof(value->layout_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->available || !umi_ui_mosaic_id_is_valid(value->id) || !umi_ui_mosaic_id_is_valid(value->layout_id) || value->name[0] == '\0') return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicPerspectiveSwitchArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x31e15846e03d52cd);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicPerspectiveSwitch *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicPerspectiveSwitch *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicPerspectiveSwitch *)0)->layout_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicPerspectiveSwitchArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicPerspectiveSwitch *)0)->id) - 1U +
        8U + sizeof(((UmiUiMosaicPerspectiveSwitch *)0)->name) - 1U +
        8U + sizeof(((UmiUiMosaicPerspectiveSwitch *)0)->layout_id) - 1U +
        8U +
        8U;
}
static void UmiUiMosaicPerspectiveSwitchArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicPerspectiveSwitch *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->layout_id, sizeof(value->layout_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->application);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->available);
}
static void UmiUiMosaicPerspectiveSwitchArchiveRead(UmiArchiveReader *reader, UmiUiMosaicPerspectiveSwitch *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->layout_id, sizeof(value->layout_id));
    value->application = (UmiUiMosaicApplication)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->available = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicPerspectiveSwitchArchiveValidate(const UmiUiMosaicPerspectiveSwitch *value)
{
    return umi_ui_mosaic_perspective_switch_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_perspective_switch_archive_encode, umi_ui_mosaic_perspective_switch_archive_decode,
    UmiUiMosaicPerspectiveSwitch, UmiUiMosaicPerspectiveSwitchArchiveSchema, UmiUiMosaicPerspectiveSwitchArchiveBound, UmiUiMosaicPerspectiveSwitchArchiveWrite, UmiUiMosaicPerspectiveSwitchArchiveRead, UmiUiMosaicPerspectiveSwitchArchiveValidate)
