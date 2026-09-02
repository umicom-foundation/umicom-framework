/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/package_trust.h
 *
 * PURPOSE:
 *   Publish the public package trust contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_PACKAGE_TRUST_H
#define UMICOM_SECURITY_PACKAGE_TRUST_H
#include "umicom/security/governance.h"
/**
 * Represent the security trusted signer data shared with callers of this public contract.
 */
typedef struct UmiSecurityTrustedSigner { char signer_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; char fingerprint[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; bool enabled; } UmiSecurityTrustedSigner;
/**
 * Represent the security signer store data shared with callers of this public contract.
 */
typedef struct UmiSecuritySignerStore { UmiSecurityTrustedSigner items[UMI_SECURITY_GOVERNANCE_MAX_SIGNERS]; size_t count; } UmiSecuritySignerStore;
/**
 * Represent the security package evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityPackageEvidence { char package_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; char signer_fingerprint[UMI_SECURITY_GOVERNANCE_ID_CAPACITY]; bool signature_present; bool signature_valid; bool digest_matches; } UmiSecurityPackageEvidence;
/**
 * Add security signer store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_signer_store_add(UmiSecuritySignerStore *store,const UmiSecurityTrustedSigner *signer);
/**
 * Provide the security signer store trusted operation used by this module and its client
 * applications.
 */
bool umi_security_signer_store_trusted(const UmiSecuritySignerStore *store,const char *fingerprint);
/**
 * Provide the security package trust evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_package_trust_evaluate(const UmiSecuritySignerStore *store,const UmiSecurityPackageEvidence *evidence,UmiSecurityEnvironment environment,UmiSecurityGovernanceDecision *out_decision);
#endif
