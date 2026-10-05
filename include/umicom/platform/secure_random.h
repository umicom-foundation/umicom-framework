/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/secure_random.h
 * PURPOSE: Obtain bounded random bytes from the operating system without process-global pseudo-random state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_SECURE_RANDOM_H
#define UMICOM_PLATFORM_SECURE_RANDOM_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_SECURE_RANDOM_MAXIMUM_BYTES (1024U * 1024U)
    /* Fill caller-owned output from the system random source. No seed, clock,
 * shared generator state or network request is used. A zero-size request
 * succeeds, including NULL output. Larger requests and missing nonempty
 * output are rejected before touching memory. On an operating-system failure,
 * valid bounded output is cleared; never use partially obtained bytes.
 * Calls can block while the system initializes its random source. Unsupported
 * targets return NOT_IMPLEMENTED. Success does not prove statistical quality;
 * the operating system owns the entropy contract. */
    UmiStatus UmiSecureRandomBytes(void *output, size_t bytes);
#ifdef __cplusplus
}
#endif
#endif
