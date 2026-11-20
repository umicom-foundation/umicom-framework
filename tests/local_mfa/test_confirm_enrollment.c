/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_mfa/test_confirm_enrollment.c
 *
 * PURPOSE:
 *   Verify atomic activation of a confirmed authenticator and consumption of its first code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/local_profile_mfa.h"
#include "../local_profile/fixture.h"

typedef struct EnrollmentFixture {
    UmiLocalMfaPendingEnrollment pending;
    UmiLocalMfaCredential active;
    UmiStatus promotionStatus;
    unsigned reads, promotions, writes, replayWrites;
    bool hasPending;
    bool replacePending;
    bool replaceActive;
} EnrollmentFixture;

static UmiStatus ReadPending(void *context, const char *profile, UmiLocalMfaPendingEnrollment *out)
{
    EnrollmentFixture *fixture = context;
    CHECK(strcmp(profile, "classroom") == 0);
    ++fixture->reads;
    if (!fixture->hasPending) return UMI_STATUS_NOT_FOUND;
    *out = fixture->pending;
    return UMI_STATUS_OK;
}

static UmiStatus Promote(void *context, const char *profile, uint64_t enrollment,
    uint64_t replaced, const UmiLocalMfaCredential *credential)
{
    EnrollmentFixture *fixture = context;
    CHECK(strcmp(profile, "classroom") == 0);
    CHECK(credential->credentialRevision == 8U && credential->secretBytes == 32U);
    CHECK(credential->replay.has_last_step && credential->replay.last_step == 1U);
    ++fixture->promotions;
    if (fixture->replacePending) ++fixture->pending.enrollmentRevision;
    if (fixture->replaceActive) ++fixture->active.credentialRevision;
    if (!fixture->hasPending || enrollment != fixture->pending.enrollmentRevision ||
        replaced != fixture->active.credentialRevision)
        return UMI_STATUS_BUSY;
    if (fixture->promotionStatus != UMI_STATUS_OK) return fixture->promotionStatus;
    fixture->active = *credential;
    fixture->hasPending = false;
    ++fixture->writes;
    return UMI_STATUS_OK;
}

static UmiStatus ReadActive(void *context, const char *profile, UmiLocalMfaCredential *out)
{
    EnrollmentFixture *fixture = context;
    CHECK(strcmp(profile, "classroom") == 0);
    if (fixture->active.credentialRevision == 0U) return UMI_STATUS_NOT_FOUND;
    *out = fixture->active;
    return UMI_STATUS_OK;
}

static UmiStatus CommitReplay(void *context, const char *profile, uint64_t revision,
    UmiTotpReplay previous, UmiTotpReplay replacement)
{
    EnrollmentFixture *fixture = context;
    CHECK(strcmp(profile, "classroom") == 0 && revision == fixture->active.credentialRevision);
    CHECK(previous.has_last_step && previous.last_step == fixture->active.replay.last_step);
    CHECK(replacement.has_last_step && replacement.last_step > previous.last_step);
    fixture->active.replay = replacement;
    ++fixture->replayWrites;
    return UMI_STATUS_OK;
}

#ifndef _WIN32
/* Use published code values as an injected provider on non-CNG hosts. */
static UmiStatus FixtureMac(void *context, UmiTotpAlgorithm algorithm,
    const uint8_t *secret, size_t length, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES])
{
    (void)context;
    CHECK(algorithm == UMI_TOTP_SHA256 && length == 32U && secret[0] == '1');
    uint64_t step = 0U;
    for (size_t i = 0U; i < UMI_TOTP_COUNTER_BYTES; ++i) step = (step << 8U) | counter[i];
    const uint32_t code = step == 1U ? UINT32_C(46119246) :
        step == UINT64_C(37037036) ? UINT32_C(68084774) : UINT32_C(12345678);
    memset(digest, 0, UMI_TOTP_MAX_DIGEST_BYTES);
    digest[0] = (uint8_t)(code >> 24U);
    digest[1] = (uint8_t)(code >> 16U);
    digest[2] = (uint8_t)(code >> 8U);
    digest[3] = (uint8_t)code;
    return UMI_STATUS_OK;
}
#endif

static void VerifySignIn(UmiLocalProfileStore *profiles, EnrollmentFixture *fixture, UmiTotpMac mac,
    const char *password, uint64_t now)
{
    UmiSessionRegistry *sessions = NULL;
    CHECK(umi_session_registry_create(&sessions) == UMI_STATUS_OK);
    const UmiLocalMfaBackend signIn = {.context = fixture, .read = ReadActive,
        .commitReplay = CommitReplay, .mac = mac};
    UmiSessionRecord session = {0};
    CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &signIn, "Classroom", password,
        "46119246", 8U, now, UINT64_C(60000000000), &session) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(fixture->replayWrites == 0U);
    const uint64_t later = UINT64_C(1111111109000000000);
    CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &signIn, "Classroom", password,
        "68084774", 8U, later, UINT64_C(60000000000), &session) == UMI_STATUS_OK);
    CHECK(fixture->replayWrites == 1U);
    umi_session_registry_destroy(sessions);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *password = "fixture password";
    const char *code = "46119246";
    uint64_t ticket = 19U;
    const uint64_t now = UINT64_C(59000000000);
    EnrollmentFixture fixture = {.hasPending = true,
        .pending = {.enrollmentRevision = 19U, .expiresAtNs = now + 1U,
            .credential = {.credentialRevision = 8U, .algorithm = UMI_TOTP_SHA256,
                .digits = 8U, .secretBytes = 32U}}};
    memcpy(fixture.pending.credential.secret, "12345678901234567890123456789012", 32U);
    UmiLocalMfaEnrollmentBackend backend = {.context = &fixture, .readPending = ReadPending, .promote = Promote};
#ifndef _WIN32
    backend.mac = FixtureMac;
#endif

    UmiStatus expected = UMI_STATUS_OK;
    unsigned reads = 1U, promotions = 0U;
    if (strcmp(mode, "activate") == 0 || strcmp(mode, "activate-and-sign-in") == 0 ||
        strcmp(mode, "duplicate-confirmation") == 0) {
        promotions = 1U;
    } else if (strcmp(mode, "rotate") == 0) {
        fixture.active.credentialRevision = fixture.pending.replacesCredentialRevision = 7U;
        promotions = 1U;
    } else if (strcmp(mode, "password-first") == 0) {
        password = "wrong fixture password";
        expected = UMI_STATUS_PERMISSION_DENIED;
        reads = 0U;
    } else if (strcmp(mode, "wrong-code") == 0) {
        code = "00000000";
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "missing") == 0) {
        fixture.hasPending = false;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "expired") == 0) {
        fixture.pending.expiresAtNs = now;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "stale-ticket") == 0) {
        ++ticket;
        expected = UMI_STATUS_BUSY;
    } else if (strcmp(mode, "pending-race") == 0) {
        fixture.replacePending = true;
        expected = UMI_STATUS_BUSY;
        promotions = 1U;
    } else if (strcmp(mode, "active-race") == 0) {
        fixture.replaceActive = true;
        expected = UMI_STATUS_BUSY;
        promotions = 1U;
    } else if (strcmp(mode, "write-failure") == 0) {
        fixture.promotionStatus = expected = UMI_STATUS_IO_ERROR;
        promotions = 1U;
    } else if (strcmp(mode, "invalid-record") == 0) {
        fixture.pending.replacesCredentialRevision = fixture.pending.credential.credentialRevision;
        expected = UMI_STATUS_PARSE_ERROR;
    } else {
        CHECK(strcmp(mode, "reused-pending") == 0);
        fixture.pending.credential.replay.has_last_step = true;
        expected = UMI_STATUS_PARSE_ERROR;
    }

    ProfileFixture passwords = {0};
    UmiLocalProfileStore *profiles = FixtureStore(&passwords);
    CHECK(UmiLocalProfileRegister(profiles, "classroom", "fixture password") == UMI_STATUS_OK);
    CHECK(UmiLocalProfileMfaConfirmEnrollment(profiles, &backend, "Classroom", password,
        ticket, code, 8U, now) == expected);
    CHECK(fixture.reads == reads && fixture.promotions == promotions);
    CHECK(fixture.writes == (expected == UMI_STATUS_OK ? 1U : 0U));
    if (expected == UMI_STATUS_OK)
        CHECK(fixture.active.credentialRevision == 8U && !fixture.hasPending &&
            fixture.active.replay.has_last_step && fixture.active.replay.last_step == 1U);
    if (strcmp(mode, "activate-and-sign-in") == 0)
        VerifySignIn(profiles, &fixture, backend.mac, password, now);
    if (strcmp(mode, "duplicate-confirmation") == 0) {
        CHECK(UmiLocalProfileMfaConfirmEnrollment(profiles, &backend, "classroom", password,
            ticket, code, 8U, now) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(fixture.promotions == 1U && fixture.writes == 1U);
    }
    UmiLocalProfileStoreRelease(profiles);
    return 0;
}
