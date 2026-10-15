/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/csv_import.c
 * PURPOSE: Publish CSV observations only after the complete statement passes validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "feed_private.h"
#include "csv_private.h"
#include "umicom/base/csv_reader.h"
#include "umicom/bank_feed/csv_import.h"
#include "umicom/bank_feed/statement.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus CsvIssue(UmiBankFeedCsvIssue *issue, size_t record, size_t column,
                          UmiStatus status)
{
    if (issue != NULL) {
        issue->record = record;
        issue->column = column;
    }
    return status;
}
/* The CSV reader checks UTF-8; this layer checks bank-field character counts.
 * Only descriptions admit multiline text. Identity and numeric fields retain
 * their exact spelling, with no trimming or guessed character replacement. */
static UmiStatus CsvBankText(const UmiCsvReadRow *source, size_t column, char *out,
                             size_t capacity, size_t characters, bool multiline,
                             bool optional)
{
    size_t length = source->lengths[column], count = 0U;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U && !optional) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)source->fields[column][i];
        if (ch < 32U && !(multiline && (ch == '\t' || ch == '\r' || ch == '\n')))
            return UMI_STATUS_PARSE_ERROR;
        if ((ch & 0xc0U) != 0x80U) ++count;
    }
    if (count > characters) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, source->fields[column], length + 1U);
    return UMI_STATUS_OK;
}
static UmiStatus CsvBankRow(const UmiCsvReadRow *source, UmiBankFeedRow *row,
                            UmiBankFeedCsvIssue *issue)
{
    if (source->count != 9U)
        return CsvIssue(issue, source->record, 0U, UMI_STATUS_PARSE_ERROR);
#define COPY_FIELD(column, field, maximum, multiline, optional) \
    do { \
        UmiStatus status = CsvBankText(source, column, row->field, sizeof row->field, \
                                       maximum, multiline, optional); \
        if (status != UMI_STATUS_OK) \
            return CsvIssue(issue, source->record, (column) + 1U, status); \
    } while (0)
    COPY_FIELD(0U, account, 40U, false, false);
    COPY_FIELD(1U, transaction, 210U, false, true);
    COPY_FIELD(2U, reference, 210U, false, true);
    COPY_FIELD(3U, information, 500U, true, true);
    COPY_FIELD(4U, bookingTime, 10U, false, false);
    COPY_FIELD(5U, currency, 3U, false, false);
#undef COPY_FIELD
    UmiFinancialDate date;
    if (UmiBankStatementDate(row->bookingTime, &date) != UMI_STATUS_OK)
        return CsvIssue(issue, source->record, 5U, UMI_STATUS_PARSE_ERROR);
    for (size_t i = 0U; i < 3U; ++i)
        if (row->currency[i] < 'A' || row->currency[i] > 'Z')
            return CsvIssue(issue, source->record, 6U, UMI_STATUS_PARSE_ERROR);
    if (strlen(row->currency) != 3U)
        return CsvIssue(issue, source->record, 6U, UMI_STATUS_PARSE_ERROR);
    const char *amount = source->fields[6];
    row->credit = amount[0] != '-';
    if (!row->credit) ++amount;
    size_t length = strlen(amount);
    if (length >= sizeof row->amountText)
        return CsvIssue(issue, source->record, 7U, UMI_STATUS_CAPACITY_EXCEEDED);
    memcpy(row->amountText, amount, length + 1U);
    UmiStatus status = BankFeedAmount(row);
    if (status != UMI_STATUS_OK)
        return CsvIssue(issue, source->record, 7U, status);
    if (strcmp(source->fields[7], "BOOK") == 0) row->booked = true;
    else if (strcmp(source->fields[7], "PDNG") != 0)
        return CsvIssue(issue, source->record, 8U, UMI_STATUS_PARSE_ERROR);
    if (strcmp(source->fields[8], "Mutable") == 0) row->mutableRecord = true;
    else if (strcmp(source->fields[8], "Immutable") != 0)
        return CsvIssue(issue, source->record, 9U, UMI_STATUS_PARSE_ERROR);
    if (!row->booked) row->mutableRecord = true;
    return UMI_STATUS_OK;
}
/* A failed candidate occupies only unpublished storage. Clearing its slot
 * lets an internal caller retry without inheriting flags from the failed row.
 * The public count and missing-ID count advance together after validation. */
UmiStatus BankFeedCsvAppend(UmiBankFeed *feed, const UmiCsvReadRow *source,
                            UmiBankFeedCsvIssue *issue)
{
    if (feed->summary.rows == UMI_BANK_FEED_MAX_ROWS)
        return CsvIssue(issue, source->record, 0U, UMI_STATUS_CAPACITY_EXCEEDED);
    UmiBankFeedRow *row = &feed->rows[feed->summary.rows];
    memset(row, 0, sizeof *row);
    UmiStatus status = CsvBankRow(source, row, issue);
    if (status != UMI_STATUS_OK) return status;
    if (row->transaction[0] != '\0') {
        for (size_t i = 0U; i < feed->summary.rows; ++i)
            if (strcmp(row->account, feed->rows[i].account) == 0 &&
                strcmp(row->transaction, feed->rows[i].transaction) == 0)
                return CsvIssue(issue, source->record, 2U, UMI_STATUS_ALREADY_EXISTS);
    } else ++feed->summary.missingIdentifiers;
    ++feed->summary.rows;
    return UMI_STATUS_OK;
}
/* Work on an unpublished feed. A bad later row must never leave a usable
 * partial balance calculation or hide an omitted transaction from the caller. */
UmiStatus UmiBankFeedCsvCreate(const void *bytes, size_t length, UmiBankFeed **out,
                             UmiBankFeedCsvIssue *issue)
{
    if (issue != NULL) memset(issue, 0, sizeof *issue);
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_BANK_FEED_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCsvReader *reader = NULL;
    UmiStatus status = UmiCsvReaderCreate(bytes, length, &reader);
    if (status != UMI_STATUS_OK) return status;
    const char *const names[] = {"account", "transaction", "reference", "information",
                                "booking_date", "currency", "amount", "status", "mutability"};
    UmiCsvReadRow source;
    status = UmiCsvReaderNext(reader, &source);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && source.count != 9U)
        status = CsvIssue(issue, 1U, 0U, UMI_STATUS_PARSE_ERROR);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 9U; ++i)
        if (strcmp(source.fields[i], names[i]) != 0)
            status = CsvIssue(issue, 1U, i + 1U, UMI_STATUS_PARSE_ERROR);
    UmiBankFeed *feed = NULL;
    if (status == UMI_STATUS_OK) {
        feed = calloc(1U, sizeof *feed);
        if (feed == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) {
        feed->rows = calloc(UMI_BANK_FEED_MAX_ROWS, sizeof *feed->rows);
        if (feed->rows == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    while (status == UMI_STATUS_OK) {
        status = UmiCsvReaderNext(reader, &source);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; break; }
        if (status != UMI_STATUS_OK) {
            CsvIssue(issue, feed->summary.rows + 2U, 0U, status);
            break;
        }
        /* The original append path remains for review. Both fixed and mapped
         * imports now call the same validator and duplicate-identity guard. */
#if 0
        if (feed->summary.rows == UMI_BANK_FEED_MAX_ROWS) {
            status = CsvIssue(issue, source.record, 0U, UMI_STATUS_CAPACITY_EXCEEDED);
            break;
        }
        UmiBankFeedRow *row = &feed->rows[feed->summary.rows];
        status = CsvBankRow(&source, row, issue);
        if (status != UMI_STATUS_OK) break;
        if (row->transaction[0] == '\0') ++feed->summary.missingIdentifiers;
        else {
            for (size_t i = 0U; i < feed->summary.rows; ++i)
                if (strcmp(row->account, feed->rows[i].account) == 0 &&
                    strcmp(row->transaction, feed->rows[i].transaction) == 0) {
                    status = CsvIssue(issue, source.record, 2U, UMI_STATUS_ALREADY_EXISTS);
                    break;
                }
        }
        if (status == UMI_STATUS_OK) ++feed->summary.rows;
#endif
        status = BankFeedCsvAppend(feed, &source, issue);
    }
    UmiCsvReaderDestroy(reader);
    if (status == UMI_STATUS_OK) *out = feed;
    else UmiBankFeedDestroy(feed);
    return status;
}
