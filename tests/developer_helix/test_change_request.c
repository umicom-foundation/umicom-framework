/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_change_request.c
 *
 * PURPOSE:
 *   Regression coverage for describe one proposed repository change and the evidence needed before it can be applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/change_request.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixChangeRequest r; umi_helix_change_request_init(&r,"change_request"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_change_request_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_change_request_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
