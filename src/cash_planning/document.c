/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/document.c
 * PURPOSE: Validate complete cash documents before exposing owned assumptions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cash_planning/document.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <stdlib.h>
#include <string.h>
void UmiCashPlanFreeBytes(char *bytes) { free(bytes); }
/* Writer failures are sticky. Fixed keys stay here; all user text passes the
 * shared JSON string encoder, so labels cannot inject new document members. */
static void TextField(UmiLanguageRuntimeJsonWriter *writer, const char *key, const char *value)
{
    (void)umi_language_runtime_json_writer_raw(writer, key);
    (void)umi_language_runtime_json_writer_string(writer, value);
}
static void IntegerField(UmiLanguageRuntimeJsonWriter *writer, const char *key, int64_t value)
{
    (void)umi_language_runtime_json_writer_raw(writer, key);
    (void)umi_language_runtime_json_writer_int64(writer, value);
}
UmiStatus UmiCashPlanEncode(const UmiCashPlan *plan, char **out, size_t *out_size)
{
    if (out != NULL)
        *out = NULL;
    if (out_size != NULL)
        *out_size = 0U;
    if (out == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCashPlanConfig config;
    UmiStatus status = UmiCashPlanRead(plan, &config);
    if (status != UMI_STATUS_OK)
        return status;
    char start[11], end[11];
    status = UmiCashPlanDateFormat(config.start, start, sizeof start);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanDateFormat(config.end, end, sizeof end);
    if (status != UMI_STATUS_OK)
        return status;
    char *bytes = malloc(UMI_CASH_PLAN_DOCUMENT_LIMIT);
    if (bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, bytes, UMI_CASH_PLAN_DOCUMENT_LIMIT);
    TextField(&writer, "{\"format\":", "umicom.cash-plan");
    TextField(&writer, ",\"title\":", config.title);
    TextField(&writer, ",\"start\":", start);
    TextField(&writer, ",\"end\":", end);
    TextField(&writer, ",\"currency\":", config.opening.currency.code);
    IntegerField(&writer, ",\"scale\":", config.opening.scale);
    IntegerField(&writer, ",\"openingMinor\":", config.opening.minor_units);
    IntegerField(&writer, ",\"bufferMinor\":", config.buffer_minor);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"entries\":[");
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiCashPlanCount(plan); ++i)
    {
        UmiCashPlanEntry entry;
        char date[11];
        status = UmiCashPlanAt(plan, i, &entry);
        if (status == UMI_STATUS_OK)
            status = UmiCashPlanDateFormat(entry.date, date, sizeof date);
        if (status != UMI_STATUS_OK)
            break;
        TextField(&writer, i == 0U ? "{\"id\":" : ",{\"id\":", entry.id);
        TextField(&writer, ",\"label\":", entry.label);
        TextField(&writer, ",\"date\":", date);
        TextField(&writer,
                  ",\"direction\":", entry.direction == UMI_FINANCIAL_DIRECTION_PAY ? "pay" : "receive");
        IntegerField(&writer, ",\"amountMinor\":", entry.amount.minor_units);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"enabled\":");
        (void)umi_language_runtime_json_writer_bool(&writer, entry.enabled);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}\n");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
    {
        *out = bytes;
        *out_size = writer.length;
    }
    else
        free(bytes);
    return status;
}
/* Exact member count plus unique named lookups rejects both unknown and
 * duplicate keys, including equivalent escaped spellings of the same name. */
static UmiStatus Members(const UmiJsonTree *tree, int object, const char *const *names, size_t count,
                         int *nodes)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, object) != count)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiStatus status = UmiJsonTreeMember(tree, object, names[i], &nodes[i]);
        if (status != UMI_STATUS_OK)
            return status;
    }
    return UMI_STATUS_OK;
}
static UmiStatus Date(const UmiJsonTree *tree, int node, UmiFinancialDate *out)
{
    char text[11];
    UmiStatus status = UmiJsonTreeText(tree, node, text, sizeof text);
    return status == UMI_STATUS_OK ? UmiCashPlanDateParse(text, strlen(text), out) : status;
}
static UmiStatus Entry(const UmiJsonTree *tree, int node, UmiCashPlan *plan, UmiMoney unit)
{
    const char *names[] = {"id", "label", "date", "direction", "amountMinor", "enabled"};
    int nodes[6];
    UmiStatus status = Members(tree, node, names, 6U, nodes);
    UmiCashPlanEntry entry = {0};
    entry.amount = unit;
    char direction[16];
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[0], entry.id, sizeof entry.id);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[1], entry.label, sizeof entry.label);
    if (status == UMI_STATUS_OK)
        status = Date(tree, nodes[2], &entry.date);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[3], direction, sizeof direction);
    if (status == UMI_STATUS_OK)
    {
        if (strcmp(direction, "pay") == 0)
            entry.direction = UMI_FINANCIAL_DIRECTION_PAY;
        else if (strcmp(direction, "receive") == 0)
            entry.direction = UMI_FINANCIAL_DIRECTION_RECEIVE;
        else
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, nodes[4], &entry.amount.minor_units);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeBoolean(tree, nodes[5], &entry.enabled);
    return status == UMI_STATUS_OK ? UmiCashPlanAdd(plan, &entry) : status;
}
UmiStatus UmiCashPlanDecode(const void *bytes, size_t size, UmiCashPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiJsonTreeLimits limits = {UMI_CASH_PLAN_DOCUMENT_LIMIT, 4096U, 6U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, NULL, &tree);
    const char *names[] = {"format", "title",        "start",       "end",    "currency",
                           "scale",  "openingMinor", "bufferMinor", "entries"};
    int nodes[9];
    UmiCashPlanConfig config = {0};
    UmiCashPlan *plan = NULL;
    char format[64];
    int64_t scale = 0;
    if (status == UMI_STATUS_OK)
        status = Members(tree, 0, names, 9U, nodes);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[0], format, sizeof format);
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.cash-plan") != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[1], config.title, sizeof config.title);
    if (status == UMI_STATUS_OK)
        status = Date(tree, nodes[2], &config.start);
    if (status == UMI_STATUS_OK)
        status = Date(tree, nodes[3], &config.end);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, nodes[4], config.opening.currency.code,
                                 sizeof config.opening.currency.code);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, nodes[5], &scale);
    if (status == UMI_STATUS_OK && (scale < 0 || scale > 9))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        config.opening.scale = (uint8_t)scale;
        status = UmiJsonTreeInteger(tree, nodes[6], &config.opening.minor_units);
    }
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, nodes[7], &config.buffer_minor);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanCreate(&config, &plan);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, nodes[8]) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, nodes[8]) > UMI_CASH_PLAN_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        for (int node = UmiJsonTreeFirst(tree, nodes[8]); status == UMI_STATUS_OK && node >= 0;
             node = UmiJsonTreeNext(tree, node))
            status = Entry(tree, node, plan, config.opening);
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out = plan;
    else
        UmiCashPlanDestroy(plan);
    return status;
}
