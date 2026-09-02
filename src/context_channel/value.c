/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/value.c
 *
 * PURPOSE:
 *   Implement typed context values with strict validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/value.h"
#include <math.h>
#include <string.h>
/*
 * Initialise context value from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_value_init(UmiContextValue *value, const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name != NULL) (void)umi_context_copy_text(value->name, sizeof(value->name), name);
}
/*
 * Provide the context value set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_text(UmiContextValue *value, const char *text)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(value->text, sizeof(value->text), text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->kind = UMI_CONTEXT_VALUE_TEXT;
    return status;
}
/*
 * Provide the context value set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_integer(UmiContextValue *value, int64_t number)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
value->integer_value=number;
value->kind=UMI_CONTEXT_VALUE_INTEGER;
return UMI_STATUS_OK;
}
/*
 * Provide the context value set unsigned operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_unsigned(UmiContextValue *value, uint64_t number)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
value->unsigned_value=number;
value->kind=UMI_CONTEXT_VALUE_UNSIGNED;
return UMI_STATUS_OK;
}
/*
 * Provide the context value set decimal operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_decimal(UmiContextValue *value, double number)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if (value == NULL || !isfinite(number)) return UMI_STATUS_INVALID_ARGUMENT;
value->decimal_value=number;
value->kind=UMI_CONTEXT_VALUE_DECIMAL;
return UMI_STATUS_OK;
}
/*
 * Provide the context value set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_boolean(UmiContextValue *value, bool state)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
value->boolean_value=state;
value->kind=UMI_CONTEXT_VALUE_BOOLEAN;
return UMI_STATUS_OK;
}
/* Check that context value satisfies its contract before another service relies on it. */
UmiStatus umi_context_value_validate(const UmiContextValue *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->kind < UMI_CONTEXT_VALUE_NONE || value->kind > UMI_CONTEXT_VALUE_BOOLEAN) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->kind == UMI_CONTEXT_VALUE_TEXT && !umi_context_text_is_valid(value->text,sizeof(value->text))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->kind == UMI_CONTEXT_VALUE_DECIMAL && !isfinite(value->decimal_value)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the context value equal operation used by this module and its client
 * applications.
 */
bool umi_context_value_equal(const UmiContextValue *left, const UmiContextValue *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || left->kind != right->kind || strcmp(left->name,right->name)!=0) return false;
    /* Select the behaviour associated with the requested command or state value. */
    switch (left->kind) {
    case UMI_CONTEXT_VALUE_NONE: return true;
    case UMI_CONTEXT_VALUE_TEXT: return strcmp(left->text,right->text)==0;
    case UMI_CONTEXT_VALUE_INTEGER: return left->integer_value==right->integer_value;
    case UMI_CONTEXT_VALUE_UNSIGNED: return left->unsigned_value==right->unsigned_value;
    case UMI_CONTEXT_VALUE_DECIMAL: return left->decimal_value==right->decimal_value;
    case UMI_CONTEXT_VALUE_BOOLEAN: return left->boolean_value==right->boolean_value;
    default: return false;
    
}
}
