/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/validation.c
 *
 * PURPOSE:
 *   Validate bounded commands, identifiers, dates and exact integer amounts before domain state can change.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

bool FinanceTextValid(const char *text, size_t capacity, bool required)
{
    size_t index = 0U;
    if (text == NULL || capacity == 0U || (required && text[0] == '\0')) return false;
    while (index < capacity) {
        unsigned char first = (unsigned char)text[index++];
        unsigned continuation;
        uint32_t code, minimum;
        if (first == 0U) return true;
        if (first < 32U || first == 127U) return false;
        if (first < 128U) continue;
        if (first >= 0xc2U && first <= 0xdfU) { continuation = 1U; code = first & 0x1fU; minimum = 0x80U; }
        else if (first >= 0xe0U && first <= 0xefU) { continuation = 2U; code = first & 0x0fU; minimum = 0x800U; }
        else if (first >= 0xf0U && first <= 0xf4U) { continuation = 3U; code = first & 0x07U; minimum = 0x10000U; }
        else return false;
        for (unsigned count = 0U; count < continuation; ++count) {
            unsigned char next;
            if (index >= capacity) return false;
            next = (unsigned char)text[index++];
            if ((next & 0xc0U) != 0x80U) return false;
            code = (code << 6U) | (next & 0x3fU);
        }
        if (code < minimum || code > 0x10ffffU || (code >= 0xd800U && code <= 0xdfffU)) return false;
    }
    return false;
}

bool FinanceIdValid(const UmiFinancialId *id, bool required)
{
    if (id == NULL || !FinanceTextValid(id->value, sizeof id->value, required)) return false;
    for (size_t i = 0U; id->value[i] != '\0'; ++i) {
        const char ch = id->value[i];
        if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
              (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' ||
              ch == '.' || ch == ':' || ch == '/')) return false;
    }
    return true;
}

bool FinanceCurrencyValid(UmiCurrency currency)
{
    return currency.code[3] == '\0' && currency.code[0] >= 'A' && currency.code[0] <= 'Z' &&
        currency.code[1] >= 'A' && currency.code[1] <= 'Z' &&
        currency.code[2] >= 'A' && currency.code[2] <= 'Z';
}

UmiStatus FinanceSetId(UmiFinancialId *id, const char *text)
{
    size_t length = 0U;
    if (id == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (length < sizeof id->value && text[length] != '\0') ++length;
    if (length == sizeof id->value) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(id, 0, sizeof *id);
    memcpy(id->value, text, length);
    return FinanceIdValid(id, true) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

bool FinanceIdEqual(UmiFinancialId left, UmiFinancialId right)
{
    return strncmp(left.value, right.value, sizeof left.value) == 0;
}

bool FinanceDateInPeriod(UmiFinancialDate date, const UmiFinanceOperationPeriod *period)
{
    return period != NULL && umi_financial_date_is_valid(date) &&
        umi_financial_date_compare(date, period->startDate) >= 0 &&
        umi_financial_date_compare(date, period->endDate) <= 0;
}

/* Check before doing the arithmetic: signed overflow is not a recoverable
 * result in C. The caller never has to interpret a wrapped financial amount. */
UmiStatus FinanceAdd(int64_t left, int64_t right, int64_t *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if ((right > 0 && left > INT64_MAX - right) ||
        (right < 0 && left < INT64_MIN - right)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = left + right;
    return UMI_STATUS_OK;
}

UmiStatus FinanceMultiplyPositive(int64_t left, int64_t right, int64_t *out)
{
    if (out == NULL || left < 0 || right < 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (right != 0 && left > INT64_MAX / right) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = left * right;
    return UMI_STATUS_OK;
}

UmiStatus FinanceCost(const UmiFinanceOperationInstrument *instrument,
    int64_t ticks, int64_t lots, int64_t *out)
{
    int64_t amount;
    UmiStatus status;
    if (instrument == NULL || out == NULL || ticks <= 0 || lots < 0) return UMI_STATUS_INVALID_ARGUMENT;
    status = FinanceMultiplyPositive(ticks, instrument->minorPerTick, &amount);
    if (status == UMI_STATUS_OK) status = FinanceMultiplyPositive(amount, instrument->unitsPerLot, &amount);
    if (status == UMI_STATUS_OK) status = FinanceMultiplyPositive(amount, lots, &amount);
    if (status == UMI_STATUS_OK) *out = amount;
    return status;
}

static bool OptionalDateValid(UmiFinancialDate date)
{
    return (date.year == 0 && date.month == 0U && date.day == 0U) || umi_financial_date_is_valid(date);
}

UmiStatus FinanceCommandValidate(const UmiFinanceOperationCommand *command)
{
    if (command == NULL || command->kind < UMI_FINANCE_CREATE_ACCOUNT ||
        command->kind > UMI_FINANCE_REOPEN_REVIEW ||
        command->lineCount > UMI_FINANCE_OPERATIONS_JOURNAL_LINES || command->scale > 9U ||
        command->accountClass < UMI_ACCOUNTING_ASSET || command->accountClass > UMI_ACCOUNTING_EXPENSE ||
        (command->side != 0 && command->side != UMI_SIDE_BUY && command->side != UMI_SIDE_SELL) ||
        !FinanceIdValid(&command->requestId, true) || !FinanceIdValid(&command->actorId, true) ||
        !FinanceIdValid(&command->id, true) || !FinanceIdValid(&command->referenceId, false) ||
        !FinanceIdValid(&command->periodId, false) || !FinanceIdValid(&command->accountId, false) ||
        !FinanceIdValid(&command->participantId, false) || !FinanceIdValid(&command->instrumentId, false) ||
        !FinanceTextValid(command->name, sizeof command->name, false) ||
        !OptionalDateValid(command->date) || !OptionalDateValid(command->endDate) ||
        (command->currency.code[0] != '\0' && !FinanceCurrencyValid(command->currency)))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Generated fill and settlement identities cannot be claimed by a manual
     * entity command. The prefix is still accepted when addressing those fills. */
    if (strncmp(command->id.value, "system.", 7U) == 0 &&
        command->kind != UMI_FINANCE_CLEAR_FILL && command->kind != UMI_FINANCE_SETTLE_FILL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = command->lineCount; i < UMI_FINANCE_OPERATIONS_JOURNAL_LINES; ++i) {
        if (command->lines[i].accountId.value[0] != '\0' || command->lines[i].debitMinor != 0 ||
            command->lines[i].creditMinor != 0) return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < command->lineCount; ++i) {
        const UmiFinanceOperationLine *line = &command->lines[i];
        if (!FinanceIdValid(&line->accountId, true) || line->debitMinor < 0 || line->creditMinor < 0 ||
            ((line->debitMinor > 0) == (line->creditMinor > 0))) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

void UmiFinanceOperationCommandInit(UmiFinanceOperationCommand *command)
{
    if (command != NULL) memset(command, 0, sizeof *command);
}

/* Magnitudes are unsigned so INT64_MIN is printable without negating it.
 * The scale is display metadata, not a request for floating-point division. */
UmiStatus UmiFinanceOperationsFormatAmount(int64_t minor, uint8_t scale, char *buffer, size_t capacity)
{
    uint64_t magnitude, divisor = 1U;
    int written;
    if (buffer == NULL || capacity == 0U || scale > 9U) return UMI_STATUS_INVALID_ARGUMENT;
    buffer[0] = '\0';
    magnitude = minor < 0 ? (uint64_t)(-(minor + 1)) + 1U : (uint64_t)minor;
    for (uint8_t i = 0U; i < scale; ++i) divisor *= 10U;
    if (scale == 0U) written = snprintf(buffer, capacity, "%s%" PRIu64, minor < 0 ? "-" : "", magnitude);
    else written = snprintf(buffer, capacity, "%s%" PRIu64 ".%0*" PRIu64,
        minor < 0 ? "-" : "", magnitude / divisor, (int)scale, magnitude % divisor);
    if (written < 0 || (size_t)written >= capacity) { buffer[0] = '\0'; return UMI_STATUS_CAPACITY_EXCEEDED; }
    return UMI_STATUS_OK;
}

const char *UmiFinanceOperationKindText(UmiFinanceOperationKind kind)
{
    switch (kind) {
    case UMI_FINANCE_CREATE_ACCOUNT: return "Create ledger account";
    case UMI_FINANCE_OPEN_PERIOD: return "Open accounting period";
    case UMI_FINANCE_ENTER_JOURNAL: return "Enter balanced journal";
    case UMI_FINANCE_APPROVE_JOURNAL: return "Approve journal";
    case UMI_FINANCE_POST_JOURNAL: return "Post journal";
    case UMI_FINANCE_REVERSE_JOURNAL: return "Prepare compensating journal";
    case UMI_FINANCE_REGISTER_PARTICIPANT: return "Register exchange participant";
    case UMI_FINANCE_LIST_INSTRUMENT: return "List instrument";
    case UMI_FINANCE_SET_MARKET_STATE: return "Open or halt instrument";
    case UMI_FINANCE_DEPOSIT_LOTS: return "Add simulated custody lots";
    case UMI_FINANCE_PLACE_ORDER: return "Place limit order";
    case UMI_FINANCE_CANCEL_ORDER: return "Cancel remaining order";
    case UMI_FINANCE_CLEAR_FILL: return "Clear fill locally";
    case UMI_FINANCE_SETTLE_FILL: return "Settle cash and lots locally";
    case UMI_FINANCE_RECONCILE_ACCOUNT: return "Reconcile account balance";
    case UMI_FINANCE_PREPARE_CLOSE: return "Prepare period close";
    case UMI_FINANCE_CLOSE_PERIOD: return "Confirm period close";
    case UMI_FINANCE_REOPEN_REVIEW: return "Reopen unconfirmed review";
    default: return "Unknown operation";
    }
}

/* These private lookups return indices rather than pointers so a caller cannot
 * accidentally retain an address across a candidate-state replacement. */
#define FINANCE_INDEX_FUNCTION(Name, Array, Count) \
size_t Name(const FinanceState *state, UmiFinancialId id) \
{ \
    for (size_t i = 0U; i < state->counts.Count; ++i) \
        if (FinanceIdEqual(state->Array[i].id, id)) return i; \
    return FINANCE_INDEX_NONE; \
}
FINANCE_INDEX_FUNCTION(FinanceAccountIndex, accounts, accounts)
FINANCE_INDEX_FUNCTION(FinancePeriodIndex, periods, periods)
FINANCE_INDEX_FUNCTION(FinanceParticipantIndex, participants, participants)
FINANCE_INDEX_FUNCTION(FinanceInstrumentIndex, instruments, instruments)
FINANCE_INDEX_FUNCTION(FinanceOrderIndex, orders, orders)
FINANCE_INDEX_FUNCTION(FinanceFillIndex, fills, fills)
#undef FINANCE_INDEX_FUNCTION

size_t FinanceJournalIndex(const FinanceState *state, UmiFinancialId id)
{
    for (size_t i = 0U; i < state->counts.journals; ++i)
        if (FinanceIdEqual(state->journals[i].entry.id, id)) return i;
    return FINANCE_INDEX_NONE;
}

size_t FinanceHoldingIndex(const FinanceState *state, UmiFinancialId participant, UmiFinancialId instrument)
{
    for (size_t i = 0U; i < state->counts.holdings; ++i)
        if (FinanceIdEqual(state->holdings[i].participantId, participant) &&
            FinanceIdEqual(state->holdings[i].instrumentId, instrument)) return i;
    return FINANCE_INDEX_NONE;
}
