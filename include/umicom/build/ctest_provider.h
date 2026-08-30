/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/ctest_provider.h
 *
 * PURPOSE:
 *   Expose the CTest provider for configured Framework and product test trees.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_CTEST_PROVIDER_H
#define UMICOM_BUILD_CTEST_PROVIDER_H

#include "umicom/build/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiBuildProvider umi_build_ctest_provider(void);

#ifdef __cplusplus
}
#endif

#endif
