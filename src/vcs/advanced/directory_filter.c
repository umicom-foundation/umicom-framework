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

#include <string.h>

void umi_vcs_advanced_directory_filter_init(UmiVcsAdvancedDirectoryFilter *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->maximum_size_bytes = UINT64_MAX;
    value->include_directories = 1;
    value->include_binary = 1;
}

UmiStatus umi_vcs_advanced_directory_filter_validate(const UmiVcsAdvancedDirectoryFilter *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->maximum_size_bytes == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static int has_suffix(const char *text, const char *suffix)
{
    size_t text_length;
    size_t suffix_length;
    if (!umi_vcs_advanced_text_present(suffix)) return 1;
    if (text == NULL) return 0;
    text_length = strlen(text);
    suffix_length = strlen(suffix);
    if (suffix_length > text_length) return 0;
    return strcmp(text + (text_length - suffix_length), suffix) == 0;
}
int umi_vcs_advanced_directory_filter_accept(const UmiVcsAdvancedDirectoryFilter *filter,
                                                const char *relative_path,
                                                uint64_t size_bytes,
                                                int directory,
                                                int hidden,
                                                int binary)
{
    size_t prefix_length;
    if (umi_vcs_advanced_directory_filter_validate(filter) != UMI_STATUS_OK ||
        !umi_vcs_advanced_text_present(relative_path)) return 0;
    if (size_bytes > filter->maximum_size_bytes) return 0;
    if (hidden && !filter->include_hidden) return 0;
    if (directory && !filter->include_directories) return 0;
    if (binary && !filter->include_binary) return 0;
    if (!has_suffix(relative_path, filter->extension)) return 0;
    prefix_length = strlen(filter->path_prefix);
    return prefix_length == 0U || strncmp(relative_path, filter->path_prefix, prefix_length) == 0;
}
