/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_capability.h
 *
 * PURPOSE:
 *   runtime capability bitsets used to gate application launches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_CAPABILITY_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_CAPABILITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrRuntimeCapability { uint64_t required; uint64_t available; } UmiDrRuntimeCapability;
void umi_dr_runtime_capability_init(UmiDrRuntimeCapability *capability);
uint64_t umi_dr_runtime_capability_missing(const UmiDrRuntimeCapability *capability);
bool umi_dr_runtime_capability_satisfied(const UmiDrRuntimeCapability *capability);

#ifdef __cplusplus
}
#endif
#endif
