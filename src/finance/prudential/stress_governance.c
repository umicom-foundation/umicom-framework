/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_governance.c
 *
 * PURPOSE:
 *   Define review, approval and sign-off state for an enterprise stress run.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_governance.h"

#include <string.h>
UmiStatus umi_pru_stress_governance_evaluate(UmiPrudentialStressGovernance *record,const char *run_id,int model_validated,int independent_reviewed,int management_approved) { UmiStatus s; if(record==NULL||run_id==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(record,0,sizeof *record); s=umi_pru_copy_text(record->run_id,sizeof record->run_id,run_id); if(s!=UMI_STATUS_OK)return s; record->model_validated=model_validated?1:0; record->independent_reviewed=independent_reviewed?1:0; record->management_approved=management_approved?1:0; record->state=(record->model_validated&&record->independent_reviewed&&record->management_approved)?UMI_PRU_STRESS_READY:UMI_PRU_STRESS_DRAFT; return UMI_STATUS_OK; }
