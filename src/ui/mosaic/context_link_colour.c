/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/context_link_colour.c
 *
 * PURPOSE:
 *   Define toolkit-neutral context link colour contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/context_link_colour.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/*
 * Initialise ui mosaic context link colour from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_context_link_colour_init(UmiUiMosaicContextLinkColour *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->bidirectional = true;
}

/*
 * Copy ui mosaic context link colour into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_ui_mosaic_context_link_colour_set(UmiUiMosaicContextLinkColour *value, const char *group_id, const char *context_type, const char *member_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->group_id, sizeof(value->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->context_type, sizeof(value->context_type), context_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->member_id, sizeof(value->member_id), member_id);
}

/*
 * Check that ui mosaic context link colour satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_context_link_colour_validate(const UmiUiMosaicContextLinkColour *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->group_id, '\0', sizeof(value->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->context_type, '\0', sizeof(value->context_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->member_id, '\0', sizeof(value->member_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->group_id) || !umi_ui_mosaic_id_is_valid(value->context_type) || !umi_ui_mosaic_id_is_valid(value->member_id)) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value->colour_index >= 16U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiMosaicContextLinkColourArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3236872855c8829f);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicContextLinkColour *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicContextLinkColour *)0)->context_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiMosaicContextLinkColour *)0)->member_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiMosaicContextLinkColourArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiMosaicContextLinkColour *)0)->group_id) - 1U +
        8U + sizeof(((UmiUiMosaicContextLinkColour *)0)->context_type) - 1U +
        8U + sizeof(((UmiUiMosaicContextLinkColour *)0)->member_id) - 1U +
        8U +
        8U;
}
static void UmiUiMosaicContextLinkColourArchiveWrite(UmiArchiveWriter *writer, const UmiUiMosaicContextLinkColour *value)
{
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->context_type, sizeof(value->context_type));
    UmiArchiveWriteText(writer, value->member_id, sizeof(value->member_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->colour_index);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bidirectional);
}
static void UmiUiMosaicContextLinkColourArchiveRead(UmiArchiveReader *reader, UmiUiMosaicContextLinkColour *value)
{
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->context_type, sizeof(value->context_type));
    UmiArchiveReadText(reader, value->member_id, sizeof(value->member_id));
    value->colour_index = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->bidirectional = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiMosaicContextLinkColourArchiveValidate(const UmiUiMosaicContextLinkColour *value)
{
    return umi_ui_mosaic_context_link_colour_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_mosaic_context_link_colour_archive_encode, umi_ui_mosaic_context_link_colour_archive_decode,
    UmiUiMosaicContextLinkColour, UmiUiMosaicContextLinkColourArchiveSchema, UmiUiMosaicContextLinkColourArchiveBound, UmiUiMosaicContextLinkColourArchiveWrite, UmiUiMosaicContextLinkColourArchiveRead, UmiUiMosaicContextLinkColourArchiveValidate)
