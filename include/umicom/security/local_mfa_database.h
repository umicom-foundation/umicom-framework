/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/local_mfa_database.h
 *
 * PURPOSE:
 *   Persist local authenticator enrollment and replay state with OS-protected keys.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_LOCAL_MFA_DATABASE_H
#define UMICOM_SECURITY_LOCAL_MFA_DATABASE_H

#include "umicom/security/local_profile_mfa.h"
#include "umicom/security/totp_enrollment.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLocalMfaDatabase UmiLocalMfaDatabase;

typedef struct UmiLocalMfaPreparedEnrollment {
    uint64_t revision;
    size_t uriLength;
    char uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
} UmiLocalMfaPreparedEnrollment;

/* Open an explicit ordinary absolute path in a directory private to the
 * application's OS user. Windows with SQLite uses user-scoped DPAPI. Hosts
 * without this provider return UNAVAILABLE. Failure clears *out.
 * SQLite stores encrypted seed records and clear revision and replay metadata.
 * Callers serialize each handle and keep its application namespace consistent
 * with the password and session owners. Keep I/O off the UI thread. */
UmiStatus UmiLocalMfaDatabaseOpen(const char *path, const char *application,
    UmiLocalMfaDatabase **out);
void UmiLocalMfaDatabaseClose(UmiLocalMfaDatabase *database);

/* Borrow a database from both compatibility callback tables. Keep the handle
 * open while the callbacks are used with the shared password-first MFA service. Each replay
 * commit and pending promotion is conditional on the revision read earlier.
 * Borrowed callbacks are intended for a trusted application process. */
UmiStatus UmiLocalMfaDatabaseBackends(UmiLocalMfaDatabase *database,
    UmiLocalMfaBackend *signIn, UmiLocalMfaEnrollmentBackend *enrollment);

/* Verify the current local password, generate a fresh native random seed and
 * stage initial enrollment. Absolute Unix nanoseconds require
 * nowNs < expiresAtNs <= INT64_MAX. Later preparations supersede older
 * pending records by revision. An active authenticator refuses preparation.
 *
 * The returned URI grants access to the seed. Display it only during the
 * admitted enrollment and clear it afterwards with umi_secret_clear.
 * Confirmation uses UmiLocalProfileMfaConfirmEnrollment. An unconfirmed
 * record never qualifies for sign-in. Failure preserves *out. */
UmiStatus UmiLocalMfaDatabasePrepare(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, const char *profile, const char *password,
    const char *issuer, UmiTotpAlgorithm algorithm, unsigned digits,
    uint64_t nowNs, uint64_t expiresAtNs, UmiLocalMfaPreparedEnrollment *out);

/* Confirm the exact pending enrollment for the verified password registration.
 * The native database rechecks the binding while holding the same writer lock
 * used for activation. Confirmation consumes its accepted step without issuing
 * a session. Use the application's trusted clock, rate limits and serialization
 * policy for all account and authenticator writers. */
UmiStatus UmiLocalMfaDatabaseConfirmEnrollment(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, const char *profile, const char *password,
    uint64_t enrollmentRevision, const char *code, size_t codeBytes, uint64_t nowNs);

/* Require the current password registration and a fresh authenticator code.
 * Issue a session only after replay state and ownership are checked durably.
 * A refusal preserves *out. An unbound historical enrollment uses the explicit
 * compatibility callbacks until its account ownership has been migrated. */
UmiStatus UmiLocalMfaDatabaseSignIn(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, UmiSessionRegistry *sessions, const char *profile,
    const char *password, const char *code, size_t codeBytes, uint64_t nowNs,
    uint64_t lifetimeNs, UmiSessionRecord *out);

#define UMI_LOCAL_MFA_RECOVERY_COUNT 8U
#define UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH 32U

typedef struct UmiLocalMfaRecoveryCodes {
    char codes[UMI_LOCAL_MFA_RECOVERY_COUNT][UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH + 1U];
} UmiLocalMfaRecoveryCodes;

/* Verify the password and a fresh current authenticator code for an active
 * enrollment tied to the same password registration. Consume the time step
 * and replace the previous recovery set in one transaction. Legacy accounts
 * without an enrollment binding cannot issue recovery codes through this API.
 * Show the generated codes once and clear the caller's copy after display.
 * Persistence holds only scoped digests. Failure leaves *out unchanged. */
UmiStatus UmiLocalMfaDatabaseIssueRecovery(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, const char *profile, const char *password,
    const char *authenticatorCode, size_t codeBytes, uint64_t nowNs,
    UmiLocalMfaRecoveryCodes *out);

/* Verify the current password and consume one exact lowercase hexadecimal
 * recovery code for this active enrollment before issuing a Framework session.
 * Follow the same clock, lock and rate-limit policy as authenticator sign-in.
 * A code stays consumed if session issuance fails. A refusal leaves *out unchanged. */
UmiStatus UmiLocalMfaDatabaseRecoverySignIn(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, UmiSessionRegistry *sessions, const char *profile,
    const char *password, const char *recoveryCode, size_t codeBytes, uint64_t nowNs,
    uint64_t lifetimeNs, UmiSessionRecord *out);

#ifdef __cplusplus
}
#endif
#endif
