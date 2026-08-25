/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/capability_matrix.c
 *
 * PURPOSE:
 *   cross-frontend capability matrix used to compare GTK4, Qt6, Native Web and headless renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/capability_matrix.h"

void umi_fc_capability_matrix_init(UmiFcCapabilityMatrix *matrix) { if (matrix != NULL) *matrix=(UmiFcCapabilityMatrix){0}; }
UmiStatus umi_fc_capability_matrix_set(UmiFcCapabilityMatrix *matrix,UmiFcFrontendKind frontend,uint64_t mask){ if(matrix==NULL||frontend<UMI_FC_FRONTEND_GTK4||frontend>UMI_FC_FRONTEND_HEADLESS)return UMI_STATUS_INVALID_ARGUMENT; matrix->masks[(unsigned)frontend]=mask; return UMI_STATUS_OK; }
uint64_t umi_fc_capability_matrix_get(const UmiFcCapabilityMatrix *matrix,UmiFcFrontendKind frontend){ if(matrix==NULL||frontend<UMI_FC_FRONTEND_GTK4||frontend>UMI_FC_FRONTEND_HEADLESS)return 0U; return matrix->masks[(unsigned)frontend]; }
uint64_t umi_fc_capability_matrix_common(const UmiFcCapabilityMatrix *matrix,const UmiFcFrontendKind *frontends,size_t count){ size_t i; uint64_t mask=UINT64_MAX; if(matrix==NULL||frontends==NULL||count==0U)return 0U; for(i=0U;i<count;++i) mask &= umi_fc_capability_matrix_get(matrix,frontends[i]); return mask; }
