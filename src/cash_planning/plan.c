/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/plan.c
 * PURPOSE: Validate and own reusable cash assumptions without changing operational balances.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "plan_private.h"
#include "umicom/finance/currency.h"
#include "umicom/document/text_encoding.h"
#include <stdlib.h>
#include <string.h>
/* Fixed fields cross application and file boundaries. Validate termination
 * before UTF-8 or string comparison so malformed records cannot overread. */
static int TextValid(const char *text, size_t capacity)
{
    size_t length = 0U;
    while (length < capacity && text[length] != '\0')
        ++length;
    if (length == 0U || length == capacity ||
        !umi_document_utf8_validate((const unsigned char *)text, length, NULL))
        return 0;
    for (size_t i = 0U; i < length; ++i)
        if ((unsigned char)text[i] < 32U || (unsigned char)text[i] == 127U)
            return 0;
    return 1;
}
static int MoneyValid(UmiMoney money) { return money.scale <= 9U && umi_currency_valid(&money.currency); }
static int Compatible(UmiMoney a, UmiMoney b)
{
    return a.scale == b.scale && memcmp(a.currency.code, b.currency.code, 4U) == 0;
}
static int ConfigValid(const UmiCashPlanConfig *config)
{
    return config != NULL && TextValid(config->title, sizeof config->title) && MoneyValid(config->opening) &&
           config->buffer_minor >= 0 && umi_financial_date_is_valid(config->start) &&
           umi_financial_date_is_valid(config->end) &&
           umi_financial_date_compare(config->start, config->end) <= 0;
}
static UmiStatus EntryValid(const UmiCashPlan *plan, const UmiCashPlanEntry *entry, size_t replaceIndex)
{
    if (entry == NULL || !TextValid(entry->id, sizeof entry->id) ||
        !TextValid(entry->label, sizeof entry->label) || !MoneyValid(entry->amount) ||
        !Compatible(plan->config.opening, entry->amount) || entry->amount.minor_units < 0 ||
        !umi_financial_date_is_valid(entry->date) ||
        (entry->direction != UMI_FINANCIAL_DIRECTION_PAY &&
         entry->direction != UMI_FINANCIAL_DIRECTION_RECEIVE) ||
        (entry->enabled != 0 && entry->enabled != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < plan->count; ++i)
        if (i != replaceIndex && strcmp(plan->entries[i].id, entry->id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanCreate(const UmiCashPlanConfig *config, UmiCashPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!ConfigValid(config))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCashPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    plan->config = *config;
    plan->revision = 1U;
    *out = plan;
    return UMI_STATUS_OK;
}
void UmiCashPlanDestroy(UmiCashPlan *plan) { free(plan); }
UmiStatus UmiCashPlanCopy(const UmiCashPlan *plan, UmiCashPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCashPlan *copy = malloc(sizeof *copy);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    *copy = *plan;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanRead(const UmiCashPlan *plan, UmiCashPlanConfig *out)
{
    if (plan == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = plan->config;
    return UMI_STATUS_OK;
}
size_t UmiCashPlanCount(const UmiCashPlan *plan) { return plan == NULL ? 0U : plan->count; }
uint64_t UmiCashPlanRevision(const UmiCashPlan *plan) { return plan == NULL ? 0U : plan->revision; }
UmiStatus UmiCashPlanSetConfig(UmiCashPlan *plan, const UmiCashPlanConfig *config)
{
    if (plan == NULL || !ConfigValid(config))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < plan->count; ++i)
        if (!Compatible(config->opening, plan->entries[i].amount))
            return UMI_STATUS_INVALID_ARGUMENT;
    plan->config = *config;
    ++plan->revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanAdd(UmiCashPlan *plan, const UmiCashPlanEntry *entry)
{
    if (plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = EntryValid(plan, entry, SIZE_MAX);
    if (status != UMI_STATUS_OK)
        return status;
    if (plan->count == UMI_CASH_PLAN_CAPACITY || plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->entries[plan->count++] = *entry;
    ++plan->revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanReplace(UmiCashPlan *plan, size_t index, const UmiCashPlanEntry *entry)
{
    if (plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->count)
        return UMI_STATUS_NOT_FOUND;
    UmiStatus status = EntryValid(plan, entry, index);
    if (status != UMI_STATUS_OK)
        return status;
    if (plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->entries[index] = *entry;
    ++plan->revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanRemove(UmiCashPlan *plan, size_t index)
{
    if (plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->count)
        return UMI_STATUS_NOT_FOUND;
    if (plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memmove(&plan->entries[index], &plan->entries[index + 1U],
            (plan->count - index - 1U) * sizeof plan->entries[0]);
    memset(&plan->entries[--plan->count], 0, sizeof plan->entries[0]);
    ++plan->revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanAt(const UmiCashPlan *plan, size_t index, UmiCashPlanEntry *out)
{
    if (plan == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->count)
        return UMI_STATUS_NOT_FOUND;
    *out = plan->entries[index];
    return UMI_STATUS_OK;
}
