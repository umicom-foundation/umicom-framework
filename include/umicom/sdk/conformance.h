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

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_CONFORMANCE_H
#define UMICOM_SDK_CONFORMANCE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkConformance { size_t checks_run; size_t checks_passed; size_t checks_failed; } UmiSdkConformance;
void umi_sdk_conformance_record(UmiSdkConformance *state,int passed);
int umi_sdk_conformance_passed(const UmiSdkConformance *state);
#ifdef __cplusplus
}
#endif
#endif
