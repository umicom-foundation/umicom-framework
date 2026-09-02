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
