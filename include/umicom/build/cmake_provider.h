/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/cmake_provider.h
 *
 * PURPOSE:
 *   Expose the CMake configure, build and clean provider used by Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_CMAKE_PROVIDER_H
#define UMICOM_BUILD_CMAKE_PROVIDER_H

#include "umicom/build/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiBuildProvider umi_build_cmake_provider(void);

#ifdef __cplusplus
}
#endif

#endif
