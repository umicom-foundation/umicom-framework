/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/totp_enrollment.c
 *
 * PURPOSE:
 *   Share bounded authenticator provisioning across local and web clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/totp_enrollment.h"

#include "umicom/base/utf8.h"
#include "umicom/security/secrets.h"
#include <stdio.h>
#include <string.h>

/* Both label fields and the issuer query can expand to three bytes per input
 * byte. Reserve additional space for the Base32 secret and fixed parameters. */
_Static_assert(UMI_TOTP_ENROLLMENT_URI_CAPACITY >=
    9U * UMI_TOTP_ENROLLMENT_LABEL_LIMIT + (8U * UMI_TOTP_MAX_SECRET_BYTES + 4U) / 5U + 128U,
    "Enrollment staging must admit the longest encoded secret and labels");

static UmiStatus LabelLength(const char *text, size_t *length)
{
    if (text == NULL || text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t size = 0U;
    while (size <= UMI_TOTP_ENROLLMENT_LABEL_LIMIT && text[size] != '\0') {
        const unsigned char byte = (unsigned char)text[size];
        if (byte < 0x20U || byte == 0x7fU || byte == ':') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        ++size;
    }
    if (size > UMI_TOTP_ENROLLMENT_LABEL_LIMIT) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!UmiUtf8Validate((const unsigned char *)text, size, NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *length = size;
    return UMI_STATUS_OK;
}

static char *Append(char *out, const char *text)
{
    const size_t length = strlen(text);
    memcpy(out, text, length);
    return out + length;
}

static char *EncodeLabel(char *out, const char *text, size_t length)
{
    static const char hex[] = "0123456789ABCDEF";
    for (size_t i = 0U; i < length; ++i) {
        const unsigned char byte = (unsigned char)text[i];
        if ((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') ||
            (byte >= '0' && byte <= '9') || byte == '-' || byte == '.' || byte == '_' || byte == '~') {
            *out++ = (char)byte;
        } else {
            *out++ = '%';
            *out++ = hex[byte >> 4U];
            *out++ = hex[byte & 0x0fU];
        }
    }
    return out;
}

static char *EncodeSecret(char *out, const uint8_t *secret, size_t length)
{
    static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    uint32_t accumulator = 0U;
    unsigned bits = 0U;
    for (size_t i = 0U; i < length; ++i) {
        accumulator = (accumulator << 8U) | secret[i];
        bits += 8U;
        while (bits >= 5U) {
            bits -= 5U;
            *out++ = base32[(accumulator >> bits) & 31U];
        }
    }
    if (bits != 0U) {
        *out++ = base32[(accumulator << (5U - bits)) & 31U];
    }
    return out;
}

UmiStatus UmiTotpBuildEnrollmentUri(const char *issuer, const char *account,
    UmiTotpAlgorithm algorithm, unsigned digits, const uint8_t *secret,
    size_t secret_size, char *uri, size_t uri_capacity, size_t *uri_size)
{
    if (uri == NULL || uri_size == NULL || secret == NULL ||
        secret_size < UMI_TOTP_MIN_SECRET_BYTES || secret_size > UMI_TOTP_MAX_SECRET_BYTES ||
        (algorithm != UMI_TOTP_SHA1 && algorithm != UMI_TOTP_SHA256) ||
        (digits != 6U && digits != 8U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t issuer_size = 0U, account_size = 0U;
    UmiStatus status = LabelLength(issuer, &issuer_size);
    if (status == UMI_STATUS_OK) {
        status = LabelLength(account, &account_size);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Publish only a complete URI and erase the private copy on every exit.
     * The shared period constant keeps enrollment aligned with verification. */
    char staged[UMI_TOTP_ENROLLMENT_URI_CAPACITY];
    char *next = Append(staged, "otpauth://totp/");
    next = EncodeLabel(next, issuer, issuer_size);
    *next++ = ':';
    next = EncodeLabel(next, account, account_size);
    next = Append(next, "?secret=");
    next = EncodeSecret(next, secret, secret_size);
    next = Append(next, "&issuer=");
    next = EncodeLabel(next, issuer, issuer_size);
    const size_t written = (size_t)(next - staged);
    const size_t remaining = sizeof staged - written;
    const int parameters = snprintf(next, remaining, "&algorithm=%s&digits=%u&period=%u",
        algorithm == UMI_TOTP_SHA1 ? "SHA1" : "SHA256", digits, UMI_TOTP_PERIOD_SECONDS);
    if (parameters < 0 || (size_t)parameters >= remaining) {
        status = UMI_STATUS_INTERNAL_ERROR;
    } else {
        const size_t complete = written + (size_t)parameters;
        if (uri_capacity <= complete) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        } else {
            memcpy(uri, staged, complete + 1U);
            *uri_size = complete;
        }
    }
    umi_secret_clear(staged, sizeof staged);
    return status;
}
