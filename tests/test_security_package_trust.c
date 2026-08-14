/* Umicom Framework Tests | Package trust v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include "umicom/security/package_trust.h"
int main(void)
{
    UmiSecuritySignerStore store = {0};
    UmiSecurityTrustedSigner signer = {0};
    UmiSecurityPackageEvidence evidence = {0};
    UmiSecurityGovernanceDecision decision;
    (void)snprintf(signer.signer_id,sizeof(signer.signer_id),"umicom-release"); (void)snprintf(signer.fingerprint,sizeof(signer.fingerprint),"sha256:trusted"); signer.enabled = true;
    assert(umi_security_signer_store_add(&store,&signer) == UMI_STATUS_OK);
    (void)snprintf(evidence.package_id,sizeof(evidence.package_id),"plugin.chart"); (void)snprintf(evidence.signer_fingerprint,sizeof(evidence.signer_fingerprint),"sha256:trusted"); evidence.signature_present = true; evidence.signature_valid = true; evidence.digest_matches = true;
    assert(umi_security_package_trust_evaluate(&store,&evidence,UMI_SECURITY_ENV_PRODUCTION,&decision) == UMI_STATUS_OK && decision.allowed);
    evidence.digest_matches = false;
    assert(umi_security_package_trust_evaluate(&store,&evidence,UMI_SECURITY_ENV_PRODUCTION,&decision) == UMI_STATUS_OK && !decision.allowed);
    return 0;
}
