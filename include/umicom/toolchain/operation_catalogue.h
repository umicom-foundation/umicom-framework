/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/operation_catalogue.h
 *
 * PURPOSE:
 *   Provide Framework-owned native tool requirements for common operations.
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
#ifndef INCLUDE_UMICOM_TOOLCHAIN_OPERATION_CATALOGUE_H
#define INCLUDE_UMICOM_TOOLCHAIN_OPERATION_CATALOGUE_H
#include "umicom/toolchain/operation_profile.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_toolchain_operation_catalogue_profile(
    UmiToolchainOperationKind kind,
    UmiToolchainOperationProfile *out_profile);

#ifdef __cplusplus
}
#endif
#endif
