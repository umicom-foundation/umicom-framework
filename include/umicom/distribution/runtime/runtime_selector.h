/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_selector.h
 *
 * PURPOSE:
 *   best-compatible runtime profile selection using explicit requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_SELECTOR_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_SELECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/distribution/runtime/runtime_profile.h"
#include "umicom/distribution/runtime/runtime_probe.h"
UmiStatus umi_dr_runtime_selector_choose(const UmiDrRuntimeProfile *profiles, size_t count, const UmiDrRuntimeProbe *probe, size_t *out_index);

#ifdef __cplusplus
}
#endif
#endif
