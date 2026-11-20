/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/totp.h
 *
 * PURPOSE:
 *   Verify time-based authenticator codes through a caller-owned replay record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_TOTP_H
#define UMICOM_SECURITY_TOTP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOTP_COUNTER_BYTES 8U
#define UMI_TOTP_MAX_DIGEST_BYTES 32U
#define UMI_TOTP_MIN_SECRET_BYTES 20U
#define UMI_TOTP_MAX_SECRET_BYTES 64U
#define UMI_TOTP_PERIOD_SECONDS 30U

typedef enum UmiTotpAlgorithm {
    UMI_TOTP_SHA1 = 1,
    UMI_TOTP_SHA256
} UmiTotpAlgorithm;

/* A successful backend writes 20 SHA-1 or 32 SHA-256 digest bytes.
 * It receives the RFC counter as eight big-endian bytes. */
typedef UmiStatus (*UmiTotpMac)(void *context, UmiTotpAlgorithm algorithm,
    const uint8_t *secret, size_t secret_size, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES]);

typedef struct UmiTotpProfile {
    UmiTotpMac mac;
    void *mac_context;
    UmiTotpAlgorithm algorithm;
    unsigned digits;
} UmiTotpProfile;

typedef struct UmiTotpReplay {
    uint64_t last_step;
    bool has_last_step;
} UmiTotpReplay;

/** Verify an exact six- or eight-digit ASCII code using Unix time in seconds.
 * The accepted window is the current step and one previous 30-second step.
 * Initialize a new credential's replay record to zero. Success advances it
 * to a newer matched step and failures leave it unchanged. Nonmatching and
 * replayed codes both return PERMISSION_DENIED.
 *
 * The caller owns the secret, throttling and the replay transaction. It must
 * serialize verification per credential and durably commit the updated record
 * before granting a session. Input and replay storage must be independent.
 * Treat SHA-1 as an authenticator compatibility choice for keyed TOTP only. */
UmiStatus UmiTotpVerify(const UmiTotpProfile *profile, const uint8_t *secret,
    size_t secret_size, const char *code, size_t code_size, uint64_t now_seconds,
    UmiTotpReplay *replay);

/* Supply Windows CNG hashing to the verifier. Other hosts can provide their
 * OS crypto implementation through UmiTotpMac. The Windows provider ignores
 * context. Backends return no code, key or digest in a diagnostic. */
UmiStatus UmiTotpPlatformMac(void *context, UmiTotpAlgorithm algorithm,
    const uint8_t *secret, size_t secret_size, const uint8_t counter[UMI_TOTP_COUNTER_BYTES],
    uint8_t digest[UMI_TOTP_MAX_DIGEST_BYTES]);

#ifdef __cplusplus
}
#endif

#endif
