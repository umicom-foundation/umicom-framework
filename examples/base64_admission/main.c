/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/base64_admission/main.c
 *
 * PURPOSE:
 *   Decode binary data only after full validation and demonstrate refusal without mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/base/base64.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    const char encoded[] = "AP/+";
    unsigned char bytes[3] = {7U, 8U, 9U};
    size_t required = 0U, count = 0U;
    UmiStatus status = UmiBase64DecodedSize(encoded, sizeof(encoded) - 1U, 0U, &required);
    if (status != UMI_STATUS_OK || required != sizeof(bytes)) return 1;
    status = UmiBase64Decode(encoded, sizeof(encoded) - 1U, 0U, bytes, sizeof(bytes), &count);
    if (status != UMI_STATUS_OK || count != sizeof(bytes)) return 1;
    printf("Decoded %zu bytes: %02x %02x %02x\n", count,
        (unsigned)bytes[0], (unsigned)bytes[1], (unsigned)bytes[2]);
    /* A caller commits decoded state only after success. Binary NUL is data,
     * so use count or memcmp rather than strlen on the output. */
    unsigned char saved[sizeof(bytes)];
    memcpy(saved, bytes, sizeof(bytes));
    size_t before = count;
    const char malformed[] = "AA==unexpected";
    status = UmiBase64Decode(malformed, sizeof(malformed) - 1U, 0U, bytes, sizeof(bytes), &count);
    if (status != UMI_STATUS_PARSE_ERROR || count != before ||
        memcmp(bytes, saved, sizeof(bytes)) != 0) return 1;
    puts("Malformed input refused; the previous bytes and count are unchanged.");
    return 0;
}
