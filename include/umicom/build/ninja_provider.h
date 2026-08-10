/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/ninja_provider.h
 *
 * PURPOSE:
 *   Expose a direct Ninja build and clean provider for configured build trees.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_NINJA_PROVIDER_H
#define UMICOM_BUILD_NINJA_PROVIDER_H

#include "umicom/build/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiBuildProvider umi_build_ninja_provider(void);

#ifdef __cplusplus
}
#endif

#endif
