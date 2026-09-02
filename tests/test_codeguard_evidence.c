/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_evidence.c
 *
 * PURPOSE:
 *   Implement the test codeguard evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | CodeGuard evidence v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/codeguard/evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCodeGuardEvidenceStore store = {0};
    UmiCodeGuardEvidence evidence = {0};
    (void)umi_codeguard_quality_copy(evidence.id,sizeof(evidence.id),"build.windows");
    evidence.kind = UMI_CODEGUARD_EVIDENCE_BUILD; evidence.state = UMI_CODEGUARD_EVIDENCE_PASS;
    assert(umi_codeguard_evidence_add(&store,&evidence) == UMI_STATUS_OK);
    assert(umi_codeguard_evidence_add(&store,&evidence) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_codeguard_evidence_find(&store,"build.windows") != NULL);
    assert(umi_codeguard_evidence_count_state(&store,UMI_CODEGUARD_EVIDENCE_PASS) == 1U);
    assert(strcmp(umi_codeguard_evidence_kind_text(UMI_CODEGUARD_EVIDENCE_ABI),"abi-api") == 0);
    return 0;
}
