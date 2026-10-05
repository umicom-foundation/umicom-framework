/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/report.c
 * PURPOSE: Export reproducible cash projections with explicit integer units.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cash_planning/document.h"
#include "umicom/base/csv_document.h"
#include "umicom/finance/money.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus Row(UmiCsvDocument *csv, UmiFinancialDate date, const char *kind, int64_t inflow,
                     int64_t output, int64_t balance, int64_t headroom, const UmiCashPlanConfig *config,
                     size_t entries)
{
    char text[11];
    UmiStatus status = UmiCashPlanDateFormat(date, text, sizeof text);
    if (status != UMI_STATUS_OK)
        return status;
    UmiCsvCell cells[] = {UmiCsvText(text),
                          UmiCsvText(kind),
                          UmiCsvSigned(inflow),
                          UmiCsvSigned(output),
                          UmiCsvSigned(balance),
                          UmiCsvSigned(headroom),
                          UmiCsvText(config->opening.currency.code),
                          UmiCsvUnsigned(config->opening.scale),
                          UmiCsvUnsigned((uint64_t)entries)};
    return UmiCsvDocumentAppendRow(csv, cells, 9U);
}
UmiStatus UmiCashPlanCsv(const UmiCashPlan *plan, char **output, size_t *out_size)
{
    if (output != NULL)
        *output = NULL;
    if (out_size != NULL)
        *out_size = 0U;
    if (output == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCashPlanForecast *forecast = malloc(sizeof *forecast);
    if (forecast == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiCashPlanProject(plan, forecast);
    UmiCsvDocument *csv = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentCreate(UMI_CASH_PLAN_DOCUMENT_LIMIT, &csv);
    const char *names[] = {"date",           "kind",     "inflow_minor", "outflow_minor", "balance_minor",
                           "headroom_minor", "currency", "scale",        "entries"};
    UmiCsvCell cells[9];
    for (size_t i = 0U; i < 9U; ++i)
        cells[i] = UmiCsvText(names[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(csv, cells, 9U);
    UmiMoney buffer = {0}, headroom = {0};
    if (status == UMI_STATUS_OK)
    {
        buffer = forecast->config.opening;
        buffer.minor_units = forecast->config.buffer_minor;
        status = umi_money_subtract(&forecast->config.opening, &buffer, &headroom);
    }
    if (status == UMI_STATUS_OK)
        status = Row(csv, forecast->config.start, "opening", 0, 0, forecast->config.opening.minor_units,
                     headroom.minor_units, &forecast->config, 0U);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < forecast->day_count; ++i)
    {
        const UmiCashPlanDay *d = &forecast->days[i];
        status = Row(csv, d->date, "activity", d->inflow_minor, d->outflow_minor, d->closing_minor,
                     d->headroom_minor, &forecast->config, d->entries);
    }
    if (status == UMI_STATUS_OK)
    {
        UmiMoney final = forecast->config.opening;
        final.minor_units = forecast->closing_minor;
        status = umi_money_subtract(&final, &buffer, &headroom);
    }
    if (status == UMI_STATUS_OK)
        status = Row(csv, forecast->config.end, "closing", 0, 0, forecast->closing_minor,
                     headroom.minor_units, &forecast->config, 0U);
    if (status == UMI_STATUS_OK)
    {
        size_t length = UmiCsvDocumentBytes(csv);
        char *bytes = malloc(length + 1U);
        if (bytes == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            memcpy(bytes, UmiCsvDocumentData(csv), length + 1U);
            *output = bytes;
            *out_size = length;
        }
    }
    UmiCsvDocumentDestroy(csv);
    free(forecast);
    return status;
}
