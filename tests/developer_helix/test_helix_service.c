/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_helix_service.c
 *
 * PURPOSE:
 *   Regression coverage for provide the top-level framework-owned helix control-plane state and guarded session transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/helix_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixService svc; umi_helix_service_init(&svc); UmiHelixSession *s=NULL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_helix_service_open(&svc,"s",2,&s)!=UMI_STATUS_OK||s==NULL){ return 1; } s->approved=true; s->checkpoint_ready=true; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_helix_service_execution_decision(&svc,s)!=UMI_HELIX_DECISION_REVIEW){ return 2; } umi_helix_service_set_mutation(&svc,true); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_helix_service_execution_decision(&svc,s)!=UMI_HELIX_DECISION_ALLOW){ return 3; } return 0; }
