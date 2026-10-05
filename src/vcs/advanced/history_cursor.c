/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/history_cursor.c
 *
 * PURPOSE:
 *   Track deterministic pagination through large repository histories.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/history_cursor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced history cursor from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_history_cursor_init(UmiVcsAdvancedHistoryCursor *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->limit = 100U;
}

/*
 * Check that vcs advanced history cursor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_history_cursor_validate(const UmiVcsAdvancedHistoryCursor *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->limit == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced history cursor advance operation used by this module and its
 * client applications.
 */
void umi_vcs_advanced_history_cursor_advance(UmiVcsAdvancedHistoryCursor *value,
                                                size_t returned,
                                                int has_more)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    value->returned = returned;
    value->offset += returned;
    value->has_more = has_more != 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedHistoryCursorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5ff7a8630113ebbd);

    return schema;
}
static size_t UmiVcsAdvancedHistoryCursorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedHistoryCursorArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedHistoryCursor *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->offset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->limit);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->returned);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->total_hint);
    UmiArchiveWriteSigned(writer, (int64_t)value->has_more);
}
static void UmiVcsAdvancedHistoryCursorArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedHistoryCursor *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->offset = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->limit = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->returned = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->total_hint = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->has_more = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedHistoryCursorArchiveValidate(const UmiVcsAdvancedHistoryCursor *value)
{
    return umi_vcs_advanced_history_cursor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_history_cursor_archive_encode, umi_vcs_advanced_history_cursor_archive_decode,
    UmiVcsAdvancedHistoryCursor, UmiVcsAdvancedHistoryCursorArchiveSchema, UmiVcsAdvancedHistoryCursorArchiveBound, UmiVcsAdvancedHistoryCursorArchiveWrite, UmiVcsAdvancedHistoryCursorArchiveRead, UmiVcsAdvancedHistoryCursorArchiveValidate)
