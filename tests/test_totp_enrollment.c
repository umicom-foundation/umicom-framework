/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_totp_enrollment.c
 *
 * PURPOSE:
 *   Verify portable authenticator provisioning and known-code interoperability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/totp_enrollment.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static const uint8_t key_sha1[] = "12345678901234567890";
static const uint8_t key_sha256[] = "12345678901234567890123456789012";

static int Standard(void)
{
    char uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    size_t length = 0U;
    CHECK(UmiTotpBuildEnrollmentUri("Umicom", "practice", UMI_TOTP_SHA1, 6U, key_sha1,
        sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_OK);
    static const char expected[] =
        "otpauth://totp/Umicom:practice?secret=GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ"
        "&issuer=Umicom&algorithm=SHA1&digits=6&period=30";
    CHECK(length == sizeof expected - 1U && strcmp(uri, expected) == 0);
    return 0;
}

static int Escaped(void)
{
    char uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    size_t length = 0U;
    CHECK(UmiTotpBuildEnrollmentUri("Umicom Lab + QA", "Ren\xc3\xa9" "e@example.test?view=1&trace=2",
        UMI_TOTP_SHA256, 8U, key_sha256, sizeof key_sha256 - 1U, uri, sizeof uri, &length) == UMI_STATUS_OK);
    static const char expected[] =
        "otpauth://totp/Umicom%20Lab%20%2B%20QA:Ren%C3%A9e%40example.test%3Fview%3D1%26trace%3D2"
        "?secret=GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZA"
        "&issuer=Umicom%20Lab%20%2B%20QA&algorithm=SHA256&digits=8&period=30";
    CHECK(length == sizeof expected - 1U && strcmp(uri, expected) == 0);
    return 0;
}

static int Capacity(void)
{
    char uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    memset(uri, 'x', sizeof uri);
    char before[sizeof uri];
    memcpy(before, uri, sizeof before);
    size_t length = 0U;
    char expected[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    CHECK(UmiTotpBuildEnrollmentUri("Umicom", "practice", UMI_TOTP_SHA1, 6U,
        key_sha1, sizeof key_sha1 - 1U, expected, sizeof expected, &length) == UMI_STATUS_OK);
    size_t reported = 5U;
    CHECK(UmiTotpBuildEnrollmentUri("Umicom", "practice", UMI_TOTP_SHA1, 6U,
        key_sha1, sizeof key_sha1 - 1U, uri, length, &reported) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(reported == 5U && memcmp(uri, before, sizeof uri) == 0);
    CHECK(UmiTotpBuildEnrollmentUri("Umicom", "practice", UMI_TOTP_SHA1, 6U,
        key_sha1, sizeof key_sha1 - 1U, uri, length + 1U, &reported) == UMI_STATUS_OK);
    CHECK(reported == length && strcmp(uri, expected) == 0 && uri[length + 1U] == 'x');
    return 0;
}

static int TailAndMaximum(void)
{
    uint8_t secret[UMI_TOTP_MAX_SECRET_BYTES] = {0};
    memcpy(secret, key_sha1, sizeof key_sha1 - 1U);
    secret[sizeof key_sha1 - 1U] = 255U;
    char uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    size_t length = 0U;
    CHECK(UmiTotpBuildEnrollmentUri("Umicom", "practice", UMI_TOTP_SHA1, 6U,
        secret, sizeof key_sha1, uri, sizeof uri, &length) == UMI_STATUS_OK);
    CHECK(strstr(uri, "?secret=GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ74&issuer=") != NULL);
    char label[UMI_TOTP_ENROLLMENT_LABEL_LIMIT + 1U];
    memset(label, '?', sizeof label - 1U);
    label[sizeof label - 1U] = '\0';
    CHECK(UmiTotpBuildEnrollmentUri(label, label, UMI_TOTP_SHA256, 8U,
        secret, sizeof secret, uri, sizeof uri, &length) == UMI_STATUS_OK);
    CHECK(length > 1250U && length < sizeof uri && uri[length] == '\0');
    return 0;
}

static int Invalid(const char *mode)
{
    char uri[] = "untouched";
    size_t length = 98U;
    if (strcmp(mode, "text") == 0) {
        const char *const invalid_labels[] = {"", "Practice:QA", "Bad\nName", "\xc0\x80", "\x7f"};
        for (size_t i = 0U; i < sizeof invalid_labels / sizeof invalid_labels[0]; ++i) {
            CHECK(UmiTotpBuildEnrollmentUri(invalid_labels[i], "account", UMI_TOTP_SHA1, 6U,
                key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiTotpBuildEnrollmentUri("Umicom", invalid_labels[i], UMI_TOTP_SHA1, 6U,
                key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
        }
        char large[UMI_TOTP_ENROLLMENT_LABEL_LIMIT + 2U];
        memset(large, 'x', sizeof large - 1U);
        large[sizeof large - 1U] = '\0';
        CHECK(UmiTotpBuildEnrollmentUri("Umicom", large, UMI_TOTP_SHA1, 6U,
            key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiTotpBuildEnrollmentUri(NULL, "account", UMI_TOTP_SHA1, 6U,
            key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        CHECK(strcmp(mode, "parameters") == 0);
        CHECK(UmiTotpBuildEnrollmentUri("Umicom", "account", (UmiTotpAlgorithm)9, 6U,
            key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTotpBuildEnrollmentUri("Umicom", "account", UMI_TOTP_SHA1, 7U,
            key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTotpBuildEnrollmentUri("Umicom", "account", UMI_TOTP_SHA1, 6U,
            key_sha1, UMI_TOTP_MIN_SECRET_BYTES - 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTotpBuildEnrollmentUri("Umicom", "account", UMI_TOTP_SHA1, 6U,
            key_sha1, UMI_TOTP_MAX_SECRET_BYTES + 1U, uri, sizeof uri, &length) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTotpBuildEnrollmentUri("Umicom", "account", UMI_TOTP_SHA1, 6U,
            key_sha1, sizeof key_sha1 - 1U, uri, sizeof uri, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(length == 98U && strcmp(uri, "untouched") == 0);
    return 0;
}

static int VerifyProvisioned(void)
{
    char uri[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    size_t length = 0U;
    CHECK(UmiTotpBuildEnrollmentUri("Umicom", "practice", UMI_TOTP_SHA256, 8U,
        key_sha256, sizeof key_sha256 - 1U, uri, sizeof uri, &length) == UMI_STATUS_OK);
    const char *encoded = strstr(uri, "?secret=");
    CHECK(encoded != NULL);
    encoded += strlen("?secret=");
    const char *end = strchr(encoded, '&');
    CHECK(end != NULL && (size_t)(end - encoded) == 52U);
    /* The fixture decodes the successful URI back into a key before testing
     * the RFC authenticator code. This is independent of the encoder. */
    uint8_t decoded[32];
    size_t decoded_length = 0U;
    unsigned bits = 0U;
    uint32_t accumulator = 0U;
    for (const char *character = encoded; character < end; ++character) {
        CHECK((*character >= 'A' && *character <= 'Z') || (*character >= '2' && *character <= '7'));
        const unsigned value = *character >= 'A' ? (unsigned)(*character - 'A') :
            26U + (unsigned)(*character - '2');
        accumulator = (accumulator << 5U) | value;
        bits += 5U;
        if (bits >= 8U) {
            bits -= 8U;
            CHECK(decoded_length < sizeof decoded);
            decoded[decoded_length++] = (uint8_t)(accumulator >> bits);
        }
    }
    CHECK(decoded_length == sizeof decoded && memcmp(decoded, key_sha256, sizeof decoded) == 0);
    UmiTotpProfile profile = {UmiTotpPlatformMac, NULL, UMI_TOTP_SHA256, 8U};
    UmiTotpReplay replay = {0};
    const UmiStatus status = UmiTotpVerify(&profile, decoded, sizeof decoded, "46119246", 8U, 59U, &replay);
    if (status == UMI_STATUS_NOT_IMPLEMENTED) {
        puts("The independent code check requires the Windows CNG platform provider.");
        return 77;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiTotpVerify(&profile, decoded, sizeof decoded, "46119246", 8U, 59U, &replay) == UMI_STATUS_PERMISSION_DENIED);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strcmp(argv[1], "standard") == 0) return Standard();
    if (strcmp(argv[1], "escaped") == 0) return Escaped();
    if (strcmp(argv[1], "capacity") == 0) return Capacity();
    if (strcmp(argv[1], "tail-and-maximum") == 0) return TailAndMaximum();
    if (strcmp(argv[1], "verify-provisioned") == 0) return VerifyProvisioned();
    return Invalid(argv[1]);
}
