/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/observation_export.c
 * PURPOSE: Keep broker evidence formatting and new-file ownership independent of Trader widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/observation_export.h"
#include "umicom/platform/output_file.h"
#include <string.h>
enum ObservationColumn
{
    RECORD_KIND,
    ACCOUNT,
    ENVIRONMENT,
    ATTESTED,
    CONNECTION_STATE,
    STALE,
    SUMMARY_COMPLETE,
    POSITIONS_COMPLETE,
    REQUEST_TIME,
    SUMMARY_TIME,
    POSITIONS_TIME,
    CAPTURE_TIME,
    VALUE_COUNT,
    POSITION_COUNT,
    TAG,
    VALUE,
    CURRENCY,
    CONTRACT_ID,
    SYMBOL,
    SECURITY_TYPE,
    EXPIRY,
    STRIKE,
    RIGHT,
    MULTIPLIER,
    EXCHANGE,
    LOCAL_SYMBOL,
    TRADING_CLASS,
    QUANTITY,
    AVERAGE_COST,
    COLUMN_COUNT
};
/* Bounded public text fields must have a terminator before the CSV convenience
 * constructor measures them. The shared CSV writer then validates UTF-8 and
 * quotes complete rows, including hostile formula and separator spellings. */
static bool ObservationText(const char *text, size_t capacity)
{
    return memchr(text, '\0', capacity) != NULL;
}
static UmiStatus ObservationValidate(const UmiIbkrConnectionSnapshot *snapshot, uint64_t now)
{
    if (snapshot == NULL || snapshot->accountCount > UMI_IBKR_ACCOUNT_LIMIT ||
        snapshot->valueCount > UMI_IBKR_VALUE_LIMIT || snapshot->positionCount > UMI_IBKR_POSITION_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!snapshot->readOnly || snapshot->environmentAttested || !snapshot->requestIssued ||
        (snapshot->requestedEnvironment != UMI_TRADING_PAPER &&
         snapshot->requestedEnvironment != UMI_TRADING_LIVE) ||
        (snapshot->state != UMI_IBKR_READY && snapshot->state != UMI_IBKR_DISCONNECTED &&
         snapshot->state != UMI_IBKR_FAILED) ||
        now < snapshot->requestedAtMilliseconds ||
        (snapshot->summaryComplete && (snapshot->summaryAtMilliseconds < snapshot->requestedAtMilliseconds ||
                                       snapshot->summaryAtMilliseconds > now)) ||
        (snapshot->positionsComplete &&
         (snapshot->positionsAtMilliseconds < snapshot->requestedAtMilliseconds ||
          snapshot->positionsAtMilliseconds > now)))
        return UMI_STATUS_INVALID_STATE;
    if (!ObservationText(snapshot->selectedAccount, sizeof(snapshot->selectedAccount)) ||
        snapshot->selectedAccount[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t matches = 0U;
    for (size_t i = 0U; i < snapshot->accountCount; ++i)
    {
        if (!ObservationText(snapshot->accounts[i], sizeof(snapshot->accounts[i])))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(snapshot->accounts[i], snapshot->selectedAccount) == 0)
            ++matches;
    }
    if (matches != 1U)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < snapshot->valueCount; ++i)
    {
        const UmiIbkrAccountValue *value = &snapshot->values[i];
        if (!ObservationText(value->tag, sizeof(value->tag)) ||
            !ObservationText(value->value, sizeof(value->value)) ||
            !ObservationText(value->currency, sizeof(value->currency)))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < snapshot->positionCount; ++i)
    {
        const UmiIbkrPositionObservation *position = &snapshot->positions[i];
#define OBSERVATION_FIELD(field)                                                                             \
    if (!ObservationText(position->field, sizeof(position->field)))                                          \
    return UMI_STATUS_INVALID_ARGUMENT
        OBSERVATION_FIELD(contractId);
        OBSERVATION_FIELD(symbol);
        OBSERVATION_FIELD(securityType);
        OBSERVATION_FIELD(expiry);
        OBSERVATION_FIELD(strike);
        OBSERVATION_FIELD(right);
        OBSERVATION_FIELD(multiplier);
        OBSERVATION_FIELD(exchange);
        OBSERVATION_FIELD(currency);
        OBSERVATION_FIELD(localSymbol);
        OBSERVATION_FIELD(tradingClass);
        OBSERVATION_FIELD(quantity);
        OBSERVATION_FIELD(averageCost);
#undef OBSERVATION_FIELD
    }
    return UMI_STATUS_OK;
}
/* Common columns travel with every record, so filtering a spreadsheet cannot
 * detach a row from its account, requested environment or completion state. */
static void ObservationRow(const UmiIbkrConnectionSnapshot *snapshot, uint64_t now, const char *kind,
                           UmiCsvCell *row)
{
    for (size_t i = 0U; i < COLUMN_COUNT; ++i)
        row[i] = UmiCsvText("");
    row[RECORD_KIND] = UmiCsvText(kind);
    row[ACCOUNT] = UmiCsvText(snapshot->selectedAccount);
    row[ENVIRONMENT] = UmiCsvText(snapshot->requestedEnvironment == UMI_TRADING_LIVE ? "live" : "paper");
    row[ATTESTED] = UmiCsvText("false");
    row[CONNECTION_STATE] = UmiCsvText(UmiIbkrConnectionStateName(snapshot->state));
    row[STALE] = UmiCsvText(snapshot->stale ? "true" : "false");
    row[SUMMARY_COMPLETE] = UmiCsvText(snapshot->summaryComplete ? "true" : "false");
    row[POSITIONS_COMPLETE] = UmiCsvText(snapshot->positionsComplete ? "true" : "false");
    row[REQUEST_TIME] = UmiCsvUnsigned(snapshot->requestedAtMilliseconds);
    if (snapshot->summaryComplete)
        row[SUMMARY_TIME] = UmiCsvUnsigned(snapshot->summaryAtMilliseconds);
    if (snapshot->positionsComplete)
        row[POSITIONS_TIME] = UmiCsvUnsigned(snapshot->positionsAtMilliseconds);
    row[CAPTURE_TIME] = UmiCsvUnsigned(now);
    row[VALUE_COUNT] = UmiCsvUnsigned((uint64_t)snapshot->valueCount);
    row[POSITION_COUNT] = UmiCsvUnsigned((uint64_t)snapshot->positionCount);
}
UmiStatus UmiIbkrObservationsExportCsv(const UmiIbkrConnectionSnapshot *snapshot, uint64_t now,
                                       UmiCsvDocument **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = ObservationValidate(snapshot, now);
    if (status != UMI_STATUS_OK)
        return status;
    const char *headers[COLUMN_COUNT] = {"record_kind",
                                         "selected_account",
                                         "requested_environment",
                                         "environment_attested",
                                         "connection_state",
                                         "connection_stale",
                                         "summary_complete",
                                         "positions_complete",
                                         "requested_monotonic_ms",
                                         "summary_monotonic_ms",
                                         "positions_monotonic_ms",
                                         "captured_monotonic_ms",
                                         "value_count",
                                         "position_count",
                                         "tag",
                                         "provider_value",
                                         "currency",
                                         "contract_id",
                                         "symbol",
                                         "security_type",
                                         "expiry",
                                         "strike",
                                         "right",
                                         "multiplier",
                                         "exchange",
                                         "local_symbol",
                                         "trading_class",
                                         "quantity",
                                         "average_cost"};
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(512U * 1024U, &document);
    UmiCsvCell row[COLUMN_COUNT];
    for (size_t i = 0U; i < COLUMN_COUNT; ++i)
        row[i] = UmiCsvText(headers[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, row, COLUMN_COUNT);
    ObservationRow(snapshot, now, "metadata", row);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, row, COLUMN_COUNT);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot->valueCount; ++i)
    {
        const UmiIbkrAccountValue *value = &snapshot->values[i];
        ObservationRow(snapshot, now, "account_value", row);
        row[TAG] = UmiCsvText(value->tag);
        row[VALUE] = UmiCsvText(value->value);
        row[CURRENCY] = UmiCsvText(value->currency);
        status = UmiCsvDocumentAppendRow(document, row, COLUMN_COUNT);
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot->positionCount; ++i)
    {
        const UmiIbkrPositionObservation *position = &snapshot->positions[i];
        ObservationRow(snapshot, now, "position", row);
        row[CURRENCY] = UmiCsvText(position->currency);
        row[CONTRACT_ID] = UmiCsvText(position->contractId);
        row[SYMBOL] = UmiCsvText(position->symbol);
        row[SECURITY_TYPE] = UmiCsvText(position->securityType);
        row[EXPIRY] = UmiCsvText(position->expiry);
        row[STRIKE] = UmiCsvText(position->strike);
        row[RIGHT] = UmiCsvText(position->right);
        row[MULTIPLIER] = UmiCsvText(position->multiplier);
        row[EXCHANGE] = UmiCsvText(position->exchange);
        row[LOCAL_SYMBOL] = UmiCsvText(position->localSymbol);
        row[TRADING_CLASS] = UmiCsvText(position->tradingClass);
        row[QUANTITY] = UmiCsvText(position->quantity);
        row[AVERAGE_COST] = UmiCsvText(position->averageCost);
        status = UmiCsvDocumentAppendRow(document, row, COLUMN_COUNT);
    }
    if (status != UMI_STATUS_OK)
    {
        UmiCsvDocumentDestroy(document);
        return status;
    }
    *out = document;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrObservationsWriteNew(const UmiCsvDocument *document, const char *path,
                                      const UmiCancellationToken *cancel)
{
    if (document == NULL || UmiCsvDocumentBytes(document) == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiOutputFile *file = NULL;
    UmiStatus status = UmiOutputFileCreate(path, &file);
    size_t size = UmiCsvDocumentBytes(document), offset = 0U;
    const char *bytes = UmiCsvDocumentData(document);
    while (status == UMI_STATUS_OK && offset < size)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        size_t count = size - offset > 16384U ? 16384U : size - offset;
        status = UmiOutputFileWrite(file, bytes + offset, count);
        offset += count;
    }
    if (file != NULL)
    {
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK)
            status = closed;
    }
    UmiOutputFileDestroy(file);
    return status;
}
