/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/input.c
 *
 * PURPOSE:
 *   Reject incomplete dates, overflowing amounts and malformed journal rows before they reach command execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/finance_operations/input.h"
#include <limits.h>
#include <string.h>

static bool Space(char ch) { return ch == ' ' || ch == '\t' || ch == '\r'; }
UmiStatus UmiFinanceOperationsParseInteger(const char *text, int64_t *out)
{
    uint64_t magnitude = 0U, limit;
    size_t index = 0U, digits = 0U;
    bool negative;
    if (text == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (index < 128U && Space(text[index])) ++index;
    if (index == 128U) return UMI_STATUS_INVALID_ARGUMENT;
    negative = text[index] == '-';
    if (negative) ++index;
    limit = negative ? (uint64_t)INT64_MAX + 1U : (uint64_t)INT64_MAX;
    while (index < 128U && text[index] >= '0' && text[index] <= '9') {
        unsigned digit = (unsigned)(text[index] - '0');
        if (magnitude > (limit - digit) / 10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        magnitude = magnitude * 10U + digit; ++index; ++digits;
    }
    while (index < 128U && Space(text[index])) ++index;
    if (index == 128U || text[index] != '\0' || digits == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *out = negative ? (magnitude == (uint64_t)INT64_MAX + 1U ? INT64_MIN : -(int64_t)magnitude) : (int64_t)magnitude;
    return UMI_STATUS_OK;
}
UmiStatus UmiFinanceOperationsParseDate(const char *text, UmiFinancialDate *out)
{
    UmiFinancialDate date = {0, 0U, 0U};
    size_t length = 0U;
    if (text == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (length < 11U && text[length] != '\0') ++length;
    if (length != 10U || text[4] != '-' || text[7] != '-') return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < 10U; ++index) {
        if (index == 4U || index == 7U) continue;
        if (text[index] < '0' || text[index] > '9') return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t index = 0U; index < 4U; ++index) date.year = date.year * 10 + (text[index] - '0');
    date.month = (uint8_t)((text[5] - '0') * 10 + (text[6] - '0'));
    date.day = (uint8_t)((text[8] - '0') * 10 + (text[9] - '0'));
    if (!umi_financial_date_is_valid(date)) return UMI_STATUS_INVALID_ARGUMENT;
    *out = date; return UMI_STATUS_OK;
}
static char *Trim(char *text)
{
    size_t length;
    while (Space(*text)) ++text;
    length = strlen(text);
    while (length > 0U && Space(text[length - 1U])) text[--length] = '\0';
    return text;
}
UmiStatus UmiFinanceOperationsParseLines(const char *text,
    UmiFinanceOperationLine *lines, size_t capacity, size_t *outCount)
{
    UmiFinanceOperationLine parsed[UMI_FINANCE_OPERATIONS_JOURNAL_LINES] = {0};
    size_t used = 0U, offset = 0U;
    if (text == NULL || lines == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCount = 0U;
    while (text[offset] != '\0') {
        if (offset >= 4096U) return UMI_STATUS_CAPACITY_EXCEEDED;
        char row[256], *account, *debit, *credit, *first, *second;
        size_t length = 0U;
        UmiStatus status;
        while (text[offset] != '\0' && text[offset] != '\n') {
            if (length + 1U >= sizeof(row) || offset >= 4096U) return UMI_STATUS_CAPACITY_EXCEEDED;
            row[length++] = text[offset++];
        }
        if (text[offset] == '\n') ++offset;
        row[length] = '\0'; account = Trim(row);
        if (*account == '\0') continue;
        if (used >= capacity || used >= UMI_FINANCE_OPERATIONS_JOURNAL_LINES) return UMI_STATUS_CAPACITY_EXCEEDED;
        first = strchr(account, ','); if (first == NULL) return UMI_STATUS_PARSE_ERROR;
        *first++ = '\0'; second = strchr(first, ','); if (second == NULL) return UMI_STATUS_PARSE_ERROR;
        *second++ = '\0'; if (strchr(second, ',') != NULL) return UMI_STATUS_PARSE_ERROR;
        account = Trim(account); debit = Trim(first); credit = Trim(second);
        status = FinanceSetId(&parsed[used].accountId, account);
        if (status == UMI_STATUS_OK) status = UmiFinanceOperationsParseInteger(debit, &parsed[used].debitMinor);
        if (status == UMI_STATUS_OK) status = UmiFinanceOperationsParseInteger(credit, &parsed[used].creditMinor);
        if (status != UMI_STATUS_OK) return status;
        if (parsed[used].debitMinor < 0 || parsed[used].creditMinor < 0 ||
            ((parsed[used].debitMinor > 0) == (parsed[used].creditMinor > 0))) return UMI_STATUS_INVALID_ARGUMENT;
        ++used;
    }
    if (used < 2U) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(lines, parsed, used * sizeof(parsed[0])); *outCount = used;
    return UMI_STATUS_OK;
}
const char *UmiFinanceOperationsExplainStatus(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_OK: return "The operation was accepted.";
    case UMI_STATUS_INVALID_ARGUMENT: return "Check the fields, date, currency, positive quantity and balanced journal lines.";
    case UMI_STATUS_INVALID_STATE: return "The record is not ready for this step. Check approval, available funds, settlement and the accounting period.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "A number, field or practice-book limit would be exceeded. Nothing was posted.";
    case UMI_STATUS_NOT_FOUND: return "A referenced record was not found. Check its identifier and reload the shared book.";
    case UMI_STATUS_ALREADY_EXISTS: return "That identifier already exists, or a request identifier was reused with different details. Use New request for a different operation.";
    case UMI_STATUS_OUT_OF_MEMORY: return "Memory could not be allocated. Close unused windows and try again.";
    case UMI_STATUS_PERMISSION_DENIED: return "A workflow or funding check stopped the operation. Check the actor, order owner, available cash and custody lots.";
    case UMI_STATUS_IO_ERROR: return "The Data Server could not complete the storage operation. Check the displayed path and reopen the book; do not assume it was saved.";
    case UMI_STATUS_PARSE_ERROR: return "The input or stored history is not valid. Preserve the file and investigate rather than replacing it with an empty book.";
    case UMI_STATUS_UNAVAILABLE: return "This capability is not available in the current build.";
    case UMI_STATUS_BUSY: return "The shared book changed or is busy. Choose Reload shared book, review the new balances, then retry.";
    default: return "The operation did not complete. Keep its request identifier and inspect the diagnostic result.";
    }
}
