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
#include <stdbool.h>

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
/* An automatic request chooses its stages before any build can create a driver.
 * The CLI verifies an already configured tree (configure_first=false); an IDE
 * may bootstrap a new tree (true). An available driver owns all three stages.
 * Preset profiles retain ordinary stages and disabled testing omits Test.
 * Preparing a plan starts no process and creates no files. Failure leaves the
 * output untouched. Retain the plan for the request; do not upgrade an ordinary
 * Test stage to the automatic provider after work starts. */
typedef struct UmiBuildAutomaticPlan {
    UmiBuildPhase phases[3];
    size_t phase_count;
    bool uses_driver;
} UmiBuildAutomaticPlan;
UmiStatus UmiBuildAutomaticPlanPrepare(const UmiBuildProfile *profile,
    bool configure_first, UmiBuildAutomaticPlan *out_plan);

UmiBuildProvider UmiBuildAutomaticTestProvider(void);

#ifdef __cplusplus
}
#endif

#endif
