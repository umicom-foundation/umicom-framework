/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/readiness.h
 *
 * PURPOSE:
 *   Explain which lifecycle operations are currently available before a menu,
 *   toolbar or AI agent offers them to the user.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_READINESS_H
#define UMICOM_DEVELOPER_WORKBENCH_READINESS_H

#include "umicom/developer_workbench/configuration.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchReadiness {
    int configuration_valid;
    int toolchain_ready;
    int can_configure;
    int can_build;
    int can_test;
    int can_run;
    int can_install;
    int can_package;
    char reason[UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY];
} UmiDeveloperWorkbenchReadiness;

UmiStatus umi_developer_workbench_readiness_evaluate(
    const UmiDeveloperWorkbenchConfiguration *configuration,
    const UmiToolchainProfile *profile,
    UmiDeveloperWorkbenchReadiness *out_readiness);

#ifdef __cplusplus
}
#endif

#endif
