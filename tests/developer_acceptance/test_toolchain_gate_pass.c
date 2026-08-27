/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_toolchain_gate_pass.c
 *
 * PURPOSE:
 *   Verify developer acceptance toolchain gate pass behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/acceptance/toolchain_gate.h"
int test_developer_acceptance_toolchain_gate_pass(void)
{
    UmiDeveloperLanguageSupportSnapshot support={0};
    UmiDeveloperAcceptanceEvidenceStore store;
    const UmiDeveloperAcceptanceEvidence *evidence;
    strcpy(support.language_id,"c");
    strcpy(support.binding_id,"c.clang");
    support.binding_found=1;
    support.supported_operation_count=7U;
    support.ready_operation_count=7U;
    support.readiness_percent=100U;
    umi_developer_acceptance_evidence_store_init(&store);
    assert(umi_developer_acceptance_record_toolchain_support(&support,&store)==UMI_STATUS_OK);
    evidence=umi_developer_acceptance_evidence_find(&store,"umicom.developer.language.c");
    assert(evidence!=NULL&&evidence->state==UMI_DEVELOPER_ACCEPTANCE_PASS);
    return 0;
}
