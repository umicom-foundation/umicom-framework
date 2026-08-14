/* Umicom Framework | Package signature trust policy | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_PACKAGE_TRUST_H
#define UMICOM_SECURITY_PACKAGE_TRUST_H
#include "umicom/security/governance.h"
typedef struct UmiSecurityTrustedSigner { char signer_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; char fingerprint[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; bool enabled; } UmiSecurityTrustedSigner;
typedef struct UmiSecuritySignerStore { UmiSecurityTrustedSigner items[UMI_SECURITY_GOVERNANCE_MAX_SIGNERS]; size_t count; } UmiSecuritySignerStore;
typedef struct UmiSecurityPackageEvidence { char package_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; char signer_fingerprint[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; bool signature_present; bool signature_valid; bool digest_matches; } UmiSecurityPackageEvidence;
UmiStatus umi_security_signer_store_add(UmiSecuritySignerStore *store,const UmiSecurityTrustedSigner *signer);
bool umi_security_signer_store_trusted(const UmiSecuritySignerStore *store,const char *fingerprint);
UmiStatus umi_security_package_trust_evaluate(const UmiSecuritySignerStore *store,const UmiSecurityPackageEvidence *evidence,UmiSecurityEnvironment environment,UmiSecurityGovernanceDecision *out_decision);
#endif
