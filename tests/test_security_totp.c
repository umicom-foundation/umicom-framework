/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_totp.c
 *
 * PURPOSE:
 *   Verify standard TOTP codes, replay refusal and cryptography error handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/security/totp.h"

#define CHECK_TOTP(condition) do { \
    if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); return 1; } \
} while (0)

/* These fixed secrets are published test vectors. Keep them inside the test. */
static const uint8_t sha1_key[] = "12345678901234567890";
static const uint8_t sha256_key[] = "12345678901234567890123456789012";

static int vectors(void)
{
#ifdef _WIN32
    static const struct {
        uint64_t time;
        const char *sha1_code;
        const char *sha256_code;
    } expected[] = {
        {UINT64_C(59), "94287082", "46119246"},
        {UINT64_C(1111111109), "07081804", "68084774"},
        {UINT64_C(1111111111), "14050471", "67062674"},
        {UINT64_C(1234567890), "89005924", "91819424"},
        {UINT64_C(2000000000), "69279037", "90698825"},
        {UINT64_C(20000000000), "65353130", "77737706"}
    };
    const UmiTotpProfile sha1 = {.mac = UmiTotpPlatformMac, .algorithm = UMI_TOTP_SHA1, .digits = 8U};
    const UmiTotpProfile sha256 = {.mac = UmiTotpPlatformMac, .algorithm = UMI_TOTP_SHA256, .digits = 8U};
    UmiTotpReplay a = {0};
    UmiTotpReplay b = {0};
    for (size_t i = 0U; i < sizeof expected / sizeof expected[0]; ++i) {
        CHECK_TOTP(UmiTotpVerify(&sha1, sha1_key, sizeof sha1_key - 1U, expected[i].sha1_code,
            8U, expected[i].time, &a) == UMI_STATUS_OK);
        CHECK_TOTP(UmiTotpVerify(&sha256, sha256_key, sizeof sha256_key - 1U, expected[i].sha256_code,
            8U, expected[i].time, &b) == UMI_STATUS_OK);
        CHECK_TOTP(a.last_step == expected[i].time / UMI_TOTP_PERIOD_SECONDS && a.has_last_step);
        CHECK_TOTP(b.last_step == a.last_step && b.has_last_step);
    }
    return 0;
#else
    (void)sha256_key;
    return 77;
#endif
}

typedef struct MacFixture {
    uint64_t seen[2];
    size_t calls;
    size_t fail_call;
} MacFixture;

static UmiStatus fixed_mac(void *context, UmiTotpAlgorithm algorithm, const uint8_t *secret,
    size_t secret_size, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES])
{
    MacFixture *fixture = context;
    if (fixture->calls >= 2U || algorithm != UMI_TOTP_SHA1 ||
        secret != sha1_key || secret_size != sizeof sha1_key - 1U)
        return UMI_STATUS_INVALID_STATE;
    uint64_t step = 0U;
    for (size_t i = 0U; i < UMI_TOTP_COUNTER_BYTES; ++i)
        step = (step << 8U) | counter[i];
    fixture->seen[fixture->calls++] = step;
    if (fixture->calls == fixture->fail_call) return UMI_STATUS_UNAVAILABLE;
    /* The offset is seven. Encode a different test value for every step. */
    memset(digest, 0, UMI_TOTP_MAX_DIGEST_BYTES);
    digest[10] = (uint8_t)(step & UINT64_C(0xff));
    digest[19] = 7U;
    return UMI_STATUS_OK;
}

static int window(void)
{
    MacFixture fixture = {0};
    const UmiTotpProfile profile = {.mac = fixed_mac, .mac_context = &fixture, .algorithm = UMI_TOTP_SHA1, .digits = 6U};
    UmiTotpReplay replay = {0};
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "000002", 6U, 89U, &replay)
        == UMI_STATUS_OK);
    CHECK_TOTP(replay.has_last_step && replay.last_step == 2U && fixture.calls == 2U);
    CHECK_TOTP(fixture.seen[0] == 2U && fixture.seen[1] == 1U);

    fixture.calls = 0U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "000004", 6U, 90U, &replay)
        == UMI_STATUS_PERMISSION_DENIED);
    CHECK_TOTP(replay.last_step == 2U);
    fixture.calls = 0U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "000002", 6U, 90U, &replay)
        == UMI_STATUS_PERMISSION_DENIED);
    CHECK_TOTP(replay.last_step == 2U && fixture.calls == 2U);
    fixture.calls = 0U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "000003", 6U, 120U, &replay)
        == UMI_STATUS_OK);
    CHECK_TOTP(replay.last_step == 3U);

    replay = (UmiTotpReplay){0};
    fixture.calls = 0U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "000000", 6U, 0U, &replay)
        == UMI_STATUS_OK);
    CHECK_TOTP(replay.has_last_step && replay.last_step == 0U && fixture.calls == 1U);
    fixture.calls = 0U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "000000", 6U, 29U, &replay)
        == UMI_STATUS_PERMISSION_DENIED);
    CHECK_TOTP(replay.has_last_step && replay.last_step == 0U);
    return 0;
}

static int boundaries(void)
{
    MacFixture fixture = {0};
    UmiTotpProfile profile = {.mac = fixed_mac, .mac_context = &fixture, .algorithm = UMI_TOTP_SHA1, .digits = 6U};
    UmiTotpReplay replay = {.last_step = 1U, .has_last_step = true};
    const char supplied[] = {'0', '0', '0', '0', '0', '2'};
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, 19U, supplied, sizeof supplied, 60U, &replay)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, supplied, 5U, 60U, &replay)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, "00002 ", 6U, 60U, &replay)
        == UMI_STATUS_INVALID_ARGUMENT);
    profile.algorithm = (UmiTotpAlgorithm)99;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, supplied, sizeof supplied, 60U, &replay)
        == UMI_STATUS_INVALID_ARGUMENT);
    profile.algorithm = UMI_TOTP_SHA1;
    CHECK_TOTP(fixture.calls == 0U && replay.last_step == 1U && replay.has_last_step);

    fixture.fail_call = 2U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, supplied, sizeof supplied, 60U, &replay)
        == UMI_STATUS_UNAVAILABLE);
    CHECK_TOTP(fixture.calls == 2U && replay.last_step == 1U && replay.has_last_step);
    fixture.calls = 0U;
    fixture.fail_call = 0U;
    CHECK_TOTP(UmiTotpVerify(&profile, sha1_key, sizeof sha1_key - 1U, supplied, sizeof supplied, 60U, &replay)
        == UMI_STATUS_OK);
    CHECK_TOTP(replay.last_step == 2U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "vectors")) return vectors();
    if (!strcmp(argv[1], "window")) return window();
    if (!strcmp(argv[1], "boundaries")) return boundaries();
    return 2;
}
