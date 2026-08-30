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
uint64_t umi_token_hash64(const void *data, size_t length);
UmiStatus umi_token_fingerprint(const void *data, size_t length,
                                char *out_text, size_t capacity);
int umi_token_constant_time_equal(const void *left, const void *right,
                                  size_t length);
void umi_secure_zero(void *data, size_t length);
#ifdef __cplusplus
}
#endif

#endif
