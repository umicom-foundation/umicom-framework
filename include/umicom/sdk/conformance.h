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
void umi_sdk_conformance_record(UmiSdkConformance *state,int passed);
/**
 * Provide the sdk conformance passed operation used by this module and its client
 * applications.
 */
int umi_sdk_conformance_passed(const UmiSdkConformance *state);
#ifdef __cplusplus
}
#endif
#endif
