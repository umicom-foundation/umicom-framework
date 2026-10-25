/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/inbox_internal.h
 *
 * PURPOSE:
 *   Share private bounded journal encoding without exporting a network format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UFMP_INBOX_INTERNAL_H
#define UMICOM_UFMP_INBOX_INTERNAL_H
#include "umicom/financial_messaging/inbox_store.h"
#include <string.h>
#define UMI_UFMP_INBOX_TEXT_CAPACITY 4096U
#define UMI_UFMP_INBOX_KEY_CAPACITY 192U
UmiStatus UmiUfmpInboxEncode(const UmiUfmpInboxRecord *record,
                             char out[UMI_UFMP_INBOX_TEXT_CAPACITY]);
UmiStatus UmiUfmpInboxDecode(char text[UMI_UFMP_INBOX_TEXT_CAPACITY],
                             UmiUfmpInboxRecord *out_record);
void UmiUfmpInboxHex(const unsigned char *bytes, size_t length, char *out);
int UmiUfmpInboxReceiptValid(const char *text);
#endif
