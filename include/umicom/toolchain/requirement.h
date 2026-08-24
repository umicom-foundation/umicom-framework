/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/requirement.h
 *
 * PURPOSE:
 *   Describe one required or optional native tool for an operation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_TOOLCHAIN_REQUIREMENT_H
#define INCLUDE_UMICOM_TOOLCHAIN_REQUIREMENT_H
#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiToolchainRequirement {
    UmiToolKind kind;
    int required;
    int validate_version;
} UmiToolchainRequirement;

void umi_toolchain_requirement_init(UmiToolchainRequirement *requirement,
                                    UmiToolKind kind,
                                    int required);
UmiStatus umi_toolchain_requirement_validate(const UmiToolchainRequirement *requirement);

#ifdef __cplusplus
}
#endif
#endif
