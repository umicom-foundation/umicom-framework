/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/money_text.c
 *
 * PURPOSE:
 *   Place the sign and decimal point using bounded integer digit operations.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/money_text.h"
#include <stdint.h>
#include <string.h>

/* Formatting belongs to Framework: views must not each implement signed
 * remainders, powers of ten or their own rounding policy. Building the entire
 * result first also makes an undersized output fail without partial money. */
UmiStatus UmiMoneyTextFormat(const UmiMoney *money, char *output,
    size_t capacity, size_t *requiredCapacity)
{
    char digits[20];
    char text[UMI_MONEY_TEXT_CAPACITY];
    size_t digitCount = 0U, length = 0U;
    uint64_t magnitude;
    size_t scale;
    if (requiredCapacity != NULL) *requiredCapacity = 0U;
    if (money == NULL || (output == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < 3U; ++index)
        if (money->currency.code[index] < 'A' || money->currency.code[index] > 'Z')
            return UMI_STATUS_INVALID_ARGUMENT;
    if (money->currency.code[3] != '\0') return UMI_STATUS_INVALID_ARGUMENT;
    scale = (size_t)money->scale;
    /* -(INT64_MIN) is not representable as signed int64_t. Move the final
     * unit into unsigned arithmetic rather than invoking signed overflow. */
    magnitude = money->minor_units < 0
        ? (uint64_t)(-(money->minor_units + INT64_C(1))) + UINT64_C(1)
        : (uint64_t)money->minor_units;
    do {
        digits[digitCount++] = (char)('0' + (int)(magnitude % UINT64_C(10)));
        magnitude /= UINT64_C(10);
    } while (magnitude != 0U);
    memcpy(text, money->currency.code, 3U); length = 3U;
    text[length++] = ' ';
    if (money->minor_units < 0) text[length++] = '-';
    if (scale >= digitCount) {
        text[length++] = '0';
        text[length++] = '.';
        for (size_t index = digitCount; index < scale; ++index) text[length++] = '0';
        for (size_t index = digitCount; index > 0U; --index) text[length++] = digits[index - 1U];
    } else {
        for (size_t index = digitCount; index > 0U; --index) {
            text[length++] = digits[index - 1U];
            if (scale != 0U && index == scale + 1U) text[length++] = '.';
        }
    }
    text[length] = '\0';
    if (requiredCapacity != NULL) *requiredCapacity = length + 1U;
    if (output == NULL) return UMI_STATUS_OK;
    if (capacity < length + 1U) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, length + 1U);
    return UMI_STATUS_OK;
}
