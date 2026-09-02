/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/tag_operation.c
 *
 * PURPOSE:
 *   Plan tag creation, deletion or verification with explicit signing intent.
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
#include "umicom/vcs/advanced/tag_operation.h"

#include <string.h>

/*
 * Initialise vcs advanced tag operation from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_tag_operation_init(UmiVcsAdvancedTagOperation *value)
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
 * Check that vcs advanced tag operation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_tag_operation_validate(const UmiVcsAdvancedTagOperation *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->tag_name) || (value->create == 0 && value->delete_tag == 0 && value->verify == 0))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise vcs advanced tag operation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_vcs_advanced_tag_operation_create(UmiVcsAdvancedTagOperation *value,
                                                  const char *tag_name,
                                                  const char *target,
                                                  const char *message,
                                                  int sign)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->tag_name, sizeof(value->tag_name), tag_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->target, sizeof(value->target), target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->message, sizeof(value->message), message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->create = 1;
    value->annotated = umi_vcs_advanced_text_present(message);
    value->sign = sign != 0;
    return umi_vcs_advanced_tag_operation_validate(value);
}
