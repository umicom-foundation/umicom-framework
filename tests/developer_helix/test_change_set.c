/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_change_set.c
 *
 * PURPOSE:
 *   Regression coverage for collect file changes and compute deterministic aggregate change risk and fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/change_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixChangeSet s; umi_helix_change_set_init(&s); UmiHelixFileChange c; umi_helix_file_change_init(&c,"change.1"); c.risk=0.2; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_change_set_add(&s,&c,UMI_HELIX_CHANGE_MODIFY,"src/a.c")!=UMI_STATUS_OK){ return 1; } /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s.count!=1U||s.fingerprint==0U||umi_helix_change_set_risk(&s)<=0.0){ return 2; } return 0; }
