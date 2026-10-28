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
/* Automatic execution uses a generated driver in this configured build tree.
 * Missing drivers and preset-owned profiles retain ordinary full CTest semantics.
 * No file is created or process started while preparing the command. */
/* Read-only discovery shared by the CLI and project worker. NOT_FOUND means
 * use ordinary build/test stages; other failures must stop the operation.
 * On success driver contains the absolute configured script path. */
UmiStatus UmiBuildAutomaticTestDriver(const UmiBuildProfile *profile,
    char *driver, size_t capacity);
UmiBuildProvider UmiBuildAutomaticTestProvider(void);

#ifdef __cplusplus
}
#endif

#endif
