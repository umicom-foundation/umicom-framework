/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting_book.c
 *
 * PURPOSE:
 *   Implement double-entry validation and approval-before-posting for treasury.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/accounting_book.h"

#include <math.h>
#include <float.h>
#include <string.h>


/* Validate the complete public value before using count or fixed strings. This
 * legacy preview model stores doubles; it is not the exact-money ledger. */
static int AccountingText(const char *text, size_t capacity)
{
    size_t length = 0U;
    int nonblank = 0;
    while (length < capacity && text[length] != '\0') {
        unsigned char c = (unsigned char)text[length++];
        if (c < 32U || c == 127U) return 0;
        if (c != ' ') nonblank = 1;
    }
    return length != 0U && length < capacity && nonblank;
}
static int AccountingCurrency(const char *currency)
{
    if (currency == NULL) return 0;
    for (size_t i = 0U; i < 3U; ++i)
        if (currency[i] < 'A' || currency[i] > 'Z') return 0;
    return currency[3] == '\0';
}
UmiStatus umi_open_tms_accounting_line_validate(const UmiOpenTmsAccountingLine *line)
{
    if (line == NULL || !AccountingText(line->accountId, sizeof line->accountId) ||
        !AccountingText(line->referenceId, sizeof line->referenceId) ||
        !AccountingCurrency(line->currency) || !isfinite(line->debit) ||
        !isfinite(line->credit) || line->debit < 0.0 || line->credit < 0.0 ||
        (line->debit > 0.0 && line->credit > 0.0) ||
        (line->debit == 0.0 && line->credit == 0.0))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_accounting_book_validate(const UmiOpenTmsAccountingBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U || (book->approved != 0 && book->approved != 1) ||
        (book->posted != 0 && book->posted != 1) || (book->posted && !book->approved) ||
        (book->approved && book->count < 2U)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiStatus status = umi_open_tms_accounting_line_validate(&book->lines[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
/* Called only after validation. Never publish a partial or overflowed total. */
static UmiStatus CurrencyTotals(const UmiOpenTmsAccountingBook *book, const char *currency,
    double *out_debit, double *out_credit)
{
    double debit = 0.0, credit = 0.0;
    size_t matched = 0U;
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsAccountingLine *line = &book->lines[i];
        if (strcmp(currency, line->currency) != 0) continue;
        if (line->debit > DBL_MAX - debit || line->credit > DBL_MAX - credit)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        debit += line->debit; credit += line->credit; ++matched;
    }
    if (matched == 0U) return UMI_STATUS_NOT_FOUND;
    *out_debit = debit; *out_credit = credit;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_accounting_book_currency_totals(const UmiOpenTmsAccountingBook *book,
    const char *currency, double *out_debit, double *out_credit)
{
    if (out_debit == NULL || out_credit == NULL || out_debit == out_credit ||
        !AccountingCurrency(currency)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    return CurrencyTotals(book, currency, out_debit, out_credit);
}
static UmiStatus BalancedCurrencies(const UmiOpenTmsAccountingBook *book)
{
    if (book->count < 2U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        int seen = 0;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(book->lines[i].currency, book->lines[j].currency) == 0) { seen = 1; break; }
        if (seen) continue;
        double debit, credit;
        UmiStatus status = CurrencyTotals(book, book->lines[i].currency, &debit, &credit);
        if (status != UMI_STATUS_OK) return status;
        if (debit == 0.0 || credit == 0.0 ||
            fabs(debit - credit) > UMI_OPEN_TMS_ACCOUNTING_BALANCE_TOLERANCE)
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Persistence and review share the same balance rule instead of duplicating
 * tolerances or currency handling outside the accounting owner. */
UmiStatus umi_open_tms_accounting_book_check_balance(const UmiOpenTmsAccountingBook *book)
{
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    return status == UMI_STATUS_OK ? BalancedCurrencies(book) : status;
}

void umi_open_tms_accounting_book_init(UmiOpenTmsAccountingBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_accounting_book_add(
    UmiOpenTmsAccountingBook *book,
    const UmiOpenTmsAccountingLine *line)
{
    if (book == NULL || line == NULL || line->accountId[0] == '\0' ||
        line->currency[0] == '\0' || line->referenceId[0] == '\0' ||
        book->posted || line->debit < 0.0 || line->credit < 0.0 ||
        (line->debit > 0.0 && line->credit > 0.0) ||
        (line->debit == 0.0 && line->credit == 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (book->count >= UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->lines[book->count++] = *line;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

/* Preserve the former accounting behavior for review. The active boundary
 * validates finite bounded rows, separates currencies and revokes stale
 * approval on edits before a local posting flag can be set. */
#if 0
UmiStatus umi_open_tms_accounting_book_add(
    UmiOpenTmsAccountingBook *book,
    const UmiOpenTmsAccountingLine *line)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (book == NULL || line == NULL || line->accountId[0] == '\0' ||
        line->currency[0] == '\0' || line->referenceId[0] == '\0' ||
        book->posted || line->debit < 0.0 || line->credit < 0.0 ||
        (line->debit > 0.0 && line->credit > 0.0) ||
        (line->debit == 0.0 && line->credit == 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (book->count >= UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->lines[book->count++] = *line;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_open_tms_accounting_book_add(UmiOpenTmsAccountingBook *book,
    const UmiOpenTmsAccountingLine *line)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_line_validate(line);
    if (status != UMI_STATUS_OK) return status;
    if (book->posted) return UMI_STATUS_INVALID_STATE;
    if (book->count == UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->lines[book->count++] = *line;
    book->approved = 0; /* Adding a line invalidates the prior complete-batch approval. */
    ++book->revision;
    return UMI_STATUS_OK;
}

/* Preserve the former accounting behavior for review. The active boundary
 * validates finite bounded rows, separates currencies and revokes stale
 * approval on edits before a local posting flag can be set. */
#if 0
UmiStatus umi_open_tms_accounting_book_totals(
    const UmiOpenTmsAccountingBook *book,
    double *outDebit,
    double *outCredit)
{
    size_t index;
    double debit = 0.0;
    double credit = 0.0;

    if (book == NULL || outDebit == NULL || outCredit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        debit += book->lines[index].debit;
        credit += book->lines[index].credit;
    }
    *outDebit = debit;
    *outCredit = credit;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_open_tms_accounting_book_totals(const UmiOpenTmsAccountingBook *book,
    double *outDebit, double *outCredit)
{
    if (outDebit == NULL || outCredit == NULL || outDebit == outCredit) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    if (book->count == 0U) { *outDebit = 0.0; *outCredit = 0.0; return UMI_STATUS_OK; }
    for (size_t i = 1U; i < book->count; ++i)
        if (strcmp(book->lines[0].currency, book->lines[i].currency) != 0)
            return UMI_STATUS_INVALID_STATE;
    return CurrencyTotals(book, book->lines[0].currency, outDebit, outCredit);
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_accounting_book_approve(UmiOpenTmsAccountingBook *book)
{
    double debit;
    double credit;

    if (book == NULL || book->count < 2U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_open_tms_accounting_book_totals(book, &debit, &credit) !=
            UMI_STATUS_OK ||
        fabs(debit - credit) > 0.000001) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->approved = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

/* Preserve the former accounting behavior for review. The active boundary
 * validates finite bounded rows, separates currencies and revokes stale
 * approval on edits before a local posting flag can be set. */
#if 0
UmiStatus umi_open_tms_accounting_book_approve(UmiOpenTmsAccountingBook *book)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    double debit;
    double credit;

    if (book == NULL || book->count < 2U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_open_tms_accounting_book_totals(book, &debit, &credit) !=
            UMI_STATUS_OK ||
        fabs(debit - credit) > 0.000001) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->approved = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_open_tms_accounting_book_approve(UmiOpenTmsAccountingBook *book)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    if (book->posted) return UMI_STATUS_INVALID_STATE;
    if (book->approved) return UMI_STATUS_ALREADY_EXISTS;
    status = BalancedCurrencies(book);
    if (status != UMI_STATUS_OK) return status;
    book->approved = 1; ++book->revision;
    return UMI_STATUS_OK;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_accounting_book_post(UmiOpenTmsAccountingBook *book)
{
    if (book == NULL || !book->approved || book->posted) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->posted = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

/* Preserve the former accounting behavior for review. The active boundary
 * validates finite bounded rows, separates currencies and revokes stale
 * approval on edits before a local posting flag can be set. */
#if 0
UmiStatus umi_open_tms_accounting_book_post(UmiOpenTmsAccountingBook *book)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (book == NULL || !book->approved || book->posted) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->posted = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_open_tms_accounting_book_post(UmiOpenTmsAccountingBook *book)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    if (!book->approved || book->posted) return UMI_STATUS_INVALID_STATE;
    status = BalancedCurrencies(book);
    if (status != UMI_STATUS_OK) return status;
    book->posted = 1; ++book->revision;
    return UMI_STATUS_OK;
}
