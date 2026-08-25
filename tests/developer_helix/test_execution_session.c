/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_execution_session.c
 *
 * PURPOSE:
 *   Regression coverage for track the running execution cursor, completed steps, failures and cancellation state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/execution_session.h"
int main(void){ UmiHelixExecutionSession s; if(umi_helix_execution_session_start(&s,2)!=UMI_STATUS_OK){ return 1; } if(umi_helix_execution_session_complete_step(&s,true)!=UMI_STATUS_OK||s.completed!=1U){ return 2; } if(umi_helix_execution_session_complete_step(&s,true)!=UMI_STATUS_OK||s.state!=UMI_HELIX_SUCCEEDED){ return 3; } return 0; }
