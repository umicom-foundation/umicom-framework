/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/line_mapping.c
 *
 * PURPOSE:
 *   Map individual lines between comparison sides with confidence evidence.
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
#include "umicom/vcs/advanced/line_mapping.h"

#include <string.h>

/*
 * Initialise vcs advanced line mapping from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_line_mapping_init(UmiVcsAdvancedLineMapping *value)
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
 * Check that vcs advanced line mapping satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_line_mapping_validate(const UmiVcsAdvancedLineMapping *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        ((value->left_line == 0U && value->right_line == 0U) || value->confidence_percent > 100U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced line mapping exact operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_line_mapping_exact(const UmiVcsAdvancedLineMapping *value)
{
    return umi_vcs_advanced_line_mapping_validate(value) == UMI_STATUS_OK &&
           value->confidence_percent == 100U &&
           value->kind == UMI_VCS_DIFF_CONTEXT;
}
