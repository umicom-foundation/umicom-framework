/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/baseline.h
 *
 * PURPOSE:
 *   Capture one expected Framework ABI baseline used by SDK and release-candidate checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_BASELINE_H
#define UMICOM_ABI_BASELINE_H
#include <stdint.h>
#include "umicom/base/version.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiBaseline { UmiVersion framework_version; uint32_t framework_abi; uint64_t contract_fingerprint; } UmiAbiBaseline;
UmiAbiBaseline umi_abi_baseline_current(uint64_t fingerprint);
int umi_abi_baseline_matches(const UmiAbiBaseline *expected, const UmiAbiBaseline *actual);
#ifdef __cplusplus
}
#endif
#endif
