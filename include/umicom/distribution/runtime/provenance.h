/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/provenance.h
 *
 * PURPOSE:
 *   build/source/toolchain provenance evidence for packaged releases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PROVENANCE_H
#define UMICOM_DISTRIBUTION_RUNTIME_PROVENANCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr provenance data shared with callers of this public contract.
 */
typedef struct UmiDrProvenance { char id[UMI_DR_ID_CAPACITY]; char source_revision[UMI_DR_DIGEST_CAPACITY]; char toolchain[UMI_DR_TEXT_CAPACITY]; char builder[UMI_DR_TEXT_CAPACITY]; bool reproducible; } UmiDrProvenance;
/**
 * Initialise dr provenance from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_provenance_init(UmiDrProvenance *value);
/**
 * Check that dr provenance satisfies its contract before another service relies on it.
 */
bool umi_dr_provenance_valid(const UmiDrProvenance *value);
/**
 * Provide the dr provenance fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_provenance_fingerprint(const UmiDrProvenance *value);

#ifdef __cplusplus
}
#endif
#endif
