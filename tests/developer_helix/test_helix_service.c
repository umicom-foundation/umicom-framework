/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_helix_service.c
 *
 * PURPOSE:
 *   Regression coverage for provide the top-level framework-owned helix control-plane state and guarded session transitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/helix_service.h"
int main(void){ UmiHelixService svc; umi_helix_service_init(&svc); UmiHelixSession *s=NULL; if(umi_helix_service_open(&svc,"s",2,&s)!=UMI_STATUS_OK||s==NULL){ return 1; } s->approved=true; s->checkpoint_ready=true; if(umi_helix_service_execution_decision(&svc,s)!=UMI_HELIX_DECISION_REVIEW){ return 2; } umi_helix_service_set_mutation(&svc,true); if(umi_helix_service_execution_decision(&svc,s)!=UMI_HELIX_DECISION_ALLOW){ return 3; } return 0; }
