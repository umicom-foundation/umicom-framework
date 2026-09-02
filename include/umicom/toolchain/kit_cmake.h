/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_cmake.h
 * PURPOSE: Render/write CMake toolchain files from Framework-owned kit state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_CMAKE_H
#define UMICOM_TOOLCHAIN_KIT_CMAKE_H

#include <stddef.h>

#include "umicom/toolchain/kit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the toolchain kit cmake render operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_cmake_render(
    const UmiToolchainKitSnapshot *kit,
    char *out_text,
    size_t capacity);
/**
 * Write toolchain kit cmake in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_toolchain_kit_cmake_write(
    const UmiToolchainKitSnapshot *kit,
    const char *path);

#ifdef __cplusplus
}
#endif
#endif
