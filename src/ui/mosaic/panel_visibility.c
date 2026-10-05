/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/panel_visibility.c
 *
 * PURPOSE:
 *   Define toolkit-neutral panel visibility contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/panel_visibility.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic panel visibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_panel_visibility_init(UmiUiMosaicPanelVisibility *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->enabled = true;
}

/*
 * Copy ui mosaic panel visibility into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_panel_visibility_set(UmiUiMosaicPanelVisibility *value, const char *id, const char *title) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->title, sizeof(value->title), title);
}

/*
 * Check that ui mosaic panel visibility satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_panel_visibility_validate(const UmiUiMosaicPanelVisibility *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->title, '\0', sizeof(value->title)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->title[0] == '\0') return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic panel visibility rank operation used by this module and its client
 * applications.
 */
uint32_t umi_ui_mosaic_panel_visibility_rank(const UmiUiMosaicPanelVisibility *value, uint32_t context_boost) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0U;
    return value->priority + context_boost + 1U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicPanelVisibilityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0a1288b7b59d9d8e);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicPanelVisibility *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicPanelVisibility *)0)->title)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicPanelVisibilityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicPanelVisibility *)0)->id) - 1U +
        8U + sizeof(((UmiUiMosaicPanelVisibility *)0)->title) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiMosaicPanelVisibilityArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicPanelVisibility *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteSigned(writer, (int64_t)value->application);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiUiMosaicPanelVisibilityArchiveRead(UmiArchiveReader *reader, UmiUiMosaicPanelVisibility *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    value->application = (UmiUiMosaicApplication)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicPanelVisibilityArchiveValidate(const UmiUiMosaicPanelVisibility *value)
{
    return umi_ui_mosaic_panel_visibility_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_panel_visibility_archive_encode, umi_ui_mosaic_panel_visibility_archive_decode,
    UmiUiMosaicPanelVisibility, UmiUiMosaicPanelVisibilityArchiveSchema, UmiUiMosaicPanelVisibilityArchiveBound, UmiUiMosaicPanelVisibilityArchiveWrite, UmiUiMosaicPanelVisibilityArchiveRead, UmiUiMosaicPanelVisibilityArchiveValidate)
