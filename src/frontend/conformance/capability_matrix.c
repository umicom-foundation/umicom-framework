/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/capability_matrix.c
 *
 * PURPOSE:
 *   cross-frontend capability matrix used to compare GTK4, Qt6, Native Web and headless renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/capability_matrix.h"

/*
 * Initialise fc capability matrix from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_capability_matrix_init(UmiFcCapabilityMatrix *matrix) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (matrix != NULL) *matrix=(UmiFcCapabilityMatrix){0}; }
/*
 * Copy fc capability matrix into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_fc_capability_matrix_set(UmiFcCapabilityMatrix *matrix,UmiFcFrontendKind frontend,uint64_t mask){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||frontend<UMI_FC_FRONTEND_GTK4||frontend>UMI_FC_FRONTEND_HEADLESS)return UMI_STATUS_INVALID_ARGUMENT; matrix->masks[(unsigned)frontend]=mask; return UMI_STATUS_OK; }
/*
 * Provide the fc capability matrix get operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_capability_matrix_get(const UmiFcCapabilityMatrix *matrix,UmiFcFrontendKind frontend){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||frontend<UMI_FC_FRONTEND_GTK4||frontend>UMI_FC_FRONTEND_HEADLESS)return 0U; return matrix->masks[(unsigned)frontend]; }
/*
 * Provide the fc capability matrix common operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_capability_matrix_common(const UmiFcCapabilityMatrix *matrix,const UmiFcFrontendKind *frontends,size_t count){ size_t i; uint64_t mask=UINT64_MAX; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||frontends==NULL||count==0U)return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i) mask &= umi_fc_capability_matrix_get(matrix,frontends[i]); return mask; }
