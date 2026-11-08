/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/profile_name.h
 *
 * PURPOSE:
 *   Define portable local-profile naming without account storage or platform dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_PROFILE_NAME_H
#define UMICOM_SECURITY_PROFILE_NAME_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LOCAL_PROFILE_NAME_CAPACITY 49U

/* Names are case-insensitive ASCII: 3..48 letters, digits, '-' or '_', starting
 * with a letter. Output is lower-case and unchanged on validation failure. */
UmiStatus UmiLocalProfileName(const char *name, char out[UMI_LOCAL_PROFILE_NAME_CAPACITY]);
/* Validate an exact byte span without searching for a terminator. The caller
 * supplies length readable bytes; embedded NUL and non-ASCII bytes are invalid.
 * out must hold NAME_CAPACITY bytes. It may overlap name: validation finishes
 * before publication. Every output byte remains unchanged on failure. This is
 * name validation only; it does not authenticate a session or a broker account. */
UmiStatus UmiLocalProfileNameBytes(const char *name, size_t length,
                                 char out[UMI_LOCAL_PROFILE_NAME_CAPACITY]);

#ifdef __cplusplus
}
#endif
#endif
