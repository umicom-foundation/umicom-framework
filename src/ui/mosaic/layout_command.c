/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/layout_command.c
 *
 * PURPOSE:
 *   Define toolkit-neutral layout command contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/layout_command.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic layout command from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_layout_command_init(UmiUiMosaicLayoutCommand *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->enabled = true;
}

/*
 * Copy ui mosaic layout command into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_layout_command_set(UmiUiMosaicLayoutCommand *value, const char *id, const char *label) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->label, sizeof(value->label), label);
}

/*
 * Check that ui mosaic layout command satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ui_mosaic_layout_command_validate(const UmiUiMosaicLayoutCommand *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->label[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Perform ui mosaic layout command can through the module contract so client applications
 * do not duplicate its policy.
 */
int umi_ui_mosaic_layout_command_can_execute(const UmiUiMosaicLayoutCommand *value, UmiUiMosaicEditMode mode) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->requires_edit_mode && mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return 0;
    return 1;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicLayoutCommandArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5a10caf49a6ce076);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicLayoutCommand *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicLayoutCommand *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicLayoutCommandArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicLayoutCommand *)0)->id) - 1U +
        8U + sizeof(((UmiUiMosaicLayoutCommand *)0)->label) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicLayoutCommandArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicLayoutCommand *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->ordinal);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->requires_edit_mode);
}
static void UmiUiMosaicLayoutCommandArchiveRead(UmiArchiveReader *reader, UmiUiMosaicLayoutCommand *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->ordinal = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->requires_edit_mode = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicLayoutCommandArchiveValidate(const UmiUiMosaicLayoutCommand *value)
{
    return umi_ui_mosaic_layout_command_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_layout_command_archive_encode, umi_ui_mosaic_layout_command_archive_decode,
    UmiUiMosaicLayoutCommand, UmiUiMosaicLayoutCommandArchiveSchema, UmiUiMosaicLayoutCommandArchiveBound, UmiUiMosaicLayoutCommandArchiveWrite, UmiUiMosaicLayoutCommandArchiveRead, UmiUiMosaicLayoutCommandArchiveValidate)
