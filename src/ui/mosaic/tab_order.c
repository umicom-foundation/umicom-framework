/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/tab_order.c
 *
 * PURPOSE:
 *   Define toolkit-neutral tab order contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/tab_order.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic tab order from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_mosaic_tab_order_init(UmiUiMosaicTabOrder *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->revision = 1U;
}

/*
 * Copy ui mosaic tab order into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_mosaic_tab_order_set(UmiUiMosaicTabOrder *value, const char *id, const char *name) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->name, sizeof(value->name), name);
}

/*
 * Check that ui mosaic tab order satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ui_mosaic_tab_order_validate(const UmiUiMosaicTabOrder *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->name[0] == '\0' || value->revision == 0U) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->item_count > UMI_UI_MOSAIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic tab order touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_mosaic_tab_order_touch(UmiUiMosaicTabOrder *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->locked) return UMI_STATUS_PERMISSION_DENIED;
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
static uint64_t UmiUiMosaicTabOrderArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe00dbac4b85f4f8a);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicTabOrder *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicTabOrder *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicTabOrderArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicTabOrder *)0)->id) - 1U +
        8U + sizeof(((UmiUiMosaicTabOrder *)0)->name) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicTabOrderArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicTabOrder *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->locked);
}
static void UmiUiMosaicTabOrderArchiveRead(UmiArchiveReader *reader, UmiUiMosaicTabOrder *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->revision = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->item_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->locked = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicTabOrderArchiveValidate(const UmiUiMosaicTabOrder *value)
{
    return umi_ui_mosaic_tab_order_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_tab_order_archive_encode, umi_ui_mosaic_tab_order_archive_decode,
    UmiUiMosaicTabOrder, UmiUiMosaicTabOrderArchiveSchema, UmiUiMosaicTabOrderArchiveBound, UmiUiMosaicTabOrderArchiveWrite, UmiUiMosaicTabOrderArchiveRead, UmiUiMosaicTabOrderArchiveValidate)
