/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/toolchain_bridge.h
 *
 * PURPOSE:
 *   Publish the public toolchain bridge contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_TOOLCHAIN_BRIDGE_H
#define UMICOM_COMPILER_TOOLCHAIN_BRIDGE_H
#include "umicom/compiler/profile.h"
#include "umicom/compiler/centre.h"
#include "umicom/compiler/registry.h"
#include "umicom/toolchain/profile.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the compiler registry import toolchain operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_registry_import_toolchain(UmiCompilerRegistry *registry,const UmiToolchainProfile *profile,size_t *out_imported);
/**
 * Provide the compiler profile from toolchain operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_profile_from_toolchain(const UmiToolchainProfile *toolchain,const char *profile_id,UmiCompilerProfile *out_profile);
/** Conversion consumes compatible profile storage, bounded display/target text,
 * and an explicitly selected GCC, Clang or MSVC C compiler. FOUND/VALIDATED
 * metadata is admitted; MISSING/FAILED is unavailable. Other kinds are unmapped.
 * profile_from_toolchain preserves its destination on failure. Import remains
 * add-only and reports zero imported on failure after non-null input admission.
 * out_imported must not overlap registry/profile storage. No tool is probed. */

/** Replace the selected provider and named compiler profile together, activating
 * that profile only after complete admission. Uses centre_configure_profile;
 * any failure preserves the centre. The profile uses Framework debug defaults,
 * so custom policy must be applied separately through configure_profile. */
UmiStatus umi_compiler_centre_sync_toolchain(UmiCompilerCentre *centre,
    const UmiToolchainProfile *toolchain,const char *profile_id);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
