/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/update_candidate.h
 *
 * PURPOSE:
 *   installed-versus-available update candidate classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_UPDATE_CANDIDATE_H
#define UMICOM_DISTRIBUTION_RUNTIME_UPDATE_CANDIDATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDrUpdateCandidateKind { UMI_DR_UPDATE_NONE=0, UMI_DR_UPDATE_UPGRADE=1, UMI_DR_UPDATE_DOWNGRADE=2, UMI_DR_UPDATE_REINSTALL=3 } UmiDrUpdateCandidateKind;
UmiDrUpdateCandidateKind umi_dr_update_candidate_classify(UmiDrVersion installed,UmiDrVersion available);

#ifdef __cplusplus
}
#endif
#endif
