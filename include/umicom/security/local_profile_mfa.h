/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/local_profile_mfa.h
 *
 * PURPOSE:
 *   Require local password verification and recorded authenticator proof
 *   before issuing a session for the same local profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_LOCAL_PROFILE_MFA_H
#define UMICOM_SECURITY_LOCAL_PROFILE_MFA_H

#include "umicom/security/local_profile.h"
#include "umicom/security/session.h"
#include "umicom/security/totp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLocalMfaCredential {
    uint64_t credentialRevision;
    UmiTotpAlgorithm algorithm;
    unsigned digits;
    size_t secretBytes;
    uint8_t secret[UMI_TOTP_MAX_SECRET_BYTES];
    UmiTotpReplay replay;
} UmiLocalMfaCredential;

/** The backend belongs to the same application scope as the password and
 * session stores. read returns the active credential and its durable replay
 * position for the canonical profile. Keep secret bytes in a secure store.
 * commitReplay must atomically compare the active credential revision and
 * previous replay position, then make the replacement durable. Return BUSY
 * if either changed. A credential revision changes whenever it is replaced
 * or revoked. Borrowed inputs must not be retained by callbacks.
 *
 * A NULL mac selects UmiTotpPlatformMac. A custom callback uses an approved
 * cryptographic provider and has the same contract as UmiTotpMac.
 */
typedef struct UmiLocalMfaBackend {
    void *context;
    UmiStatus (*read)(void *context, const char *profile, UmiLocalMfaCredential *out);
    UmiStatus (*commitReplay)(void *context, const char *profile, uint64_t credentialRevision,
        UmiTotpReplay previous, UmiTotpReplay replacement);
    UmiTotpMac mac;
    void *macContext;
} UmiLocalMfaBackend;

/** Verify both factors for one canonical profile and issue the existing
 * Framework session. nowNs is trusted Unix time in nanoseconds. Session
 * expiry uses the same clock. Keep this operation under the password store's
 * serialization contract and apply the application's login rate limits.
 *
 * A refusal leaves outSession unchanged. Missing credentials and invalid
 * codes are denied. Once a replay position is committed, its code is consumed
 * even if session issuance fails. No password, secret or code is returned.
 */
UmiStatus UmiLocalProfileMfaSignIn(UmiLocalProfileStore *profiles, UmiSessionRegistry *sessions,
    const UmiLocalMfaBackend *backend, const char *profile, const char *password,
    const char *code, size_t codeBytes, uint64_t nowNs, uint64_t lifetimeNs,
    UmiSessionRecord *outSession);

typedef struct UmiLocalMfaPendingEnrollment {
    uint64_t enrollmentRevision;
    uint64_t replacesCredentialRevision;
    uint64_t expiresAtNs;
    UmiLocalMfaCredential credential;
} UmiLocalMfaPendingEnrollment;

/** Read an inactive credential from the same application's secure backend.
 * Assign a unique enrollment revision to an immutable pending record.
 * replacesCredentialRevision identifies the active credential observed
 * when preparing enrollment, or zero for an initial enrollment.
 *
 * promote must atomically verify that the same pending and active revisions
 * still apply, replace the active credential including its verified replay
 * step, and remove the pending record in one durable operation. Use BUSY if
 * either record changed. Serialize profile credential access, including
 * enrollment, rotation and revocation, across every writer of this namespace.
 * Borrowed buffers, including the candidate key, must not leave the call.
 */
typedef struct UmiLocalMfaEnrollmentBackend {
    void *context;
    UmiStatus (*readPending)(void *context, const char *profile, UmiLocalMfaPendingEnrollment *out);
    UmiStatus (*promote)(void *context, const char *profile, uint64_t enrollmentRevision,
        uint64_t replacesCredentialRevision, const UmiLocalMfaCredential *credential);
    UmiTotpMac mac;
    void *macContext;
} UmiLocalMfaEnrollmentBackend;

/** Verify the local password and a code from the intended pending credential.
 * The trusted host provides time and the revision shown during enrollment.
 * Expired or missing enrollments and invalid codes cannot activate. The
 * confirmation consumes its accepted time step and never issues a session.
 * The backend owns throttling, key generation, secure storage and revocation.
 */
UmiStatus UmiLocalProfileMfaConfirmEnrollment(UmiLocalProfileStore *profiles,
    const UmiLocalMfaEnrollmentBackend *backend, const char *profile, const char *password,
    uint64_t enrollmentRevision, const char *code, size_t codeBytes, uint64_t nowNs);

#ifdef __cplusplus
}
#endif
#endif
