/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/runner.h
 *
 * PURPOSE:
 *   Execute build phases through provider commands, capture diagnostics and retain history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RUNNER_H
#define UMICOM_BUILD_RUNNER_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/history.h"
#include "umicom/build/profile.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/clock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildRunner UmiBuildRunner;

typedef struct UmiBuildRunnerConfig {
    UmiBuildProfile profile;
    UmiBuildHistory *history;
    UmiClock *clock;
    UmiCancellationToken *cancellation;
} UmiBuildRunnerConfig;

UmiStatus umi_build_runner_create(const UmiBuildRunnerConfig *config,
                                  UmiBuildRunner **out_runner);
void umi_build_runner_destroy(UmiBuildRunner *runner);
UmiStatus umi_build_runner_set_profile(UmiBuildRunner *runner,
                                       const UmiBuildProfile *profile);
UmiStatus umi_build_runner_run(UmiBuildRunner *runner,
                               UmiBuildPhase phase,
                               UmiBuildResult *out_result);
uint64_t umi_build_runner_next_operation_id(const UmiBuildRunner *runner);

#ifdef __cplusplus
}
#endif

#endif
