/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_probe.h
 *
 * PURPOSE:
 *   deterministic probe snapshots describing detected host runtime properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_PROBE_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_PROBE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrRuntimeProbe { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrArchitecture architecture; UmiDrVersion version; uint64_t capabilities; uint64_t memory_mb; } UmiDrRuntimeProbe;
void umi_dr_runtime_probe_init(UmiDrRuntimeProbe *value);
bool umi_dr_runtime_probe_valid(const UmiDrRuntimeProbe *value);
uint64_t umi_dr_runtime_probe_fingerprint(const UmiDrRuntimeProbe *value);

#ifdef __cplusplus
}
#endif
#endif
