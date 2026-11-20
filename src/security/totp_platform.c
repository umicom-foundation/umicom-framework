/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/totp_platform.c
 *
 * PURPOSE:
 *   Use Windows CNG to authenticate time-step messages without owning the key.
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
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#endif

UmiStatus UmiTotpPlatformMac(void *context, UmiTotpAlgorithm algorithm,
    const uint8_t *secret, size_t secret_size, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES])
{
    (void)context;
    if (secret == NULL || secret_size < UMI_TOTP_MIN_SECRET_BYTES ||
        secret_size > UMI_TOTP_MAX_SECRET_BYTES || counter == NULL || digest == NULL ||
        (algorithm != UMI_TOTP_SHA1 && algorithm != UMI_TOTP_SHA256))
        return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    BCRYPT_ALG_HANDLE provider = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    uint8_t candidate[UMI_TOTP_MAX_DIGEST_BYTES] = {0};
    const ULONG digest_size = algorithm == UMI_TOTP_SHA1 ? 20U : 32U;
    const wchar_t *algorithm_name = algorithm == UMI_TOTP_SHA1 ? BCRYPT_SHA1_ALGORITHM : BCRYPT_SHA256_ALGORITHM;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&provider, algorithm_name, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (status >= 0)
        status = BCryptCreateHash(provider, &hash, NULL, 0U, (PUCHAR)(const void *)secret,
            (ULONG)secret_size, 0U);
    if (status >= 0)
        status = BCryptHashData(hash, (PUCHAR)(const void *)counter, UMI_TOTP_COUNTER_BYTES, 0U);
    if (status >= 0) status = BCryptFinishHash(hash, candidate, digest_size, 0U);
    if (hash != NULL) (void)BCryptDestroyHash(hash);
    if (provider != NULL) (void)BCryptCloseAlgorithmProvider(provider, 0U);
    if (status >= 0) memcpy(digest, candidate, sizeof candidate);
    umi_secret_clear(candidate, sizeof candidate);
    return status >= 0 ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
#else
    /* Platform selection stays explicit so a host without an adapter cannot
     * accidentally grant a second-factor result. */
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
