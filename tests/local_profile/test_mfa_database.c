/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_profile/test_mfa_database.c
 *
 * PURPOSE:
 *   Qualify encrypted enrollment, replay persistence and app/profile separation.
 *   Qualify recovery issuance, durable consumption and password registration binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/native_arguments.h"
#include "umicom/platform/threading.h"
#include "umicom/security/local_mfa_database.h"
#include "umicom/security/local_profile_database.h"
#include "umicom/security/secrets.h"
#include <sqlite3.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

static const char fixturePassword[] = "fictional local profile fixture password";
static const uint64_t startTime = UINT64_C(1700000010000000000);
static const uint64_t stepNs = UINT64_C(30000000000);

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "MFA database fixture, line %d: %s\n", __LINE__, #expression); \
    goto finish; } } while (0)

/* Emulate an authenticator holding a synthetic enrollment key. The actual
 * native MAC computes the code and neither the code nor key reaches the log. */
static bool AuthenticatorCode(const UmiLocalMfaCredential *key, uint64_t timeNs, char out[9])
{
    uint64_t step = timeNs / stepNs;
    uint8_t counter[UMI_TOTP_COUNTER_BYTES] = {0}, digest[UMI_TOTP_MAX_DIGEST_BYTES] = {0};
    for (size_t i = sizeof counter; i > 0U; --i) {
        counter[i - 1U] = (uint8_t)(step & 255U);
        step >>= 8U;
    }
    UmiStatus status = UmiTotpPlatformMac(NULL, key->algorithm, key->secret, key->secretBytes, counter, digest);
    int printed = 0;
    if (status == UMI_STATUS_OK) {
        const size_t digestSize = key->algorithm == UMI_TOTP_SHA1 ? 20U : 32U;
        const size_t offset = digest[digestSize - 1U] & 15U;
        const uint32_t truncated = ((uint32_t)(digest[offset] & 127U) << 24U) |
            ((uint32_t)digest[offset + 1U] << 16U) | ((uint32_t)digest[offset + 2U] << 8U) |
            (uint32_t)digest[offset + 3U];
        const uint32_t mod = key->digits == 6U ? 1000000U : 100000000U;
        printed = snprintf(out, 9U, "%0*u", (int)key->digits, (unsigned)(truncated % mod));
    }
    umi_secret_clear(digest, sizeof digest);
    return status == UMI_STATUS_OK && printed == (int)key->digits;
}

static bool HasPlaintext(const unsigned char *blob, size_t length, const UmiLocalMfaCredential *secret)
{
    if (length < secret->secretBytes) return false;
    for (size_t offset = 0U; offset <= length - secret->secretBytes; ++offset)
        if (memcmp(blob + offset, secret->secret, secret->secretBytes) == 0) return true;
    return false;
}

static bool CheckProtectedRow(sqlite3 *db, const UmiLocalMfaCredential *credential, bool active)
{
    sqlite3_stmt *statement = NULL;
    bool matches = false;
    const char *sql = active ? "SELECT active_seed, active, replay, pending FROM local_mfa_v1"
                            : "SELECT pending_seed, pending, expires, active FROM local_mfa_v1";
    if (sqlite3_prepare_v2(db, sql, -1, &statement, NULL) == SQLITE_OK &&
        sqlite3_step(statement) == SQLITE_ROW && sqlite3_column_type(statement, 0) == SQLITE_BLOB) {
        const unsigned char *blob = sqlite3_column_blob(statement, 0);
        const size_t length = (size_t)sqlite3_column_bytes(statement, 0);
        const sqlite3_int64 revision = sqlite3_column_int64(statement, 1);
        const sqlite3_int64 stamp = sqlite3_column_int64(statement, 2);
        matches = blob != NULL && length > credential->secretBytes && !HasPlaintext(blob, length, credential) &&
            revision == (sqlite3_int64)credential->credentialRevision &&
            stamp == (sqlite3_int64)(active ? credential->replay.last_step : startTime + stepNs) &&
            sqlite3_column_int64(statement, 3) == 0 && sqlite3_step(statement) == SQLITE_DONE;
    }
    (void)sqlite3_finalize(statement);
    return matches;
}

static int Lifecycle(const char *path, const char *application)
{
    UmiLocalProfileStore *profiles = NULL;
    UmiLocalMfaDatabase *first = NULL, *second = NULL;
    sqlite3 *external = NULL;
    UmiSessionRegistry *sessions = NULL;
    UmiLocalMfaPreparedEnrollment prepared = {.revision = 777U};
    UmiLocalMfaPendingEnrollment pending = {0}, next = {0};
    UmiLocalMfaCredential active = {0}, reopened = {0};
    UmiLocalMfaBackend signIn, checkedAgain;
    UmiLocalMfaEnrollmentBackend enrollment, pendingAgain;
    UmiSessionRecord session = {0};
    char code[9] = {0}, uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY] = {0};
    size_t uriLength = 0U;
    int result = 1;
    UmiStatus status = UmiLocalProfileStoreDatabase(path, application, &profiles);
    if (status == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRegister(profiles, "Alice", fixturePassword) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseOpen("relative.sqlite", application, &first) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &first) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(first, &signIn, &enrollment) == UMI_STATUS_OK);
    CHECK(umi_session_registry_create(&sessions) == UMI_STATUS_OK);

    CHECK(UmiLocalMfaDatabasePrepare(first, profiles, "Alice", "fictional wrong password",
        "Practice", UMI_TOTP_SHA1, 6U, startTime, startTime + stepNs, &prepared) ==
        UMI_STATUS_PERMISSION_DENIED);
    CHECK(prepared.revision == 777U && enrollment.readPending(enrollment.context, "alice", &pending) ==
        UMI_STATUS_NOT_FOUND);
    CHECK(UmiLocalMfaDatabasePrepare(first, profiles, "Alice", fixturePassword,
        "Practice", UMI_TOTP_SHA1, 6U, startTime, startTime + stepNs, &prepared) == UMI_STATUS_OK);
    CHECK(prepared.revision == 1U && prepared.uriLength != 0U);
    UmiLocalMfaDatabaseClose(first);
    first = NULL;

    CHECK(UmiLocalMfaDatabaseOpen(path, application, &first) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(first, &signIn, &enrollment) == UMI_STATUS_OK);
    CHECK(enrollment.readPending(enrollment.context, "alice", &pending) == UMI_STATUS_OK);
    CHECK(pending.enrollmentRevision == 1U && pending.expiresAtNs == startTime + stepNs);
    CHECK(UmiTotpBuildEnrollmentUri("Practice", "alice", pending.credential.algorithm,
        pending.credential.digits, pending.credential.secret, pending.credential.secretBytes,
        uri, sizeof uri, &uriLength) == UMI_STATUS_OK);
    CHECK(uriLength == prepared.uriLength && memcmp(uri, prepared.uri, uriLength + 1U) == 0);
    CHECK(sqlite3_open_v2(path, &external, SQLITE_OPEN_READONLY, NULL) == SQLITE_OK);
    CHECK(CheckProtectedRow(external, &pending.credential, false));
    CHECK(signIn.read(signIn.context, "alice", &active) == UMI_STATUS_NOT_FOUND);
    CHECK(AuthenticatorCode(&pending.credential, startTime + stepNs, code));
    CHECK(UmiLocalProfileMfaConfirmEnrollment(profiles, &enrollment, "alice", fixturePassword,
        prepared.revision, code, 6U, startTime + stepNs) == UMI_STATUS_PERMISSION_DENIED);

    CHECK(UmiLocalMfaDatabaseOpen(path, application, &second) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(second, &checkedAgain, &pendingAgain) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabasePrepare(second, profiles, "ALICE", fixturePassword,
        "Practice", UMI_TOTP_SHA256, 8U, startTime + stepNs, startTime + 4U * stepNs, &prepared) == UMI_STATUS_OK);
    CHECK(prepared.revision == 2U);
    CHECK(UmiLocalProfileMfaConfirmEnrollment(profiles, &enrollment, "Alice", fixturePassword,
        pending.enrollmentRevision, code, 6U, startTime + stepNs) == UMI_STATUS_BUSY);
    CHECK(pendingAgain.readPending(pendingAgain.context, "alice", &next) == UMI_STATUS_OK);
    CHECK(next.enrollmentRevision == 2U && next.credential.secretBytes == 32U);
    CHECK(AuthenticatorCode(&next.credential, startTime + stepNs, code));
    CHECK(UmiLocalProfileMfaConfirmEnrollment(profiles, &enrollment, "alice", fixturePassword,
        prepared.revision, code, 8U, startTime + stepNs) == UMI_STATUS_OK);
    CHECK(pendingAgain.readPending(pendingAgain.context, "alice", &next) == UMI_STATUS_NOT_FOUND);
    CHECK(checkedAgain.read(checkedAgain.context, "alice", &active) == UMI_STATUS_OK);
    CHECK(active.credentialRevision == 2U && active.replay.has_last_step);
    CHECK(CheckProtectedRow(external, &active, true));
    CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &checkedAgain, "alice", fixturePassword,
        code, 8U, startTime + stepNs, 2U * stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_session_registry_count(sessions) == 0U);
    CHECK(AuthenticatorCode(&active, startTime + 2U * stepNs, code));
    CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &signIn, "Alice", fixturePassword,
        code, 8U, startTime + 2U * stepNs, 2U * stepNs, &session) == UMI_STATUS_OK);
    CHECK(umi_session_registry_count(sessions) == 1U && strcmp(session.principal_id, "alice") == 0);
    CHECK(checkedAgain.commitReplay(checkedAgain.context, "alice", active.credentialRevision,
        active.replay, (UmiTotpReplay){active.replay.last_step + 1U, true}) == UMI_STATUS_BUSY);
    UmiLocalMfaDatabaseClose(second);
    second = NULL;
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &second) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(second, &checkedAgain, &pendingAgain) == UMI_STATUS_OK);
    CHECK(checkedAgain.read(checkedAgain.context, "alice", &reopened) == UMI_STATUS_OK);
    CHECK(reopened.replay.last_step == active.replay.last_step + 1U);
    CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &checkedAgain, "alice", fixturePassword,
        code, 8U, startTime + 2U * stepNs, 2U * stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
    const uint64_t revision = prepared.revision;
    CHECK(UmiLocalMfaDatabasePrepare(second, profiles, "alice", fixturePassword,
        "Practice", UMI_TOTP_SHA1, 6U, startTime + 2U * stepNs, startTime + 4U * stepNs, &prepared) ==
        UMI_STATUS_ALREADY_EXISTS);
    CHECK(prepared.revision == revision);
    puts("Encrypted pending activation, exact revision checks and replay after reopening passed");
    result = 0;
finish:
    umi_secret_clear(code, sizeof code);
    umi_secret_clear(uri, sizeof uri);
    umi_secret_clear(&pending, sizeof pending);
    umi_secret_clear(&next, sizeof next);
    umi_secret_clear(&active, sizeof active);
    umi_secret_clear(&reopened, sizeof reopened);
    umi_secret_clear(&prepared, sizeof prepared);
    umi_secret_clear(&session, sizeof session);
    if (external != NULL) (void)sqlite3_close_v2(external);
    umi_session_registry_destroy(sessions);
    UmiLocalMfaDatabaseClose(second);
    UmiLocalMfaDatabaseClose(first);
    UmiLocalProfileStoreRelease(profiles);
    return result;
}

static int Scope(const char *path, const char *application)
{
    UmiLocalProfileStore *profiles = NULL;
    UmiLocalMfaDatabase *first = NULL, *other = NULL;
    sqlite3 *external = NULL;
    UmiLocalMfaBackend signIn, foreign;
    UmiLocalMfaEnrollmentBackend enrollment, foreignEnrollment;
    UmiLocalMfaPreparedEnrollment prepared = {0};
    UmiLocalMfaPendingEnrollment pending = {0};
    char foreignApp[96];
    int result = 1;
    UmiStatus status = UmiLocalProfileStoreDatabase(path, application, &profiles);
    if (status == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRegister(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &first) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(first, &signIn, &enrollment) == UMI_STATUS_OK);
    const int characters = snprintf(foreignApp, sizeof foreignApp, "%s.other", application);
    CHECK(characters > 0 && (size_t)characters < sizeof foreignApp);
    CHECK(UmiLocalMfaDatabaseOpen(path, foreignApp, &other) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(other, &foreign, &foreignEnrollment) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabasePrepare(first, profiles, "alice", fixturePassword,
        "Practice", UMI_TOTP_SHA1, 6U, startTime, startTime + stepNs, &prepared) == UMI_STATUS_OK);
    CHECK(foreignEnrollment.readPending(foreignEnrollment.context, "alice", &pending) == UMI_STATUS_NOT_FOUND);
    CHECK(enrollment.readPending(enrollment.context, "bravo", &pending) == UMI_STATUS_NOT_FOUND);
    CHECK(sqlite3_open_v2(path, &external, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK);
    /* Simulate an incorrectly restored row in this synthetic database only.
     * A DPAPI record remains tied to its original app and canonical user. */
    CHECK(sqlite3_exec(external,
        "INSERT INTO local_mfa_v1 SELECT application||'.other',username,generation,active,active_seed,replay,"
        "pending,pending_seed,expires FROM local_mfa_v1 WHERE username='alice';"
        "INSERT INTO local_mfa_v1 SELECT application,'bravo',generation,active,active_seed,replay,"
        "pending,pending_seed,expires FROM local_mfa_v1 WHERE username='alice' AND application NOT LIKE '%.other';",
        NULL, NULL, NULL) == SQLITE_OK);
    CHECK(foreignEnrollment.readPending(foreignEnrollment.context, "alice", &pending) == UMI_STATUS_PARSE_ERROR);
    CHECK(enrollment.readPending(enrollment.context, "bravo", &pending) == UMI_STATUS_PARSE_ERROR);
    CHECK(enrollment.readPending(enrollment.context, "alice", &pending) == UMI_STATUS_OK);
    CHECK(pending.enrollmentRevision == prepared.revision);
    puts("Protected seed refused after synthetic app and profile changes");
    result = 0;
finish:
    umi_secret_clear(&pending, sizeof pending);
    umi_secret_clear(&prepared, sizeof prepared);
    if (external != NULL) (void)sqlite3_close_v2(external);
    UmiLocalMfaDatabaseClose(other);
    UmiLocalMfaDatabaseClose(first);
    UmiLocalProfileStoreRelease(profiles);
    return result;
}

static bool RecoveryDigestsOnly(sqlite3 *db, const UmiLocalMfaRecoveryCodes *codes, uint64_t serial)
{
    sqlite3_stmt *query = NULL;
    bool match = false;
    if (sqlite3_prepare_v2(db, "SELECT serial,available,digests FROM local_mfa_recovery_v1", -1, &query, NULL) == SQLITE_OK &&
        sqlite3_step(query) == SQLITE_ROW && sqlite3_column_int64(query, 0) == (sqlite3_int64)serial &&
        sqlite3_column_int(query, 1) == 255 && sqlite3_column_type(query, 2) == SQLITE_BLOB &&
        sqlite3_column_bytes(query, 2) == 256) {
        const unsigned char *bytes = sqlite3_column_blob(query, 2);
        match = bytes != NULL;
        for (size_t index = 0U; match && index < UMI_LOCAL_MFA_RECOVERY_COUNT; ++index) {
            const char *code = codes->codes[index];
            match = strlen(code) == UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH &&
                strspn(code, "0123456789abcdef") == UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH;
            for (size_t offset = 0U; match && offset <= 256U - UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH; ++offset)
                match = memcmp(bytes + offset, code, UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH) != 0;
            for (size_t prior = 0U; match && prior < index; ++prior)
                match = strcmp(codes->codes[prior], code) != 0;
        }
        match = match && sqlite3_step(query) == SQLITE_DONE;
    }
    (void)sqlite3_finalize(query);
    return match;
}

typedef struct RecoveryJob {
    UmiLocalMfaDatabase *database;
    UmiLocalProfileStore *profiles;
    UmiSessionRegistry *sessions;
    const char *code;
    atomic_bool *start;
    UmiStatus status;
} RecoveryJob;

static int RecoverOnThread(void *context)
{
    RecoveryJob *job = context;
    while (!atomic_load_explicit(job->start, memory_order_acquire)) umi_thread_sleep_ms(1U);
    UmiSessionRecord session = {0};
    job->status = UmiLocalMfaDatabaseRecoverySignIn(job->database, job->profiles, job->sessions, "alice", fixturePassword,
        job->code, UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH, startTime + stepNs, stepNs, &session);
    umi_secret_clear(&session, sizeof session);
    return 0;
}

static bool CompareNativeRecoveryJobs(UmiLocalMfaDatabase *first, UmiLocalMfaDatabase *second,
    UmiLocalProfileStore *profiles, UmiLocalProfileStore *otherProfiles, UmiSessionRegistry *sessions,
    const char *firstCode, const char *secondCode, unsigned expected)
{
    atomic_bool start;
    atomic_init(&start, false);
    RecoveryJob jobs[] = {{first, profiles, sessions, firstCode, &start, UMI_STATUS_INTERNAL_ERROR},
                          {second, otherProfiles, sessions, secondCode, &start, UMI_STATUS_INTERNAL_ERROR}};
    UmiThread *threads[2] = {0};
    bool healthy = umi_thread_start(RecoverOnThread, &jobs[0], &threads[0]) == UMI_STATUS_OK &&
                   umi_thread_start(RecoverOnThread, &jobs[1], &threads[1]) == UMI_STATUS_OK;
    atomic_store_explicit(&start, true, memory_order_release);
    unsigned succeeded = 0U;
    for (size_t index = 0U; index < 2U; ++index) {
        int result = 1;
        if (threads[index] == NULL || umi_thread_join(threads[index], &result) != UMI_STATUS_OK || result != 0)
            healthy = false;
        umi_thread_destroy(threads[index]);
        if (jobs[index].status == UMI_STATUS_OK) ++succeeded;
        else if (jobs[index].status != UMI_STATUS_BUSY && jobs[index].status != UMI_STATUS_PERMISSION_DENIED) healthy = false;
    }
    return healthy && succeeded == expected;
}

static int Recovery(const char *path, const char *application, const char *scenario)
{
    UmiLocalProfileStore *profiles = NULL, *otherProfiles = NULL;
    UmiLocalMfaDatabase *database = NULL, *other = NULL;
    UmiSessionRegistry *sessions = NULL, *full = NULL;
    sqlite3 *external = NULL;
    UmiLocalMfaPreparedEnrollment prepared = {0};
    UmiLocalMfaPendingEnrollment pending = {0};
    UmiLocalMfaCredential active = {0};
    UmiLocalMfaBackend signIn;
    UmiLocalMfaEnrollmentBackend enrollment;
    UmiLocalMfaRecoveryCodes codes = {0}, initial = {0}, replacement = {0};
    UmiSessionRecord session = {0}, prior = {0};
    const uint64_t issuance = startTime + stepNs;
    const char wrongPassword[] = "fictional different password";
    char authenticator[9] = {0};
    int result = 1;

    UmiStatus status = UmiLocalProfileStoreDatabase(path, application, &profiles);
    if (status == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRegister(profiles, "Alice", fixturePassword) == UMI_STATUS_OK);
    CHECK(umi_session_registry_create(&sessions) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &database) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(database, &signIn, &enrollment) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabasePrepare(database, profiles, "Alice", fixturePassword,
        "Practice", UMI_TOTP_SHA1, 6U, startTime, startTime + stepNs, &prepared) == UMI_STATUS_OK);
    CHECK(enrollment.readPending(enrollment.context, "alice", &pending) == UMI_STATUS_OK);
    CHECK(AuthenticatorCode(&pending.credential, startTime, authenticator));
    memset(&codes, 0x7a, sizeof codes);
    initial = codes;
    CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "Alice", fixturePassword,
        authenticator, 6U, startTime, &codes) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&codes, &initial, sizeof codes) == 0);
    CHECK(UmiLocalProfileMfaConfirmEnrollment(profiles, &enrollment, "alice", fixturePassword,
        prepared.revision, authenticator, 6U, startTime) == UMI_STATUS_OK);
    CHECK(AuthenticatorCode(&pending.credential, issuance, authenticator));
    CHECK(sqlite3_open_v2(path, &external, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK);

    if (!strcmp(scenario, "recovery-legacy")) {
        /* Simulate an enrollment stored before the password binding existed. */
        CHECK(sqlite3_exec(external, "DELETE FROM local_mfa_recovery_v1", NULL, NULL, NULL) == SQLITE_OK);
        CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "alice", fixturePassword,
            authenticator, 6U, issuance, &codes) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&codes, &initial, sizeof codes) == 0);
        CHECK(UmiLocalMfaDatabaseSignIn(database, profiles, sessions, "alice", fixturePassword,
            authenticator, 6U, issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_session_registry_count(sessions) == 0U);
        CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &signIn, "alice", fixturePassword,
            authenticator, 6U, issuance, stepNs, &session) == UMI_STATUS_OK);
        puts("An existing unbound enrollment kept native TOTP sign-in and refused recovery issuance");
        result = 0;
        goto finish;
    }

    if (!strcmp(scenario, "recovery-rollback")) {
        CHECK(sqlite3_exec(external, "CREATE TRIGGER fixture_rollback BEFORE UPDATE OF digests ON local_mfa_recovery_v1"
            " BEGIN SELECT RAISE(ABORT,'synthetic failed recovery write'); END", NULL, NULL, NULL) == SQLITE_OK);
        CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "Alice", fixturePassword,
            authenticator, 6U, issuance, &codes) == UMI_STATUS_IO_ERROR);
        CHECK(memcmp(&codes, &initial, sizeof codes) == 0);
        CHECK(signIn.read(signIn.context, "alice", &active) == UMI_STATUS_OK);
        CHECK(active.replay.last_step == startTime / stepNs);
        CHECK(sqlite3_exec(external, "DROP TRIGGER fixture_rollback", NULL, NULL, NULL) == SQLITE_OK);
    }
    CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "Alice", wrongPassword,
        authenticator, 6U, issuance, &codes) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&codes, &initial, sizeof codes) == 0);
    CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "Alice", fixturePassword,
        "???", 3U, issuance, &codes) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&codes, &initial, sizeof codes) == 0);
    CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "ALICE", fixturePassword,
        authenticator, 6U, issuance, &codes) == UMI_STATUS_OK);
    CHECK(RecoveryDigestsOnly(external, &codes, issuance / stepNs));
    CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "Alice", fixturePassword,
        authenticator, 6U, issuance, &initial) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &signIn, "alice", fixturePassword,
        authenticator, 6U, issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);

    UmiLocalMfaDatabaseClose(database);
    database = NULL;
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &database) == UMI_STATUS_OK);
    if (!strcmp(scenario, "recovery-race")) {
        CHECK(UmiLocalMfaDatabaseOpen(path, application, &other) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileStoreDatabase(path, application, &otherProfiles) == UMI_STATUS_OK);
        CHECK(CompareNativeRecoveryJobs(database, other, profiles, otherProfiles, sessions,
                                       codes.codes[0], codes.codes[0], 1U));
        CHECK(CompareNativeRecoveryJobs(database, other, profiles, otherProfiles, sessions,
                                       codes.codes[1], codes.codes[2], 2U));
        CHECK(umi_session_registry_count(sessions) == 3U);
        puts("Concurrent handles granted one session for a shared code and one each for two distinct codes");
        result = 0;
        goto finish;
    }

    if (!strcmp(scenario, "recovery-binding")) {
        CHECK(UmiLocalProfileRegister(profiles, "bravo", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalMfaDatabaseRecoverySignIn(database, profiles, sessions, "bravo", fixturePassword,
            codes.codes[0], strlen(codes.codes[0]), issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiLocalProfileRemove(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileRegister(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileVerify(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalMfaDatabaseRecoverySignIn(database, profiles, sessions, "alice", fixturePassword,
            codes.codes[0], strlen(codes.codes[0]), issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(AuthenticatorCode(&pending.credential, issuance + stepNs, authenticator));
        CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "alice", fixturePassword,
            authenticator, 6U, issuance + stepNs, &replacement) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_session_registry_count(sessions) == 0U);
        puts("A deleted and recreated same-name registration could not consume or issue recovery codes");
        result = 0;
        goto finish;
    }

    memset(&session, 0x33, sizeof session);
    memcpy(&prior, &session, sizeof prior);
    CHECK(UmiLocalMfaDatabaseRecoverySignIn(database, profiles, sessions, "alice", wrongPassword,
        codes.codes[0], strlen(codes.codes[0]), issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&prior, &session, sizeof session) == 0 && umi_session_registry_count(sessions) == 0U);
    CHECK(UmiLocalMfaDatabaseRecoverySignIn(database, profiles, sessions, "alice", fixturePassword,
        codes.codes[0], 1U, issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&prior, &session, sizeof session) == 0);
    CHECK(UmiLocalMfaDatabaseRecoverySignIn(database, profiles, sessions, "ALICE", fixturePassword,
        codes.codes[0], strlen(codes.codes[0]), issuance, stepNs, &session) == UMI_STATUS_OK);
    CHECK(strcmp(session.principal_id, "alice") == 0);
    CHECK(umi_session_registry_validate(sessions, session.session_id, issuance, &prior) == UMI_STATUS_OK);
    CHECK(umi_session_registry_count(sessions) == 1U);
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &other) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseRecoverySignIn(other, profiles, sessions, "alice", fixturePassword,
        codes.codes[0], strlen(codes.codes[0]), issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
    if (strcmp(scenario, "recovery") == 0) {
        CHECK(umi_session_registry_create(&full) == UMI_STATUS_OK);
        for (size_t index = 0U; index < UMI_SESSION_REGISTRY_MAX; ++index)
            CHECK(umi_session_registry_issue(full, "practice-session", issuance, stepNs, &prior) == UMI_STATUS_OK);
        memcpy(&prior, &session, sizeof prior);
        CHECK(UmiLocalMfaDatabaseRecoverySignIn(other, profiles, full, "alice", fixturePassword,
            codes.codes[2], strlen(codes.codes[2]), issuance, stepNs, &session) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&session, &prior, sizeof session) == 0);
        CHECK(UmiLocalMfaDatabaseRecoverySignIn(database, profiles, sessions, "alice", fixturePassword,
            codes.codes[2], strlen(codes.codes[2]), issuance, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
        puts("An already consumed recovery code stayed spent when the session registry was full");
    }
    CHECK(AuthenticatorCode(&pending.credential, issuance + stepNs, authenticator));
    CHECK(UmiLocalMfaDatabaseIssueRecovery(database, profiles, "alice", fixturePassword,
        authenticator, 6U, issuance + stepNs, &replacement) == UMI_STATUS_OK);
    CHECK(RecoveryDigestsOnly(external, &replacement, (issuance + stepNs) / stepNs));
    CHECK(UmiLocalMfaDatabaseRecoverySignIn(other, profiles, sessions, "alice", fixturePassword,
        codes.codes[1], strlen(codes.codes[1]), issuance + stepNs, stepNs, &session) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalMfaDatabaseRecoverySignIn(other, profiles, sessions, "alice", fixturePassword,
        replacement.codes[7], strlen(replacement.codes[7]), issuance + stepNs, stepNs, &session) == UMI_STATUS_OK);
    CHECK(umi_session_registry_count(sessions) == 2U);
    puts(!strcmp(scenario, "recovery-rollback")
        ? "A failed recovery write rolled back the authenticator step before the same proof succeeded"
        : "Native recovery codes persisted as digests, were consumed once and rotated across handles");
    result = 0;

finish:
    umi_secret_clear(&prepared, sizeof prepared);
    umi_secret_clear(&pending, sizeof pending);
    umi_secret_clear(&active, sizeof active);
    umi_secret_clear(&session, sizeof session);
    umi_secret_clear(&prior, sizeof prior);
    umi_secret_clear(authenticator, sizeof authenticator);
    umi_secret_clear(&codes, sizeof codes);
    umi_secret_clear(&initial, sizeof initial);
    umi_secret_clear(&replacement, sizeof replacement);
    if (external != NULL) (void)sqlite3_close_v2(external);
    UmiLocalMfaDatabaseClose(other);
    UmiLocalMfaDatabaseClose(database);
    UmiLocalProfileStoreRelease(otherProfiles);
    UmiLocalProfileStoreRelease(profiles);
    umi_session_registry_destroy(full);
    umi_session_registry_destroy(sessions);
    return result;
}

static int BoundAccount(const char *path, const char *application, bool pendingOnly)
{
    UmiLocalProfileStore *profiles = NULL;
    UmiLocalMfaDatabase *database = NULL;
    UmiSessionRegistry *sessions = NULL;
    UmiLocalMfaPreparedEnrollment prepared = {0};
    UmiLocalMfaPendingEnrollment pending = {0}, unchanged = {0};
    UmiLocalMfaBackend active;
    UmiLocalMfaEnrollmentBackend enrollment;
    UmiSessionRecord observed, before;
    memset(&observed, 0x62, sizeof observed);
    memcpy(&before, &observed, sizeof before);
    char authenticator[9] = {0};
    int result = 1;
    UmiStatus status = UmiLocalProfileStoreDatabase(path, application, &profiles);
    if (status == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRegister(profiles, "Alice", fixturePassword) == UMI_STATUS_OK);
    CHECK(umi_session_registry_create(&sessions) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseOpen(path, application, &database) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabaseBackends(database, &active, &enrollment) == UMI_STATUS_OK);
    CHECK(UmiLocalMfaDatabasePrepare(database, profiles, "alice", fixturePassword,
        "Practice", UMI_TOTP_SHA1, 6U, startTime, startTime + stepNs, &prepared) == UMI_STATUS_OK);
    CHECK(enrollment.readPending(enrollment.context, "alice", &pending) == UMI_STATUS_OK);
    CHECK(AuthenticatorCode(&pending.credential, startTime, authenticator));
    CHECK(UmiLocalMfaDatabaseSignIn(database, profiles, sessions, "alice", fixturePassword,
        authenticator, 6U, startTime, stepNs, &observed) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&before, &observed, sizeof observed) == 0);
    CHECK(UmiLocalMfaDatabaseConfirmEnrollment(database, profiles, "alice", "fictional incorrect password",
        prepared.revision, authenticator, 6U, startTime) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalMfaDatabaseConfirmEnrollment(database, profiles, "alice", fixturePassword,
        prepared.revision + 1U, authenticator, 6U, startTime) == UMI_STATUS_BUSY);
    CHECK(UmiLocalMfaDatabaseConfirmEnrollment(database, profiles, "alice", fixturePassword,
        prepared.revision, authenticator, 6U, startTime + stepNs) == UMI_STATUS_PERMISSION_DENIED);

    if (pendingOnly) {
        CHECK(UmiLocalProfileRemove(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileRegister(profiles, "ALICE", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalMfaDatabaseConfirmEnrollment(database, profiles, "ALICE", fixturePassword,
            prepared.revision, authenticator, 6U, startTime) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(enrollment.readPending(enrollment.context, "alice", &unchanged) == UMI_STATUS_OK);
        CHECK(unchanged.enrollmentRevision == pending.enrollmentRevision && !unchanged.credential.replay.has_last_step);
        CHECK(umi_session_registry_count(sessions) == 0U);
        puts("A recreated password registration could not confirm an earlier pending enrollment");
    } else {
        CHECK(UmiLocalMfaDatabaseConfirmEnrollment(database, profiles, "ALICE", fixturePassword,
            prepared.revision, authenticator, 6U, startTime) == UMI_STATUS_OK);
        CHECK(UmiLocalMfaDatabaseSignIn(database, profiles, sessions, "alice", fixturePassword,
            authenticator, 6U, startTime, stepNs, &observed) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&before, &observed, sizeof observed) == 0);
        CHECK(AuthenticatorCode(&pending.credential, startTime + stepNs, authenticator));
        CHECK(UmiLocalMfaDatabaseSignIn(database, profiles, sessions, "ALICE", fixturePassword,
            authenticator, 6U, startTime + stepNs, stepNs, &observed) == UMI_STATUS_OK);
        CHECK(strcmp(observed.principal_id, "alice") == 0);
        CHECK(umi_session_registry_validate(sessions, observed.session_id, startTime + stepNs, &before) == UMI_STATUS_OK);
        UmiLocalMfaDatabaseClose(database);
        database = NULL;
        CHECK(UmiLocalMfaDatabaseOpen(path, application, &database) == UMI_STATUS_OK);
        memcpy(&before, &observed, sizeof before);
        CHECK(UmiLocalMfaDatabaseSignIn(database, profiles, sessions, "alice", fixturePassword,
            authenticator, 6U, startTime + stepNs, stepNs, &observed) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiLocalProfileRemove(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileRegister(profiles, "alice", fixturePassword) == UMI_STATUS_OK);
        CHECK(AuthenticatorCode(&pending.credential, startTime + 2U * stepNs, authenticator));
        CHECK(UmiLocalMfaDatabaseSignIn(database, profiles, sessions, "alice", fixturePassword,
            authenticator, 6U, startTime + 2U * stepNs, stepNs, &observed) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&before, &observed, sizeof observed) == 0 && umi_session_registry_count(sessions) == 1U);
        puts("The bound native authenticator granted one session and refused replay and a recreated registration");
    }
    result = 0;
finish:
    umi_secret_clear(&prepared, sizeof prepared);
    umi_secret_clear(&pending, sizeof pending);
    umi_secret_clear(&unchanged, sizeof unchanged);
    umi_secret_clear(&observed, sizeof observed);
    umi_secret_clear(&before, sizeof before);
    umi_secret_clear(authenticator, sizeof authenticator);
    UmiLocalMfaDatabaseClose(database);
    UmiLocalProfileStoreRelease(profiles);
    umi_session_registry_destroy(sessions);
    return result;
}

static int Run(int argc, char **argv)
{
    if (argc != 4) return 2;
    if (strcmp(argv[2], "lifecycle") == 0) return Lifecycle(argv[1], argv[3]);
    if (strcmp(argv[2], "scope") == 0) return Scope(argv[1], argv[3]);
    if (strcmp(argv[2], "bound-pending") == 0) return BoundAccount(argv[1], argv[3], true);
    if (strcmp(argv[2], "bound-active") == 0) return BoundAccount(argv[1], argv[3], false);
    if (strcmp(argv[2], "recovery") == 0 || strcmp(argv[2], "recovery-rollback") == 0 ||
        strcmp(argv[2], "recovery-race") == 0 || strcmp(argv[2], "recovery-binding") == 0 ||
        strcmp(argv[2], "recovery-legacy") == 0) return Recovery(argv[1], argv[3], argv[2]);
    return 2;
}

int main(int argc, char **argv)
{
    int result = 1;
    const UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, Run, &result);
    return status == UMI_STATUS_OK ? result : 1;
}
