/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_quality_session.c
 *
 * PURPOSE:
 *   Verify a quality session coordinates ordinary evidence, documentation
 *   inventory, evaluation, remediation and revision changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/codeguard/quality_session.h"
#include "umicom/codeguard/quality_catalogue.h"

/* Exercise the complete reusable quality-session lifecycle. */
int main(void)
{
    UmiCodeGuardQualitySession session;
    UmiCodeGuardEvidence item = {0};
    UmiCodeGuardDocumentationReport documentation = {0};
    umi_codeguard_quality_session_init(&session);
    /* Populate one ordinary passing build record. */
    (void)umi_codeguard_quality_copy(item.id, sizeof(item.id), "build");
    item.kind = UMI_CODEGUARD_EVIDENCE_BUILD;
    item.state = UMI_CODEGUARD_EVIDENCE_PASS;
    assert(umi_codeguard_quality_session_record(
        &session, &item) == UMI_STATUS_OK);
    /* Populate one passing documentation report without retaining its path. */
    documentation.structure_size = (uint32_t)sizeof(documentation);
    documentation.function_count = 2U;
    documentation.documented_function_count = 2U;
    documentation.has_standard_file_header = true;
    documentation.passes = true;
    assert(umi_codeguard_quality_session_record_documentation(
        &session, &documentation) == UMI_STATUS_OK);
    assert(umi_codeguard_quality_session_evaluate(&session) == UMI_STATUS_OK);
    /* Three successful operations after initialisation produce revision four. */
    assert(session.evaluation.decision == UMI_CODEGUARD_QUALITY_PASS);
    assert(session.documentation.file_count == 1U);
    assert(session.revision == 4U);
    assert(umi_codeguard_quality_catalogue_count() == 10U);
    assert(strcmp(umi_codeguard_quality_catalogue_at(0U),
                  "umicom.codeguard.evidence/2") == 0);
    return 0;
}
