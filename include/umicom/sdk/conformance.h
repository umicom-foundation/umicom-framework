/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/conformance.h
 *
 * PURPOSE:
 *   Record conformance checks for an SDK consumer build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_CONFORMANCE_H
#define UMICOM_SDK_CONFORMANCE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk conformance data shared with callers of this public contract.
 */
typedef struct UmiSdkConformance { size_t checks_run; size_t checks_passed; size_t checks_failed; } UmiSdkConformance;
/**
 * Provide the sdk conformance record operation used by this module and its client
 * applications.
 */
/* Validate the counter invariant checks_run == checks_passed + checks_failed
 * without overflowing the addition. Empty initialized summaries are valid.
 * INVALID_ARGUMENT means null; INVALID_STATE means inconsistent counters. */
UmiStatus umi_sdk_conformance_validate(const UmiSdkConformance *state);

/* Record one caller-reported observation. Nonzero passed values retain their
 * existing true meaning. On invalid state or exhausted counters, return a
 * refusal and preserve every byte of state. Always check the returned status:
 * an unchanged prior summary is not evidence for an observation refused here.
 * Serialize access; the struct and this operation do not provide a thread lock. */
UmiStatus umi_sdk_conformance_record_checked(UmiSdkConformance *state, int passed);

/* Compatibility entry point with no status result. Refusal latches
 * checks_failed to SIZE_MAX so a dropped observation cannot leave a passing
 * verdict. Other counts remain for diagnosis; reinitialize the complete struct
 * to start a new collection. New callers should use the checked entry point. */
void umi_sdk_conformance_record(UmiSdkConformance *state,int passed);
/**
 * Provide the sdk conformance passed operation used by this module and its client
 * applications.
 */
/* True only for a consistent, nonempty summary containing no failed checks.
 * It describes supplied observations, not independent ABI/runtime proof. */
int umi_sdk_conformance_passed(const UmiSdkConformance *state);
#ifdef __cplusplus
}
#endif
#endif
