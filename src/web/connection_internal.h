/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/connection_internal.h
 *
 * PURPOSE:
 *   Share bounded ASCII field checks between request framing and response serialization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_CONNECTION_INTERNAL_H
#define UMICOM_WEB_CONNECTION_INTERNAL_H
#include <stddef.h>
#include "umicom/web/connection.h"
/* HTTP field syntax is ASCII and must not depend on the process locale. */
int UmiWebFieldToken(unsigned char value);
int UmiWebFieldEqual(const unsigned char *bytes, size_t length, const char *word);
/* Admit a complete header block and report its expected message length.
 * Internal only: dispatch still requires the complete strict request. */
UmiStatus UmiWebRequestHeaderFrameRead(const void *bytes, size_t length, size_t maximum,
                                     UmiWebRequestFrame *out);
#endif
