/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_repair_strategy.c
 *
 * PURPOSE:
 *   Regression coverage for describe a bounded repair approach and the failure classes for which it may be considered.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/repair_strategy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixRepairStrategy r; umi_helix_repair_strategy_init(&r,"repair_strategy"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_repair_strategy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_repair_strategy_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
