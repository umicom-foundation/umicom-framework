/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_quality_session.c
 *
 * PURPOSE:
 *   Implement the test codeguard quality session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Quality session v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/codeguard/quality_session.h"
#include "umicom/codeguard/quality_catalogue.h"
int main(void)
{
    UmiCodeGuardQualitySession session;
    UmiCodeGuardEvidence item = {0};
    umi_codeguard_quality_session_init(&session);
    (void)umi_codeguard_quality_copy(item.id,sizeof(item.id),"build"); item.kind = UMI_CODEGUARD_EVIDENCE_BUILD; item.state = UMI_CODEGUARD_EVIDENCE_PASS;
    assert(umi_codeguard_quality_session_record(&session,&item) == UMI_STATUS_OK);
    assert(umi_codeguard_quality_session_evaluate(&session) == UMI_STATUS_OK);
    assert(session.evaluation.decision == UMI_CODEGUARD_QUALITY_PASS && session.revision == 3U);
    assert(umi_codeguard_quality_catalogue_count() == 9U);
    assert(strcmp(umi_codeguard_quality_catalogue_at(0U),"umicom.codeguard.evidence/2") == 0);
    return 0;
}
