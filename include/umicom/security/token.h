/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/token.h
 *
 * PURPOSE:
 *   Provide token fingerprinting, constant-time comparison and secure zeroing
 *   helpers for credential boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_TOKEN_H
#define UMICOM_SECURITY_TOKEN_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TOKEN_FINGERPRINT_CAPACITY 17U
/**
 * Provide the token hash64 operation used by this module and its client applications.
 */
uint64_t umi_token_hash64(const void *data, size_t length);
/**
 * Provide the token fingerprint operation used by this module and its client applications.
 */
UmiStatus umi_token_fingerprint(const void *data, size_t length,
                                char *out_text, size_t capacity);
/**
 * Provide the token constant time equal operation used by this module and its client
 * applications.
 */
int umi_token_constant_time_equal(const void *left, const void *right,
                                  size_t length);
/**
 * Provide the secure zero operation used by this module and its client applications.
 */
void umi_secure_zero(void *data, size_t length);
#ifdef __cplusplus
}
#endif

#endif
