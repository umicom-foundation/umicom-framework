/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/operation_profile.c
 *
 * PURPOSE:
 *   Implement operation profile construction and validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/operation_profile.h"

#include <stdio.h>
#include <string.h>

void umi_toolchain_operation_profile_init(UmiToolchainOperationProfile *profile,
                                          UmiToolchainOperationKind kind,
                                          const char *id,
                                          const char *display_name)
{
    if (profile == NULL) return;
    (void)memset(profile, 0, sizeof(*profile));
    profile->kind = kind;
    profile->inherit_path = 1;
    umi_toolchain_requirement_set_init(&profile->requirements);
    if (id != NULL) (void)snprintf(profile->id, sizeof(profile->id), "%s", id);
    if (display_name != NULL) {
        (void)snprintf(profile->display_name, sizeof(profile->display_name), "%s", display_name);
    }
}

UmiStatus umi_toolchain_operation_profile_validate(
    const UmiToolchainOperationProfile *profile)
{
    if (profile == NULL || profile->kind <= UMI_TOOLCHAIN_OPERATION_UNKNOWN ||
        profile->kind > UMI_TOOLCHAIN_OPERATION_PACKAGE ||
        profile->id[0] == '\0' || profile->display_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_toolchain_requirement_set_validate(&profile->requirements);
}
