/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_types.c
 *
 * PURPOSE:
 *   Regression coverage for shared Helix types, scoring and fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    char id[UMI_HELIX_ID_CAPACITY]; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_copy_text(id,sizeof(id),"helix.session")!=UMI_STATUS_OK){ return 1; }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_helix_valid_id(id) || strcmp(umi_helix_state_text(UMI_HELIX_RUNNING),"running")!=0){ return 2; }
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_helix_clamp_score(2.0)!=1.0 || umi_helix_safe_ratio(2.0,4.0)!=0.5){ return 3; }
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_helix_hash_text("baseline")==0U || umi_helix_severity_from_risk(0.9)!=UMI_HELIX_CRITICAL){ return 4; } return 0;
}
