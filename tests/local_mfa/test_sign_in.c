/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_mfa/test_sign_in.c
 *
 * PURPOSE:
 *   Verify that password failure, authenticator refusal or a stale replay
 *   write cannot publish a native session for an isolated local profile.
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

typedef struct MfaFixture {
    UmiLocalMfaCredential credential;
    UmiStatus readStatus;
    UmiStatus commitStatus;
    unsigned reads;
    unsigned commits;
    unsigned durableWrites;
    int changeRevision;
    int useStepFirst;
} MfaFixture;

static UmiStatus ReadCredential(void *context, const char *profile, UmiLocalMfaCredential *out)
{
    MfaFixture *fixture = context;
    CHECK(strcmp(profile, "classroom") == 0);
    ++fixture->reads;
    if (fixture->readStatus != UMI_STATUS_OK) return fixture->readStatus;
    *out = fixture->credential;
    return UMI_STATUS_OK;
}

static UmiStatus CommitReplay(void *context, const char *profile, uint64_t revision,
    UmiTotpReplay previous, UmiTotpReplay replacement)
{
    MfaFixture *fixture = context;
    CHECK(strcmp(profile, "classroom") == 0);
    CHECK(replacement.has_last_step && replacement.last_step == 1U);
    ++fixture->commits;
    if (fixture->changeRevision) ++fixture->credential.credentialRevision;
    if (fixture->useStepFirst) fixture->credential.replay = replacement;
    const UmiTotpReplay current = fixture->credential.replay;
    if (revision != fixture->credential.credentialRevision ||
        previous.has_last_step != current.has_last_step || previous.last_step != current.last_step)
        return UMI_STATUS_BUSY;
    if (fixture->commitStatus != UMI_STATUS_OK) return fixture->commitStatus;
    fixture->credential.replay = replacement;
    ++fixture->durableWrites;
    return UMI_STATUS_OK;
}

static UmiStatus UnavailableProvider(void *context, UmiTotpAlgorithm algorithm,
    const uint8_t *secret, size_t length, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES])
{
    (void)context;
    (void)algorithm;
    (void)secret;
    (void)length;
    (void)counter;
    (void)digest;
    return UMI_STATUS_UNAVAILABLE;
}

#ifndef _WIN32
/* A platform without the Windows crypto adapter can still check the composed
 * ownership and refusal rules with known deterministic fixture output. */
static UmiStatus FixtureMac(void *context, UmiTotpAlgorithm algorithm,
    const uint8_t *secret, size_t length, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES])
{
    (void)context;
    CHECK(algorithm == UMI_TOTP_SHA256 && length == 32U && secret[0] == '1');
    const uint32_t code = counter[7] == 1U ? UINT32_C(46119246) : UINT32_C(58349031);
    memset(digest, 0, UMI_TOTP_MAX_DIGEST_BYTES);
    digest[0] = (uint8_t)(code >> 24U);
    digest[1] = (uint8_t)(code >> 16U);
    digest[2] = (uint8_t)(code >> 8U);
    digest[3] = (uint8_t)code;
    return UMI_STATUS_OK;
}
#endif

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *password = "fixture password";
    const char *code = "46119246";
    uint64_t now = UINT64_C(59000000000), lifetime = UINT64_C(60000000000);
    ProfileFixture passwords = {0};
    UmiLocalProfileStore *profiles = FixtureStore(&passwords);
    CHECK(UmiLocalProfileRegister(profiles, "classroom", password) == UMI_STATUS_OK);
    UmiSessionRegistry *sessions = NULL;
    CHECK(umi_session_registry_create(&sessions) == UMI_STATUS_OK);

    /* RFC 6238's published SHA-256 key and t=59 output. It is public test
     * data, independent of local accounts and of the fixture password. */
    MfaFixture fixture = {.credential = {.credentialRevision = 7U, .algorithm = UMI_TOTP_SHA256,
        .digits = 8U, .secretBytes = 32U}};
    memcpy(fixture.credential.secret, "12345678901234567890123456789012", 32U);
    UmiLocalMfaBackend backend = {.context = &fixture, .read = ReadCredential, .commitReplay = CommitReplay};
#ifndef _WIN32
    backend.mac = FixtureMac;
#endif
    UmiStatus expected = UMI_STATUS_OK;
    unsigned reads = 1U, commits = 0U;
    size_t priorSessions = 0U;
    if (strcmp(mode, "password-refused") == 0) {
        password = "wrong fixture password";
        expected = UMI_STATUS_PERMISSION_DENIED;
        reads = 0U;
    } else if (strcmp(mode, "code-refused") == 0) {
        code = "00000000";
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "malformed-code") == 0) {
        code = "bad-code";
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "missing-credential") == 0) {
        fixture.readStatus = UMI_STATUS_NOT_FOUND;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "replay") == 0) {
        fixture.credential.replay = (UmiTotpReplay){.last_step = 1U, .has_last_step = true};
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "corrupt-record") == 0) {
        fixture.credential.secretBytes = sizeof fixture.credential.secret + 1U;
        expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(mode, "concurrent-use") == 0) {
        fixture.useStepFirst = 1;
        expected = UMI_STATUS_BUSY;
        commits = 1U;
    } else if (strcmp(mode, "rotation") == 0) {
        fixture.changeRevision = 1;
        expected = UMI_STATUS_BUSY;
        commits = 1U;
    } else if (strcmp(mode, "commit-io") == 0) {
        fixture.commitStatus = UMI_STATUS_IO_ERROR;
        expected = UMI_STATUS_IO_ERROR;
        commits = 1U;
    } else if (strcmp(mode, "provider-error") == 0) {
        backend.mac = UnavailableProvider;
        expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(mode, "registry-full") == 0) {
        UmiSessionRecord existing;
        for (size_t i = 0U; i < UMI_SESSION_REGISTRY_MAX; ++i)
            CHECK(umi_session_registry_issue(sessions, "independent", now, lifetime, &existing) == UMI_STATUS_OK);
        priorSessions = UMI_SESSION_REGISTRY_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
        commits = 1U;
    } else if (strcmp(mode, "invalid-clock") == 0) {
        now = UINT64_MAX;
        reads = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else {
        CHECK(strcmp(mode, "sign-in") == 0);
        commits = 1U;
    }

    UmiSessionRecord session;
    unsigned char unchanged[sizeof session];
    memset(&session, 0x76, sizeof session);
    memcpy(unchanged, &session, sizeof unchanged);
    const UmiStatus status = UmiLocalProfileMfaSignIn(profiles, sessions, &backend, "Classroom", password,
        code, strlen(code), now, lifetime, &session);
    CHECK(status == expected && fixture.reads == reads && fixture.commits == commits);
    if (status == UMI_STATUS_OK) {
        UmiSessionRecord verified;
        CHECK(fixture.durableWrites == 1U);
        CHECK(strcmp(session.principal_id, "classroom") == 0 && session.expires_at_ns == now + lifetime);
        CHECK(umi_session_registry_count(sessions) == 1U);
        CHECK(umi_session_registry_validate(sessions, session.session_id, now, &verified) == UMI_STATUS_OK);
        CHECK(strcmp(verified.principal_id, "classroom") == 0);
    } else {
        CHECK(memcmp(&session, unchanged, sizeof unchanged) == 0);
        CHECK(umi_session_registry_count(sessions) == priorSessions);
        CHECK(fixture.durableWrites == (priorSessions != 0U ? 1U : 0U));
    }
    if (priorSessions != 0U) {
        CHECK(UmiLocalProfileMfaSignIn(profiles, sessions, &backend, "classroom", password, code, strlen(code),
            now, lifetime, &session) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(fixture.durableWrites == 1U && fixture.commits == 1U && memcmp(&session, unchanged, sizeof unchanged) == 0);
    }
    umi_session_registry_destroy(sessions);
    UmiLocalProfileStoreRelease(profiles);
    return EXIT_SUCCESS;
}
