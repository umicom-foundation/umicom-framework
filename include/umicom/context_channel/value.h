/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/value.h
 *
 * PURPOSE:
 *   Represent named typed values carried by generic context payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_VALUE_H
#define UMICOM_CONTEXT_CHANNEL_VALUE_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context value data shared with callers of this public contract.
 */
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
/**
 * Initialise context value from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_value_init(UmiContextValue *value, const char *name);
/**
 * Provide the context value set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_text(UmiContextValue *value, const char *text);
/**
 * Provide the context value set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_integer(UmiContextValue *value, int64_t number);
/**
 * Provide the context value set unsigned operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_unsigned(UmiContextValue *value, uint64_t number);
/**
 * Provide the context value set decimal operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_decimal(UmiContextValue *value, double number);
/**
 * Provide the context value set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_boolean(UmiContextValue *value, bool state);
/**
 * Check that context value satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_value_validate(const UmiContextValue *value);
/**
 * Provide the context value equal operation used by this module and its client
 * applications.
 */
bool umi_context_value_equal(const UmiContextValue *left, const UmiContextValue *right);
#ifdef __cplusplus
}
#endif
#endif
