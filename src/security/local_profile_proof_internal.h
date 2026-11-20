/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_profile_proof_internal.h
 *
 * PURPOSE:
 *   Share password account binding and authenticator proof inside security owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_LOCAL_PROFILE_PROOF_INTERNAL_H
#define UMICOM_SECURITY_LOCAL_PROFILE_PROOF_INTERNAL_H

#include "umicom/security/local_profile_mfa.h"

#define UMI_LOCAL_PROFILE_BINDING_BYTES 32U

/* Return the persistent registration salt only after password verification.
 * It binds an enrollment to a password record and must remain inside the
 * security service. Existing profile records and backends already own it. */
UmiStatus UmiLocalProfileVerifyBound(UmiLocalProfileStore *store, const char *name,
    const char *password, unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES]);

UmiStatus UmiLocalMfaVerifyCredentialCode(const UmiLocalMfaCredential *credential,
    UmiTotpMac mac, void *context, const char *code, size_t codeBytes,
    uint64_t nowNs, UmiTotpReplay *replay);

#endif
