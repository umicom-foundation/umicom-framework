/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_profile_mfa.c
 *
 * PURPOSE:
 *   Compose the shared local profile, authenticator and session owners so
 *   only durable proof of both factors grants a new session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/local_profile_mfa.h"
#include "umicom/security/secrets.h"
#include "local_profile_proof_internal.h"

static bool ValidCredential(const UmiLocalMfaCredential *credential)
{
    return credential->credentialRevision != 0U &&
        (credential->algorithm == UMI_TOTP_SHA1 || credential->algorithm == UMI_TOTP_SHA256) &&
        (credential->digits == 6U || credential->digits == 8U) &&
        credential->secretBytes >= UMI_TOTP_MIN_SECRET_BYTES &&
        credential->secretBytes <= UMI_TOTP_MAX_SECRET_BYTES &&
        (credential->replay.has_last_step || credential->replay.last_step == 0U);
}

/* Confirmation, sign-in and recovery issuance use the same MAC selection,
 * time conversion and refusal rules. The security owner keeps the proof internal. */
UmiStatus UmiLocalMfaVerifyCredentialCode(const UmiLocalMfaCredential *credential, UmiTotpMac mac,
    void *context, const char *code, size_t codeBytes, uint64_t nowNs, UmiTotpReplay *replay)
{
    const UmiTotpProfile profile = {.mac = mac != NULL ? mac : UmiTotpPlatformMac,
        .mac_context = context, .algorithm = credential->algorithm, .digits = credential->digits};
    const UmiStatus status = UmiTotpVerify(&profile, credential->secret, credential->secretBytes,
        code, codeBytes, nowNs / UINT64_C(1000000000), replay);
    return status == UMI_STATUS_INVALID_ARGUMENT ? UMI_STATUS_PERMISSION_DENIED : status;
}

UmiStatus UmiLocalProfileMfaSignIn(UmiLocalProfileStore *profiles, UmiSessionRegistry *sessions,
    const UmiLocalMfaBackend *backend, const char *profile, const char *password,
    const char *code, size_t codeBytes, uint64_t nowNs, uint64_t lifetimeNs,
    UmiSessionRecord *outSession)
{
    if (profiles == NULL || sessions == NULL || backend == NULL || backend->read == NULL ||
        backend->commitReplay == NULL || password == NULL || code == NULL || outSession == NULL ||
        lifetimeNs == 0U || lifetimeNs > UINT64_MAX - nowNs)
        return UMI_STATUS_INVALID_ARGUMENT;

    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status != UMI_STATUS_OK) return status;
    status = UmiLocalProfileVerify(profiles, canonical, password);
    if (status != UMI_STATUS_OK) return status;

    /* The first factor gates access to the authenticator secret. Capture the
     * revision and replay position for a later compare-and-commit operation. */
    UmiLocalMfaCredential credential = {0};
    status = backend->read(backend->context, canonical, &credential);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && !ValidCredential(&credential)) status = UMI_STATUS_PARSE_ERROR;
    const uint64_t revision = credential.credentialRevision;
    const UmiTotpReplay previous = credential.replay;
    UmiTotpReplay replacement = previous;
    /* The shared verifier above gives enrolment and sign-in the same code
     * rules. Retain the earlier inline call for review. */
#if 0
    if (status == UMI_STATUS_OK) {
        const UmiTotpProfile verifier = {.mac = backend->mac != NULL ? backend->mac : UmiTotpPlatformMac,
            .mac_context = backend->macContext, .algorithm = credential.algorithm, .digits = credential.digits};
        status = UmiTotpVerify(&verifier, credential.secret, credential.secretBytes, code, codeBytes,
            nowNs / UINT64_C(1000000000), &replacement);
        if (status == UMI_STATUS_INVALID_ARGUMENT) status = UMI_STATUS_PERMISSION_DENIED;
    }
#endif
    if (status == UMI_STATUS_OK)
        status = UmiLocalMfaVerifyCredentialCode(&credential, backend->mac, backend->macContext,
                                                 code, codeBytes, nowNs, &replacement);
    umi_secret_clear(&credential, sizeof credential);
    if (status != UMI_STATUS_OK) return status;

    /* Issue only after the backend has durably accepted this exact step for
     * the same active credential. Never roll back consumption after failure. */
    status = backend->commitReplay(backend->context, canonical, revision, previous, replacement);
    if (status != UMI_STATUS_OK) return status;
    return umi_session_registry_issue(sessions, canonical, nowNs, lifetimeNs, outSession);
}

UmiStatus UmiLocalProfileMfaConfirmEnrollment(UmiLocalProfileStore *profiles,
    const UmiLocalMfaEnrollmentBackend *backend, const char *profile, const char *password,
    uint64_t enrollmentRevision, const char *code, size_t codeBytes, uint64_t nowNs)
{
    if (profiles == NULL || backend == NULL || backend->readPending == NULL ||
        backend->promote == NULL || password == NULL || code == NULL || enrollmentRevision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status != UMI_STATUS_OK) return status;
    status = UmiLocalProfileVerify(profiles, canonical, password);
    if (status != UMI_STATUS_OK) return status;

    /* An inactive key can't grant a session. A successful confirmation
     * publishes its first replay position and active identity together. */
    UmiLocalMfaPendingEnrollment pending = {0};
    status = backend->readPending(backend->context, canonical, &pending);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK &&
        (pending.enrollmentRevision == 0U || pending.expiresAtNs == 0U ||
         !ValidCredential(&pending.credential) || pending.credential.replay.has_last_step ||
         pending.credential.credentialRevision <= pending.replacesCredentialRevision))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && pending.enrollmentRevision != enrollmentRevision)
        status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && nowNs >= pending.expiresAtNs)
        status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK)
        status = UmiLocalMfaVerifyCredentialCode(&pending.credential, backend->mac, backend->macContext,
            code, codeBytes, nowNs, &pending.credential.replay);
    if (status == UMI_STATUS_OK)
        status = backend->promote(backend->context, canonical, pending.enrollmentRevision,
            pending.replacesCredentialRevision, &pending.credential);
    umi_secret_clear(&pending, sizeof pending);
    return status;
}
