/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_diagnostic_signal.c
 *
 * PURPOSE:
 *   Regression coverage for represent a compiler, test, runtime or static-analysis signal suitable for failure reasoning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/diagnostic_signal.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixDiagnosticSignal r; umi_helix_diagnostic_signal_init(&r,"diagnostic_signal"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_diagnostic_signal_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_diagnostic_signal_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
