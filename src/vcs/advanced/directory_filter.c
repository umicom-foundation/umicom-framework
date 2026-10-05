/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/directory_filter.c
 *
 * PURPOSE:
 *   Implement deterministic inclusion policy for large directory comparisons.
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
#include "umicom/vcs/advanced/directory_filter.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced directory filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_directory_filter_init(UmiVcsAdvancedDirectoryFilter *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->maximum_size_bytes = UINT64_MAX;
    value->include_directories = 1;
    value->include_binary = 1;
}

/*
 * Check that vcs advanced directory filter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_directory_filter_validate(const UmiVcsAdvancedDirectoryFilter *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->extension, '\0', sizeof(value->extension)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->path_prefix, '\0', sizeof(value->path_prefix)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->maximum_size_bytes == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the has suffix operation used by this module and its client applications. */
static int has_suffix(const char *text, const char *suffix)
{
    size_t text_length;
    size_t suffix_length;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_text_present(suffix)) return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_length = strlen(text);
    suffix_length = strlen(suffix);
    /* Apply this branch only when its contract condition is satisfied. */
    if (suffix_length > text_length) return 0;
    return strcmp(text + (text_length - suffix_length), suffix) == 0;
}
/*
 * Provide the vcs advanced directory filter accept operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_directory_filter_accept(const UmiVcsAdvancedDirectoryFilter *filter,
                                                const char *relative_path,
                                                uint64_t size_bytes,
                                                int directory,
                                                int hidden,
                                                int binary)
{
    size_t prefix_length;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_directory_filter_validate(filter) != UMI_STATUS_OK ||
        !umi_vcs_advanced_text_present(relative_path)) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (size_bytes > filter->maximum_size_bytes) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (hidden && !filter->include_hidden) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (directory && !filter->include_directories) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (binary && !filter->include_binary) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!has_suffix(relative_path, filter->extension)) return 0;
    prefix_length = strlen(filter->path_prefix);
    return prefix_length == 0U || strncmp(relative_path, filter->path_prefix, prefix_length) == 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedDirectoryFilterArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x358777a1073095ee);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedDirectoryFilter *)0)->extension)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedDirectoryFilter *)0)->path_prefix)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiVcsAdvancedDirectoryFilterArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiVcsAdvancedDirectoryFilter *)0)->extension) - 1U +
        8U + sizeof(((UmiVcsAdvancedDirectoryFilter *)0)->path_prefix) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedDirectoryFilterArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedDirectoryFilter *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->extension, sizeof(value->extension));
    UmiArchiveWriteText(writer, value->path_prefix, sizeof(value->path_prefix));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_size_bytes);
    UmiArchiveWriteSigned(writer, (int64_t)value->include_hidden);
    UmiArchiveWriteSigned(writer, (int64_t)value->include_directories);
    UmiArchiveWriteSigned(writer, (int64_t)value->include_binary);
}
static void UmiVcsAdvancedDirectoryFilterArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedDirectoryFilter *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->extension, sizeof(value->extension));
    UmiArchiveReadText(reader, value->path_prefix, sizeof(value->path_prefix));
    value->maximum_size_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->include_hidden = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->include_directories = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->include_binary = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedDirectoryFilterArchiveValidate(const UmiVcsAdvancedDirectoryFilter *value)
{
    return umi_vcs_advanced_directory_filter_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_directory_filter_archive_encode, umi_vcs_advanced_directory_filter_archive_decode,
    UmiVcsAdvancedDirectoryFilter, UmiVcsAdvancedDirectoryFilterArchiveSchema, UmiVcsAdvancedDirectoryFilterArchiveBound, UmiVcsAdvancedDirectoryFilterArchiveWrite, UmiVcsAdvancedDirectoryFilterArchiveRead, UmiVcsAdvancedDirectoryFilterArchiveValidate)
