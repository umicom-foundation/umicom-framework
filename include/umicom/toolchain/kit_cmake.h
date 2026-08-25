/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_cmake.h
 * PURPOSE: Render/write CMake toolchain files from Framework-owned kit state.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_CMAKE_H
#define UMICOM_TOOLCHAIN_KIT_CMAKE_H

#include <stddef.h>

#include "umicom/toolchain/kit.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_toolchain_kit_cmake_render(
    const UmiToolchainKitSnapshot *kit,
    char *out_text,
    size_t capacity);
UmiStatus umi_toolchain_kit_cmake_write(
    const UmiToolchainKitSnapshot *kit,
    const char *path);

#ifdef __cplusplus
}
#endif
#endif
