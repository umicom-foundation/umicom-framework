/* Umicom Framework | Toolchain-to-compiler bridge | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_TOOLCHAIN_BRIDGE_H
#define UMICOM_COMPILER_TOOLCHAIN_BRIDGE_H
#include "umicom/compiler/profile.h"
#include "umicom/compiler/registry.h"
#include "umicom/toolchain/profile.h"
UmiStatus umi_compiler_registry_import_toolchain(UmiCompilerRegistry *registry,const UmiToolchainProfile *profile,size_t *out_imported);
UmiStatus umi_compiler_profile_from_toolchain(const UmiToolchainProfile *toolchain,const char *profile_id,UmiCompilerProfile *out_profile);
#endif
