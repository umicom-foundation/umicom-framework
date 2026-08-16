/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/result.h
 *
 * PURPOSE:
 *   Represent one complete configure, build, test, clean or run operation and its diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RESULT_H
#define UMICOM_BUILD_RESULT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/diagnostic.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildResult {
    uint64_t operation_id;
    UmiBuildPhase phase;
    UmiBuildState state;
    UmiStatus status;
    int exit_code;
    uint64_t started_ns;
    uint64_t duration_ms;
    char profile_id[UMI_BUILD_ID_CAPACITY];
    char command[UMI_BUILD_COMMAND_CAPACITY];
    char output[UMI_BUILD_OUTPUT_CAPACITY];
    UmiBuildDiagnosticList diagnostics;
} UmiBuildResult;

UmiStatus umi_build_result_create(UmiBuildResult **out_result);
void umi_build_result_destroy(UmiBuildResult *result);
void umi_build_result_init(UmiBuildResult *result,
                           uint64_t operation_id,
                           UmiBuildPhase phase,
                           const char *profile_id);
void umi_build_result_finish(UmiBuildResult *result,
                             UmiStatus status,
                             int exit_code,
                             uint64_t duration_ms);
UmiStatus umi_build_result_summary(const UmiBuildResult *result,
                                   char *out_text,
                                   size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
