/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_navigation.c
 *
 * PURPOSE:
 *   Track deterministic next/previous change navigation in comparison sessions.
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
#include "umicom/vcs/advanced/compare_navigation.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced compare navigation from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_compare_navigation_init(UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->wrap = 1;
}

/*
 * Check that vcs advanced compare navigation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_compare_navigation_validate(const UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->change_count > 0U && value->current_index >= value->change_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced compare navigation next operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_compare_navigation_next(UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->change_count == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->current_index + 1U < value->change_count) {
        value->current_index += 1U;
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->wrap) {
        value->current_index = 0U;
        return 1;
    }
    return 0;
}
/*
 * Provide the vcs advanced compare navigation previous operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_compare_navigation_previous(UmiVcsAdvancedCompareNavigation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->change_count == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->current_index > 0U) {
        value->current_index -= 1U;
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->wrap) {
        value->current_index = value->change_count - 1U;
        return 1;
    }
    return 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedCompareNavigationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xff820525a806ab85);

    return schema;
}
static size_t UmiVcsAdvancedCompareNavigationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedCompareNavigationArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedCompareNavigation *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->change_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->current_index);
    UmiArchiveWriteSigned(writer, (int64_t)value->wrap);
}
static void UmiVcsAdvancedCompareNavigationArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedCompareNavigation *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->change_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->current_index = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->wrap = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedCompareNavigationArchiveValidate(const UmiVcsAdvancedCompareNavigation *value)
{
    return umi_vcs_advanced_compare_navigation_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_compare_navigation_archive_encode, umi_vcs_advanced_compare_navigation_archive_decode,
    UmiVcsAdvancedCompareNavigation, UmiVcsAdvancedCompareNavigationArchiveSchema, UmiVcsAdvancedCompareNavigationArchiveBound, UmiVcsAdvancedCompareNavigationArchiveWrite, UmiVcsAdvancedCompareNavigationArchiveRead, UmiVcsAdvancedCompareNavigationArchiveValidate)
