/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/totp_enrollment.h
 *
 * PURPOSE:
 *   Build an authenticator enrollment URI from a caller-owned TOTP secret.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_TOTP_ENROLLMENT_H
#define UMICOM_SECURITY_TOTP_ENROLLMENT_H

#include "umicom/security/totp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOTP_ENROLLMENT_LABEL_LIMIT 128U
#define UMI_TOTP_ENROLLMENT_URI_CAPACITY 1400U

/* Build a TOTP Key URI using the verifier's algorithms, code widths, secret
 * size and period. Issuer and account are nonempty UTF-8 text, at most 128
 * bytes each, with no ASCII controls or colon. Both are percent encoded.
 * The secret uses unpadded RFC 4648 Base32.
 *
 * All inputs and outputs are separate. Success writes a terminated URI and
 * reports its length without the terminator. Refusal preserves both outputs.
 * The caller already owns a securely generated secret. Protect the resulting
 * URI as that secret and clear it after authorized enrollment display.
 * This routine performs no persistence, credential activation or I/O. */
UmiStatus UmiTotpBuildEnrollmentUri(const char *issuer, const char *account,
    UmiTotpAlgorithm algorithm, unsigned digits, const uint8_t *secret,
    size_t secret_size, char *uri, size_t uri_capacity, size_t *uri_size);

#ifdef __cplusplus
}
#endif

#endif
