/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/hmac_sha256.h
 *
 * PURPOSE:
 *   Authenticate caller-supplied bytes using HMAC-SHA-256, composing the
 *   Framework's canonical incremental SHA-256 rather than duplicating it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_HMAC_SHA256_H
#define UMICOM_SECURITY_HMAC_SHA256_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SECURITY_HMAC_SHA256_BYTES 32U

/* Standard RFC 2104 HMAC with SHA-256, for binary keys and messages.
 * Requires a nonempty key and valid message storage (NULL with zero bytes is
 * valid). The caller owns the key and is responsible for secret provisioning,
 * storage, rotation and transport security. Outputs are unchanged on errors.
 * This is NOT encryption, proof of model quality or a public-key signature. */
UmiStatus UmiSecurityHmacSha256Compute(
    const uint8_t *key, size_t keyBytes,
    const uint8_t *data, size_t dataBytes,
    uint8_t outMac[UMI_SECURITY_HMAC_SHA256_BYTES]);

/* Constant-work comparison of fixed-size MAC bytes, after a fresh HMAC.
 * An incorrect MAC returns PERMISSION_DENIED, without publishing a result.
 * Timing still depends on message/key length and underlying SHA operation. */
UmiStatus UmiSecurityHmacSha256Verify(
    const uint8_t *key, size_t keyBytes,
    const uint8_t *data, size_t dataBytes,
    const uint8_t claimedMac[UMI_SECURITY_HMAC_SHA256_BYTES]);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_SECURITY_HMAC_SHA256_H */
