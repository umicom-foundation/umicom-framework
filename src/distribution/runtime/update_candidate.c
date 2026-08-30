/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_candidate.c
 *
 * PURPOSE:
 *   installed-versus-available update candidate classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_candidate.h"

UmiDrUpdateCandidateKind umi_dr_update_candidate_classify(UmiDrVersion i,UmiDrVersion a){int c=umi_dr_version_compare(a,i);if(c>0)return UMI_DR_UPDATE_UPGRADE;if(c<0)return UMI_DR_UPDATE_DOWNGRADE;return UMI_DR_UPDATE_REINSTALL;}
