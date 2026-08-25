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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/tag_operation.h"

#include <string.h>

void umi_vcs_advanced_tag_operation_init(UmiVcsAdvancedTagOperation *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_tag_operation_validate(const UmiVcsAdvancedTagOperation *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->tag_name) || (value->create == 0 && value->delete_tag == 0 && value->verify == 0))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_tag_operation_create(UmiVcsAdvancedTagOperation *value,
                                                  const char *tag_name,
                                                  const char *target,
                                                  const char *message,
                                                  int sign)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->tag_name, sizeof(value->tag_name), tag_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->target, sizeof(value->target), target);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->message, sizeof(value->message), message);
    if (status != UMI_STATUS_OK) return status;
    value->create = 1;
    value->annotated = umi_vcs_advanced_text_present(message);
    value->sign = sign != 0;
    return umi_vcs_advanced_tag_operation_validate(value);
}
