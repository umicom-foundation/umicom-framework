/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/provenance.h
 *
 * PURPOSE:
 *   build/source/toolchain provenance evidence for packaged releases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrProvenance { char id[UMI_DR_ID_CAPACITY]; char source_revision[UMI_DR_DIGEST_CAPACITY]; char toolchain[UMI_DR_TEXT_CAPACITY]; char builder[UMI_DR_TEXT_CAPACITY]; bool reproducible; } UmiDrProvenance;
void umi_dr_provenance_init(UmiDrProvenance *value);
bool umi_dr_provenance_valid(const UmiDrProvenance *value);
uint64_t umi_dr_provenance_fingerprint(const UmiDrProvenance *value);

#ifdef __cplusplus
}
#endif
#endif
