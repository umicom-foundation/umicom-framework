/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_numbers.c
 * PURPOSE: Preserve broker decimal text when a value cannot be represented exactly.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "order_numbers.h"
#include <string.h>
bool UmiIbkrOrderNumberRead(const char *text, bool optional, bool nonnegative, UmiIbkrOrderNumber *out)
{
    if (!out || !UmiIbkrText(text, sizeof out->reportedText, optional))
        return false;
    if (*text && (!UmiIbkrDecimalText(text) || (nonnegative && text[0] == '-')))
        return false;
    UmiIbkrOrderNumber copy = {0};
    strcpy(copy.reportedText, text);
    if (*text)
        copy.exact = UmiDecimalParseScientificExact(text, strlen(text), &copy.value) == UMI_STATUS_OK;
    *out = copy;
    return true;
}
