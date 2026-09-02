/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_line.c
 *
 * PURPOSE:
 *   Represent a normalized line in a side-by-side or inline comparison.
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
#include "umicom/vcs/advanced/diff_line.h"

#include <string.h>

/*
 * Initialise vcs advanced diff line from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_line_init(UmiVcsAdvancedDiffLine *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced diff line satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_diff_line_validate(const UmiVcsAdvancedDiffLine *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->kind > UMI_VCS_DIFF_MOVED)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced diff line into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_vcs_advanced_diff_line_set(UmiVcsAdvancedDiffLine *value,
                                           UmiVcsDiffKind kind,
                                           size_t old_line,
                                           size_t new_line,
                                           const char *text)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->kind = kind;
    value->old_line = old_line;
    value->new_line = new_line;
    status = umi_vcs_advanced_copy_text(value->text, sizeof(value->text), text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->fingerprint = umi_vcs_advanced_hash_text(text);
    return umi_vcs_advanced_diff_line_validate(value);
}
