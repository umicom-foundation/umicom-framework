/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/package_trust.c
 *
 * PURPOSE:
 *   Implement the package trust behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Package signature trust policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/package_trust.h"
#include <stdio.h>
#include <string.h>
/*
 * Add security signer store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_signer_store_add(UmiSecuritySignerStore *store,const UmiSecurityTrustedSigner *signer)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || signer == NULL || signer->signer_id[0] == '\0' || signer->fingerprint[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(store->items[index].fingerprint,signer->fingerprint) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_SECURITY_GOVERNANCE_MAX_SIGNERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count++] = *signer;
    return UMI_STATUS_OK;
}
/*
 * Provide the security signer store trusted operation used by this module and its client
 * applications.
 */
bool umi_security_signer_store_trusted(const UmiSecuritySignerStore *store,const char *fingerprint)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || fingerprint == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (store->items[index].enabled && strcmp(store->items[index].fingerprint,fingerprint) == 0) return true;
    return false;
}
/*
 * Provide the security package trust evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_package_trust_evaluate(const UmiSecuritySignerStore *store,const UmiSecurityPackageEvidence *evidence,UmiSecurityEnvironment environment,UmiSecurityGovernanceDecision *out_decision)
{
    bool trusted;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || evidence == NULL || out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    trusted = evidence->signature_present && evidence->signature_valid && evidence->digest_matches && umi_security_signer_store_trusted(store,evidence->signer_fingerprint);
    out_decision->approval_required = false;
    /* Apply this operation only while the related capability or state is available. */
    if (trusted) { out_decision->allowed = true; (void)snprintf(out_decision->reason,sizeof(out_decision->reason),"Package signature, digest and signer trust verified"); }
    else /* Apply this operation only while the related capability or state is available. */ if (environment == UMI_SECURITY_ENV_DEVELOPMENT) { out_decision->allowed = true; out_decision->approval_required = true; (void)snprintf(out_decision->reason,sizeof(out_decision->reason),"Untrusted development package requires explicit approval"); }
    /* Use this fallback path when the earlier condition does not apply. */
    else { out_decision->allowed = false; (void)snprintf(out_decision->reason,sizeof(out_decision->reason),"Untrusted package denied outside development"); }
    return UMI_STATUS_OK;
}
