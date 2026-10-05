/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/connection_internal.h
 * PURPOSE: Share bounded ASCII field checks between request framing and response serialization.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_CONNECTION_INTERNAL_H
#define UMICOM_WEB_CONNECTION_INTERNAL_H
#include <stddef.h>
/* HTTP field syntax is ASCII and must not depend on the process locale. */
int UmiWebFieldToken(unsigned char value);
int UmiWebFieldEqual(const unsigned char *bytes, size_t length, const char *word);
#endif
