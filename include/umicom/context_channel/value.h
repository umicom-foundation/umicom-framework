/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/value.h
 *
 * PURPOSE:
 *   Represent named typed values carried by generic context payloads.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_VALUE_H
#define UMICOM_CONTEXT_CHANNEL_VALUE_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextValue {
    uint32_t structure_size;
    UmiContextValueKind kind;
    char name[UMI_CONTEXT_TEXT_CAPACITY];
    char text[UMI_CONTEXT_VALUE_CAPACITY];
    int64_t integer_value;
    uint64_t unsigned_value;
    double decimal_value;
    bool boolean_value;
} UmiContextValue;
void umi_context_value_init(UmiContextValue *value, const char *name);
UmiStatus umi_context_value_set_text(UmiContextValue *value, const char *text);
UmiStatus umi_context_value_set_integer(UmiContextValue *value, int64_t number);
UmiStatus umi_context_value_set_unsigned(UmiContextValue *value, uint64_t number);
UmiStatus umi_context_value_set_decimal(UmiContextValue *value, double number);
UmiStatus umi_context_value_set_boolean(UmiContextValue *value, bool state);
UmiStatus umi_context_value_validate(const UmiContextValue *value);
bool umi_context_value_equal(const UmiContextValue *left, const UmiContextValue *right);
#ifdef __cplusplus
}
#endif
#endif
