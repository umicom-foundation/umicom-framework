/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/totp.c
 *
 * PURPOSE:
 *   Share authenticator window, truncation and replay admission across clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/totp.h"
#include "umicom/security/secrets.h"

static UmiStatus match_step(const UmiTotpProfile *profile, const uint8_t *secret,
    size_t secret_size, uint64_t step, uint32_t supplied, bool *matches)
{
    uint8_t counter[UMI_TOTP_COUNTER_BYTES];
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES] = {0};
    for (size_t i = sizeof counter; i > 0U; --i) {
        counter[i - 1U] = (uint8_t)(step & UINT64_C(0xff));
        step >>= 8U;
    }
    const UmiStatus status = profile->mac(profile->mac_context, profile->algorithm,
        secret, secret_size, counter, digest);
    if (status == UMI_STATUS_OK) {
        /* RFC 4226 uses the low nibble of the last digest byte as the
         * truncation offset. Both admitted digest lengths cover that offset. */
        const size_t digest_size = profile->algorithm == UMI_TOTP_SHA1 ? 20U : 32U;
        const size_t offset = digest[digest_size - 1U] & 0x0fU;
        const uint32_t binary = ((uint32_t)(digest[offset] & 0x7fU) << 24U) |
            ((uint32_t)digest[offset + 1U] << 16U) |
            ((uint32_t)digest[offset + 2U] << 8U) | digest[offset + 3U];
        const uint32_t divisor = profile->digits == 6U ? UINT32_C(1000000) : UINT32_C(100000000);
        *matches = (binary % divisor) == supplied;
    }
    umi_secret_clear(digest, sizeof digest);
    return status;
}

UmiStatus UmiTotpVerify(const UmiTotpProfile *profile, const uint8_t *secret,
    size_t secret_size, const char *code, size_t code_size, uint64_t now_seconds,
    UmiTotpReplay *replay)
{
    if (profile == NULL || profile->mac == NULL || replay == NULL || secret == NULL ||
        secret_size < UMI_TOTP_MIN_SECRET_BYTES || secret_size > UMI_TOTP_MAX_SECRET_BYTES ||
        code == NULL || (profile->algorithm != UMI_TOTP_SHA1 && profile->algorithm != UMI_TOTP_SHA256) ||
        (profile->digits != 6U && profile->digits != 8U) || code_size != profile->digits)
        return UMI_STATUS_INVALID_ARGUMENT;

    uint32_t supplied = 0U;
    for (size_t i = 0U; i < code_size; ++i) {
        if (code[i] < '0' || code[i] > '9') return UMI_STATUS_INVALID_ARGUMENT;
        supplied = supplied * 10U + (uint32_t)(code[i] - '0');
    }

    /* Evaluate the whole admitted window before making the authorization
     * decision. A backend failure never consumes the replay step. */
    const uint64_t current_step = now_seconds / UMI_TOTP_PERIOD_SECONDS;
    bool current_match = false;
    bool previous_match = false;
    UmiStatus status = match_step(profile, secret, secret_size, current_step, supplied, &current_match);
    if (status != UMI_STATUS_OK) return status;
    if (current_step != 0U) {
        status = match_step(profile, secret, secret_size, current_step - 1U, supplied, &previous_match);
        if (status != UMI_STATUS_OK) return status;
    }
    if (!current_match && !previous_match) return UMI_STATUS_PERMISSION_DENIED;
    const uint64_t matched_step = current_match ? current_step : current_step - 1U;
    if (replay->has_last_step && matched_step <= replay->last_step) return UMI_STATUS_PERMISSION_DENIED;
    replay->last_step = matched_step;
    replay->has_last_step = true;
    return UMI_STATUS_OK;
}
