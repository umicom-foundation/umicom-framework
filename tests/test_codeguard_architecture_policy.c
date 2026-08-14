/* Umicom Framework Tests | Architecture policy v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/codeguard/architecture_policy.h"
int main(void)
{
    UmiCodeGuardArchitecturePolicy policy;
    UmiCodeGuardEvidenceStore evidence = {0};
    umi_codeguard_architecture_policy_default(&policy);
    assert(umi_codeguard_architecture_classify("applications/studio/src/app.c") == UMI_CODEGUARD_LAYER_STUDIO);
    assert(umi_codeguard_architecture_is_private_header("framework/src/private.h"));
    assert(umi_codeguard_architecture_check_dependency(&policy,"applications/studio/src/app.c","framework/src/internal.h",&evidence) == UMI_STATUS_OK);
    assert(evidence.count == 1U && evidence.items[0].state == UMI_CODEGUARD_EVIDENCE_FAIL);
    assert(umi_codeguard_architecture_check_dependency(&policy,"applications/studio/src/app.c","framework/include/umicom/base.h",&evidence) == UMI_STATUS_OK);
    return 0;
}
