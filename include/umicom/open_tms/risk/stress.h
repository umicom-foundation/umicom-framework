/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/risk/stress.h
 *
 * PURPOSE:
 *   Evaluate Treasury Stress Score through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RISK_STRESS_H
#define UMICOM_OPEN_TMS_RISK_STRESS_H

#include <stddef.h>
#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_risk_stress(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);


/** Evaluate a complete group using this policy without publishing partial rows.
 * Zero count is a no-op and permits NULL arrays. Nonempty groups require arrays
 * with count readable inputs and capacity writable outputs. At most 4096 rows
 * and 16 MiB of temporary output are accepted; allocation can fail.
 * Any failure preserves all outputs. Non-finite signals and overlong labels
 * are refused. Inputs may borrow output storage until publication. The caller
 * serializes access; this calculation does not submit orders or execute jobs.
 */
UmiStatus umi_open_tms_evaluate_risk_stress_batch(const UmiOpenTmsInput *inputs,
    size_t count, UmiOpenTmsSnapshot *outputs, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
