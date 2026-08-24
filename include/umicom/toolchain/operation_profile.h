/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/operation_profile.h
 *
 * PURPOSE:
 *   Define reusable operation profiles that declare only the tools they need.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_OPERATION_PROFILE_H
#define INCLUDE_UMICOM_TOOLCHAIN_OPERATION_PROFILE_H
#include "umicom/toolchain/operation_types.h"
#include "umicom/toolchain/requirement_set.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiToolchainOperationProfile {
    char id[UMI_TOOLCHAIN_OPERATION_ID_CAPACITY];
    char display_name[UMI_TOOLCHAIN_OPERATION_TEXT_CAPACITY];
    UmiToolchainOperationKind kind;
    UmiToolchainRequirementSet requirements;
    int requires_compiler;
    int run_compile_probe;
    int inherit_path;
} UmiToolchainOperationProfile;

void umi_toolchain_operation_profile_init(UmiToolchainOperationProfile *profile,
                                          UmiToolchainOperationKind kind,
                                          const char *id,
                                          const char *display_name);
UmiStatus umi_toolchain_operation_profile_validate(
    const UmiToolchainOperationProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
