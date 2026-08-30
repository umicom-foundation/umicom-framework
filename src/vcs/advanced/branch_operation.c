/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/branch_operation.c
 *
 * PURPOSE:
 *   Plan branch create, checkout, rename and deletion without executing Git in UI code.
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
#include "umicom/vcs/advanced/branch_operation.h"
#include <string.h>
static int valid_branch_name(const char *name)
{
    const char *p;
    if (!umi_vcs_advanced_text_present(name) || name[0] == '-' || name[0] == '/') return 0;
    for (p = name; *p != '\0'; ++p)
        if (*p == ' ' || *p == '~' || *p == '^' || *p == ':' || *p == '?' || *p == '*' || *p == '[' || *p == '\\')
            return 0;
    return strstr(name, "..") == NULL && strstr(name, "//") == NULL;
}
void umi_vcs_advanced_branch_operation_init(UmiVcsAdvancedBranchOperation *operation)
{
    if (operation == NULL) return;
    (void)memset(operation, 0, sizeof(*operation));
    operation->struct_size = (uint32_t)sizeof(*operation);
    operation->api_version = UMI_VCS_ADVANCED_API_VERSION;
    operation->safety = UMI_VCS_SAFETY_REVIEW;
}
UmiStatus umi_vcs_advanced_branch_operation_validate(const UmiVcsAdvancedBranchOperation *operation)
{
    if (operation == NULL || operation->struct_size < sizeof(*operation) ||
        operation->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        operation->action > UMI_VCS_BRANCH_DELETE || !valid_branch_name(operation->name))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (operation->action == UMI_VCS_BRANCH_RENAME && !valid_branch_name(operation->new_name))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_vcs_advanced_branch_operation_create(UmiVcsAdvancedBranchOperation *operation,
                                                    const char *name,
                                                    const char *start_point,
                                                    int checkout)
{
    UmiStatus s;
    if (operation == NULL || !valid_branch_name(name)) return UMI_STATUS_INVALID_ARGUMENT;
    operation->action = UMI_VCS_BRANCH_CREATE;
    s = umi_vcs_advanced_copy_text(operation->name, sizeof(operation->name), name);
    if (s != UMI_STATUS_OK) return s;
    s = umi_vcs_advanced_copy_text(operation->start_point, sizeof(operation->start_point), start_point);
    if (s != UMI_STATUS_OK) return s;
    operation->checkout_after_create = checkout != 0;
    return umi_vcs_advanced_branch_operation_validate(operation);
}
UmiStatus umi_vcs_advanced_branch_operation_rename(UmiVcsAdvancedBranchOperation *operation,
                                                    const char *name,
                                                    const char *new_name)
{
    UmiStatus s;
    if (operation == NULL || !valid_branch_name(name) || !valid_branch_name(new_name))
        return UMI_STATUS_INVALID_ARGUMENT;
    operation->action = UMI_VCS_BRANCH_RENAME;
    s = umi_vcs_advanced_copy_text(operation->name, sizeof(operation->name), name);
    if (s != UMI_STATUS_OK) return s;
    s = umi_vcs_advanced_copy_text(operation->new_name, sizeof(operation->new_name), new_name);
    if (s != UMI_STATUS_OK) return s;
    return umi_vcs_advanced_branch_operation_validate(operation);
}
