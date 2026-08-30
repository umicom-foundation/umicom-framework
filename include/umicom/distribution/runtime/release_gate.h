/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/release_gate.h
 *
 * PURPOSE:
 *   release decision combining signature, checksum, compatibility and test evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RELEASE_GATE_H
#define UMICOM_DISTRIBUTION_RUNTIME_RELEASE_GATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrReleaseGateInput { bool signatures_ok; bool checksums_ok; bool compatibility_ok; bool tests_ok; bool frontend_conformance_ok; size_t blockers; } UmiDrReleaseGateInput;
bool umi_dr_release_gate_pass(const UmiDrReleaseGateInput *input);

#ifdef __cplusplus
}
#endif
#endif
