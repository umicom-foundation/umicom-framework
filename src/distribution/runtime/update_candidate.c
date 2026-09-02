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

/*
 * Provide the dr update candidate classify operation used by this module and its client
 * applications.
 */
UmiDrUpdateCandidateKind umi_dr_update_candidate_classify(UmiDrVersion i,UmiDrVersion a){int c=umi_dr_version_compare(a,i);/* Apply this branch only when its contract condition is satisfied. */ if(c>0)return UMI_DR_UPDATE_UPGRADE;/* Apply this branch only when its contract condition is satisfied. */ if(c<0)return UMI_DR_UPDATE_DOWNGRADE;return UMI_DR_UPDATE_REINSTALL;}
