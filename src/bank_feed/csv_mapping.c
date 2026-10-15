/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/csv_mapping.c
 * PURPOSE: Map explicit statement columns into the shared financial validation boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "csv_private.h"
#include "../base/csv_text_private.h"
#include "umicom/bank_feed/csv_mapping.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus MappingIssue(UmiBankFeedCsvIssue *issue, size_t record,
                              size_t column, UmiStatus status)
{
    if (issue != NULL) { issue->record = record; issue->column = column; }
    return status;
}
void UmiBankFeedCsvMappingInit(UmiBankFeedCsvMapping *mapping)
{
    if (mapping == NULL) return;
    memset(mapping, 0, sizeof *mapping);
    const char *const names[] = {"account", "transaction", "reference", "information",
                                "booking_date", "currency", "amount", "status", "mutability"};
    for (size_t i = 0U; i < 9U; ++i)
        memcpy(mapping->headers[i], names[i], strlen(names[i]) + 1U);
    /* Missing evidence remains pending and editable until the user explicitly
     * declares otherwise. A source status column always takes precedence. */
    mapping->mutableRecord = true;
}
/* Fixed public arrays may arrive from settings storage or a language binding.
 * Verify termination before calling string functions, then validate UTF-8 and
 * code-point limits. No character replacement or whitespace trimming occurs. */
static bool MappingText(const char *text, size_t capacity, size_t maximum)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL) return false;
    size_t length = (size_t)(end - text), count = 0U;
    if (!CsvTextValid(text, length, capacity - 1U)) return false;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (ch < 32U || ch == 127U) return false;
        if ((ch & 0xc0U) != 0x80U) ++count;
    }
    return count <= maximum;
}
static bool MappingValid(const UmiBankFeedCsvMapping *mapping)
{
    if (mapping == NULL ||
        (mapping->dateOrder != UMI_BANK_CSV_DATE_ISO &&
         mapping->dateOrder != UMI_BANK_CSV_DATE_DAY_FIRST &&
         mapping->dateOrder != UMI_BANK_CSV_DATE_MONTH_FIRST) ||
        (mapping->amountMode != UMI_BANK_CSV_AMOUNT_SIGNED &&
         mapping->amountMode != UMI_BANK_CSV_AMOUNT_SEPARATE))
        return false;
    for (size_t i = 0U; i < UMI_BANK_CSV_FIELD_COUNT; ++i) {
        if (!MappingText(mapping->headers[i], sizeof mapping->headers[i],
                          UMI_BANK_CSV_HEADER_BYTES - 1U)) return false;
        for (size_t j = 0U; j < i; ++j)
            if (mapping->headers[i][0] != '\0' &&
                strcmp(mapping->headers[i], mapping->headers[j]) == 0) return false;
    }
    if (!MappingText(mapping->account, sizeof mapping->account, 40U) ||
        !MappingText(mapping->currency, sizeof mapping->currency, 3U)) return false;
    /* Exactly one source is required for identity and currency. A constant is
     * not a fallback for blank cells in a mapped column, which could mix accounts. */
    if ((mapping->headers[UMI_BANK_CSV_ACCOUNT][0] == '\0') ==
        (mapping->account[0] == '\0') ||
        (mapping->headers[UMI_BANK_CSV_CURRENCY][0] == '\0') ==
        (mapping->currency[0] == '\0')) return false;
    if (mapping->currency[0] != '\0') {
        if (strlen(mapping->currency) != 3U) return false;
        for (size_t i = 0U; i < 3U; ++i)
            if (mapping->currency[i] < 'A' || mapping->currency[i] > 'Z') return false;
    }
    if (mapping->headers[UMI_BANK_CSV_DATE][0] == '\0') return false;
    bool amount = mapping->headers[UMI_BANK_CSV_AMOUNT][0] != '\0';
    bool debit = mapping->headers[UMI_BANK_CSV_DEBIT][0] != '\0';
    bool credit = mapping->headers[UMI_BANK_CSV_CREDIT][0] != '\0';
    return mapping->amountMode == UMI_BANK_CSV_AMOUNT_SIGNED
        ? amount && !debit && !credit : !amount && debit && credit;
}
/* Resolve names once from the header. Reject ambiguous headers even when their
 * columns are unused, so a saved mapping cannot silently select a different value. */
static UmiStatus MappingColumns(const UmiBankFeedCsvMapping *mapping,
                                  const UmiCsvReadRow *header,
                                  size_t *columns, UmiBankFeedCsvIssue *issue)
{
    for (size_t i = 0U; i < header->count; ++i) {
        if (header->lengths[i] == 0U ||
            header->lengths[i] >= UMI_BANK_CSV_HEADER_BYTES ||
            !MappingText(header->fields[i], header->lengths[i] + 1U,
                          UMI_BANK_CSV_HEADER_BYTES - 1U))
            return MappingIssue(issue, 1U, i + 1U, UMI_STATUS_PARSE_ERROR);
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(header->fields[i], header->fields[j]) == 0)
                return MappingIssue(issue, 1U, i + 1U, UMI_STATUS_ALREADY_EXISTS);
    }
    for (size_t i = 0U; i < UMI_BANK_CSV_FIELD_COUNT; ++i) {
        columns[i] = SIZE_MAX;
        if (mapping->headers[i][0] == '\0') continue;
        for (size_t j = 0U; j < header->count; ++j)
            if (strcmp(mapping->headers[i], header->fields[j]) == 0) columns[i] = j;
        if (columns[i] == SIZE_MAX)
            return MappingIssue(issue, 1U, 0U, UMI_STATUS_NOT_FOUND);
    }
    return UMI_STATUS_OK;
}
/* Reorder digits only after checking the chosen spelling. The common row
 * validator checks leap years and calendar bounds after this conversion. */
static bool MappingDate(const char *source, UmiBankCsvDateOrder order, char *date)
{
    if (strlen(source) != 10U) return false;
    if (order == UMI_BANK_CSV_DATE_ISO) { memcpy(date, source, 11U); return true; }
    if (source[2] != '/' || source[5] != '/') return false;
    for (size_t i = 0U; i < 10U; ++i)
        if (i != 2U && i != 5U && (source[i] < '0' || source[i] > '9')) return false;
    memcpy(date, source + 6U, 4U);
    date[4] = '-'; date[7] = '-'; date[10] = '\0';
    size_t month = order == UMI_BANK_CSV_DATE_DAY_FIRST ? 3U : 0U;
    size_t day = order == UMI_BANK_CSV_DATE_DAY_FIRST ? 0U : 3U;
    memcpy(date + 5U, source + month, 2U);
    memcpy(date + 8U, source + day, 2U);
    return true;
}
static UmiStatus MappingMagnitude(const char *text, bool *nonzero)
{
    *nonzero = false;
    if (text[0] == '\0') return UMI_STATUS_OK;
    UmiBankFeedRow row = {0};
    size_t length = strlen(text);
    if (length >= sizeof row.amountText) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(row.amountText, text, length + 1U);
    /* This temporary code checks only the amount grammar. The actual currency
     * still passes the common row validator and is never replaced by this code. */
    memcpy(row.currency, "XXX", 4U);
    UmiStatus status = BankFeedAmount(&row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < length; ++i)
        if (text[i] >= '1' && text[i] <= '9') *nonzero = true;
    return UMI_STATUS_OK;
}
static UmiStatus MappingSeparate(const UmiCsvReadRow *source, const size_t *columns,
                                  char *amount, size_t *origin, UmiBankFeedCsvIssue *issue)
{
    const char *debit = source->fields[columns[UMI_BANK_CSV_DEBIT]];
    const char *credit = source->fields[columns[UMI_BANK_CSV_CREDIT]];
    bool hasDebit = false, hasCredit = false;
    UmiStatus status = MappingMagnitude(debit, &hasDebit);
    if (status != UMI_STATUS_OK)
        return MappingIssue(issue, source->record, columns[UMI_BANK_CSV_DEBIT] + 1U, status);
    status = MappingMagnitude(credit, &hasCredit);
    if (status != UMI_STATUS_OK)
        return MappingIssue(issue, source->record, columns[UMI_BANK_CSV_CREDIT] + 1U, status);
    if ((hasDebit && hasCredit) || (debit[0] == '\0' && credit[0] == '\0'))
        return MappingIssue(issue, source->record, 0U, UMI_STATUS_PARSE_ERROR);
    /* Zero on the unused side is common in exports. Nonzero values on both
     * sides are ambiguous and must not be silently netted into one transaction. */
    bool useDebit = hasDebit || (!hasCredit && credit[0] == '\0');
    const char *selected = useDebit ? debit : credit;
    size_t length = strlen(selected), prefix = hasDebit ? 1U : 0U;
    if (prefix != 0U) amount[0] = '-';
    memcpy(amount + prefix, selected, length + 1U);
    *origin = columns[useDebit ? UMI_BANK_CSV_DEBIT : UMI_BANK_CSV_CREDIT] + 1U;
    return UMI_STATUS_OK;
}
static UmiStatus MappingAppend(UmiBankFeed *feed, const UmiCsvReadRow *source,
                                const UmiBankFeedCsvMapping *mapping,
                                const size_t *columns, UmiBankFeedCsvIssue *issue)
{
    UmiCsvReadRow row = {0};
    size_t origins[9] = {0};
    row.count = 9U; row.record = source->record;
    for (size_t i = 0U; i < 9U; ++i) {
        row.fields[i] = columns[i] == SIZE_MAX ? "" : source->fields[columns[i]];
        if (columns[i] != SIZE_MAX) origins[i] = columns[i] + 1U;
    }
    if (columns[UMI_BANK_CSV_ACCOUNT] == SIZE_MAX) row.fields[0] = mapping->account;
    if (columns[UMI_BANK_CSV_CURRENCY] == SIZE_MAX) row.fields[5] = mapping->currency;
    if (columns[UMI_BANK_CSV_STATUS] == SIZE_MAX)
        row.fields[7] = mapping->booked ? "BOOK" : "PDNG";
    if (columns[UMI_BANK_CSV_MUTABILITY] == SIZE_MAX)
        row.fields[8] = mapping->mutableRecord ? "Mutable" : "Immutable";
    char date[11], amount[25];
    if (!MappingDate(row.fields[4], mapping->dateOrder, date))
        return MappingIssue(issue, source->record, origins[4], UMI_STATUS_PARSE_ERROR);
    row.fields[4] = date;
    if (mapping->amountMode == UMI_BANK_CSV_AMOUNT_SEPARATE) {
        UmiStatus status = MappingSeparate(source, columns, amount, &origins[6], issue);
        if (status != UMI_STATUS_OK) return status;
        row.fields[6] = amount;
    }
    for (size_t i = 0U; i < 9U; ++i) row.lengths[i] = strlen(row.fields[i]);
    /* Borrowed spans live until the next reader call. Append validates and copies
     * them before advancing, including descriptions that look like formulas. */
    UmiBankFeedCsvIssue normalised = {0};
    UmiStatus status = BankFeedCsvAppend(feed, &row, &normalised);
    if (status != UMI_STATUS_OK)
        return MappingIssue(issue, normalised.record,
            normalised.column == 0U ? 0U : origins[normalised.column - 1U], status);
    return UMI_STATUS_OK;
}
UmiStatus UmiBankFeedCsvMappedCreate(const void *bytes, size_t length,
                                    const UmiBankFeedCsvMapping *mapping,
                                    UmiBankFeed **out, UmiBankFeedCsvIssue *issue)
{
    if (issue != NULL) memset(issue, 0, sizeof *issue);
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || length == 0U || !MappingValid(mapping))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_BANK_FEED_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCsvReader *reader = NULL;
    UmiStatus status = UmiCsvReaderCreate(bytes, length, &reader);
    if (status != UMI_STATUS_OK) return status;
    UmiCsvReadRow source;
    size_t columns[UMI_BANK_CSV_FIELD_COUNT], width = 0U;
    status = UmiCsvReaderNext(reader, &source);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        width = source.count;
        status = MappingColumns(mapping, &source, columns, issue);
    } else MappingIssue(issue, 1U, 0U, status);
    UmiBankFeed *feed = NULL;
    if (status == UMI_STATUS_OK) {
        feed = calloc(1U, sizeof *feed);
        if (feed == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) {
        feed->rows = calloc(UMI_BANK_FEED_MAX_ROWS, sizeof *feed->rows);
        if (feed->rows == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Nothing escapes until every record has passed. The existing immutable
     * review and export APIs therefore never receive an accepted prefix. */
    while (status == UMI_STATUS_OK) {
        status = UmiCsvReaderNext(reader, &source);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; break; }
        if (status != UMI_STATUS_OK) {
            MappingIssue(issue, feed->summary.rows + 2U, 0U, status);
            break;
        }
        if (source.count != width) {
            status = MappingIssue(issue, source.record, 0U, UMI_STATUS_PARSE_ERROR);
            break;
        }
        status = MappingAppend(feed, &source, mapping, columns, issue);
    }
    UmiCsvReaderDestroy(reader);
    if (status == UMI_STATUS_OK) *out = feed;
    else UmiBankFeedDestroy(feed);
    return status;
}
