/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/internal.h
 *
 * PURPOSE:
 *   Share bounded envelope validation between financial messaging services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_INTERNAL_H
#define UMICOM_FINANCIAL_MESSAGING_INTERNAL_H
#include "umicom/financial_messaging/envelope.h"
#include <string.h>
UmiStatus UmiUfmpEnvelopeSyntax(const UmiUfmpEnvelope *value);
#endif
