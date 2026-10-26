/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward_book.c
 *
 * PURPOSE:
 *   Implement monotonic FX-forward lifecycle and simple forward MTM evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/fx_forward_book.h"
#include <string.h>

#include <stdbool.h>
#include <math.h>

/* The former implementation is retained for engineering review. Its permissive
 * state assignment and unchecked numeric/text values are replaced below by
 * bounded admission, explicit lifecycle edges and checked valuation evidence. */
#if 0
void umi_open_tms_fx_forward_book_init(UmiOpenTmsFxForwardBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiOpenTmsFxForward *umi_open_tms_fx_forward_book_find(
    const UmiOpenTmsFxForwardBook *book,
    const char *tradeId)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->trades[i].tradeId, tradeId) == 0)
            return &book->trades[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_fx_forward_book_add(
    UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForward *trade)
{
    if (book == NULL || trade == NULL || trade->tradeId[0] == '\0' ||
        trade->accountId[0] == '\0' || trade->counterpartyId[0] == '\0' ||
        trade->baseCurrency[0] == '\0' || trade->quoteCurrency[0] == '\0' ||
        trade->baseNotional <= 0.0 || trade->forwardRate <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_open_tms_fx_forward_book_find(book, trade->tradeId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_OPEN_TMS_FX_FORWARD_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count++] = *trade;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_open_tms_fx_forward_book_add(
    UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForward *trade)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (book == NULL || trade == NULL || trade->tradeId[0] == '\0' ||
        trade->accountId[0] == '\0' || trade->counterpartyId[0] == '\0' ||
        trade->baseCurrency[0] == '\0' || trade->quoteCurrency[0] == '\0' ||
        trade->baseNotional <= 0.0 || trade->forwardRate <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_open_tms_fx_forward_book_find(book, trade->tradeId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_OPEN_TMS_FX_FORWARD_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count++] = *trade;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_fx_forward_book_transition(
    UmiOpenTmsFxForwardBook *book,
    const char *tradeId,
    UmiOpenTmsFxForwardState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiOpenTmsFxForward *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) == 0) {
            if (version <= trade->version) return UMI_STATUS_INVALID_STATE;
            if (trade->state == UMI_OPEN_TMS_FX_FORWARD_SETTLED ||
                trade->state == UMI_OPEN_TMS_FX_FORWARD_CANCELLED)
                return UMI_STATUS_INVALID_STATE;
            trade->state = state;
            trade->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
#endif

UmiStatus umi_open_tms_fx_forward_book_transition(
    UmiOpenTmsFxForwardBook *book,
    const char *tradeId,
    UmiOpenTmsFxForwardState state,
    uint64_t version)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiOpenTmsFxForward *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) == 0) {
            if (version <= trade->version) return UMI_STATUS_INVALID_STATE;
            if (trade->state == UMI_OPEN_TMS_FX_FORWARD_SETTLED ||
                trade->state == UMI_OPEN_TMS_FX_FORWARD_CANCELLED)
                return UMI_STATUS_INVALID_STATE;
            trade->state = state;
            trade->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

double umi_open_tms_fx_forward_mark_to_market(
    const UmiOpenTmsFxForward *trade)
{
    if (trade == NULL || trade->baseNotional <= 0.0 ||
        trade->forwardRate <= 0.0 || trade->marketForwardRate <= 0.0)
        return 0.0;
    return trade->baseNotional *
        (trade->marketForwardRate - trade->forwardRate);
}

#endif

/* A public value may arrive from a decoder or application-owned storage.
 * Bound text before comparison and reject non-finite financial observations.
 * This book stores lifecycle evidence; it is not authentication or settlement. */
static bool ForwardText(const char *text, size_t capacity) {
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL || end == text) return false;
    bool nonSpace = false;
    for (const char *p = text; p < end; ++p) {
        unsigned char c = (unsigned char)*p;
        if (c < 32U || c == 127U) return false;
        if (c != (unsigned char)' ') nonSpace = true;
    }
    return nonSpace;
}
static bool ForwardCurrency(const char value[8]) {
    return value[0] >= 'A' && value[0] <= 'Z' &&
        value[1] >= 'A' && value[1] <= 'Z' &&
        value[2] >= 'A' && value[2] <= 'Z' && value[3] == '\0';
}
static bool ForwardState(UmiOpenTmsFxForwardState state) {
    return state >= UMI_OPEN_TMS_FX_FORWARD_DRAFT && state <= UMI_OPEN_TMS_FX_FORWARD_CANCELLED;
}
static bool ForwardValue(const UmiOpenTmsFxForward *trade) {
    return trade != NULL && ForwardText(trade->tradeId, sizeof trade->tradeId) &&
        ForwardText(trade->accountId, sizeof trade->accountId) &&
        ForwardText(trade->counterpartyId, sizeof trade->counterpartyId) &&
        ForwardCurrency(trade->baseCurrency) && ForwardCurrency(trade->quoteCurrency) &&
        strcmp(trade->baseCurrency, trade->quoteCurrency) != 0 &&
        isfinite(trade->baseNotional) && trade->baseNotional > 0.0 &&
        isfinite(trade->forwardRate) && trade->forwardRate > 0.0 &&
        isfinite(trade->marketForwardRate) && trade->marketForwardRate >= 0.0 &&
        ForwardState(trade->state);
}
static bool ForwardBook(const UmiOpenTmsFxForwardBook *book) {
    if (book == NULL || book->count > UMI_OPEN_TMS_FX_FORWARD_CAPACITY || book->revision == 0U)
        return false;
    /* Earlier row-only validation is retained for review. Unique identities
     * are also required so a restored public book cannot target an ambiguous row. */
#if 0
    for (size_t i = 0U; i < book->count; ++i)
        if (!ForwardValue(&book->trades[i])) return false;
#endif
    for (size_t i = 0U; i < book->count; ++i) {
        if (!ForwardValue(&book->trades[i])) return false;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(book->trades[i].tradeId, book->trades[j].tradeId) == 0) return false;
    }
    return true;
}
void umi_open_tms_fx_forward_book_init(UmiOpenTmsFxForwardBook *book) {
    if (book == NULL) return;
    memset(book, 0, sizeof *book);
    book->revision = 1U;
}
const UmiOpenTmsFxForward *umi_open_tms_fx_forward_book_find(
    const UmiOpenTmsFxForwardBook *book, const char *tradeId) {
    if (tradeId == NULL || !ForwardBook(book)) return NULL;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->trades[i].tradeId, tradeId) == 0) return &book->trades[i];
    return NULL;
}
UmiStatus umi_open_tms_fx_forward_book_add(
    UmiOpenTmsFxForwardBook *book, const UmiOpenTmsFxForward *trade) {
    if (book == NULL || trade == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!ForwardBook(book)) return UMI_STATUS_INVALID_STATE;
    if (!ForwardValue(trade)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_open_tms_fx_forward_book_find(book, trade->tradeId) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (book->count == UMI_OPEN_TMS_FX_FORWARD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count] = *trade;
    ++book->count;
    ++book->revision;
    return UMI_STATUS_OK;
}
/* Importers and native consumers use the same admission boundary as mutation. */
UmiStatus umi_open_tms_fx_forward_book_validate(const UmiOpenTmsFxForwardBook *book) {
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return ForwardBook(book) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
static bool ForwardTransition(UmiOpenTmsFxForwardState from, UmiOpenTmsFxForwardState to) {
    switch (from) {
    case UMI_OPEN_TMS_FX_FORWARD_DRAFT:
        return to == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED || to == UMI_OPEN_TMS_FX_FORWARD_CANCELLED;
    case UMI_OPEN_TMS_FX_FORWARD_CONFIRMED:
        return to == UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING || to == UMI_OPEN_TMS_FX_FORWARD_CANCELLED;
    case UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING:
        return to == UMI_OPEN_TMS_FX_FORWARD_SETTLED || to == UMI_OPEN_TMS_FX_FORWARD_CANCELLED;
    default:
        return false;
    }
}
UmiStatus umi_open_tms_fx_forward_book_transition(
    UmiOpenTmsFxForwardBook *book, const char *tradeId, UmiOpenTmsFxForwardState state, uint64_t version) {
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!ForwardBook(book)) return UMI_STATUS_INVALID_STATE;
    if (!ForwardState(state)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiOpenTmsFxForward *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) != 0) continue;
        if (version <= trade->version || !ForwardTransition(trade->state, state))
            return UMI_STATUS_INVALID_STATE;
        trade->state = state;
        trade->version = version;
        ++book->revision;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
/* Price evidence stays deliberately separate from money posting. This simple
 * undiscounted observation has no curve, FX conversion or collateral model.
 * A status-bearing interface avoids treating an unavailable quote as zero. */
UmiStatus umi_open_tms_fx_forward_mark_to_market_checked(
    const UmiOpenTmsFxForward *trade, double *outValue) {
    if (trade == NULL || outValue == NULL ||
        !isfinite(trade->baseNotional) || trade->baseNotional <= 0.0 ||
        !isfinite(trade->forwardRate) || trade->forwardRate <= 0.0 ||
        !isfinite(trade->marketForwardRate) || trade->marketForwardRate < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (trade->marketForwardRate == 0.0) return UMI_STATUS_UNAVAILABLE;
    double spread = trade->marketForwardRate - trade->forwardRate;
    if (!isfinite(spread)) return UMI_STATUS_CAPACITY_EXCEEDED;
    double value = trade->baseNotional * spread;
    if (!isfinite(value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outValue = value;
    return UMI_STATUS_OK;
}
/* Keep the scalar API for existing consumers. New code uses the checked
 * interface so a refused observation cannot masquerade as a measured zero. */
double umi_open_tms_fx_forward_mark_to_market(const UmiOpenTmsFxForward *trade) {
    double value = 0.0;
    (void)umi_open_tms_fx_forward_mark_to_market_checked(trade, &value);
    return value;
}
