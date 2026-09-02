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
#include "umicom/compiler/registry.h"
#include "umicom/toolchain/profile.h"
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
#endif
