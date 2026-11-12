/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/payment_control_book.c
 *
 * PURPOSE:
 *   Implement controlled payment lifecycle transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/financial_platform/payment_control_book.h"
#include <string.h>

#include "control_query.h"

/* Retained for review: the former owner trusted public array bounds and inline
 * identities. Framework now admits every active row before lookup or mutation,
 * retains existing lifecycle rules and publishes checked observations atomically. */
#if 0
void umi_financial_payment_control_book_init(UmiFinancialPaymentControlBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialPaymentControl *umi_financial_payment_control_book_find(
    const UmiFinancialPaymentControlBook *book,
    const char *paymentId)
{
    size_t i;
    if (book == NULL || paymentId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->payments[i].paymentId, paymentId) == 0)
            return &book->payments[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_payment_control_book_add(
    UmiFinancialPaymentControlBook *book,
    const UmiFinancialPaymentControl *payment)
{
    if (book == NULL || payment == NULL || payment->paymentId[0] == '\0' ||
        payment->debtorAccountId[0] == '\0' || payment->creditorPartyId[0] == '\0' ||
        payment->currency[0] == '\0' || payment->amount <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_payment_control_book_find(book, payment->paymentId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->payments[book->count++] = *payment;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_financial_payment_control_book_add(
    UmiFinancialPaymentControlBook *book,
    const UmiFinancialPaymentControl *payment)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (book == NULL || payment == NULL || payment->paymentId[0] == '\0' ||
        payment->debtorAccountId[0] == '\0' || payment->creditorPartyId[0] == '\0' ||
        payment->currency[0] == '\0' || payment->amount <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_payment_control_book_find(book, payment->paymentId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->payments[book->count++] = *payment;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_payment_control_book_transition(
    UmiFinancialPaymentControlBook *book,
    const char *paymentId,
    UmiFinancialPaymentControlState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || paymentId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialPaymentControl *payment = &book->payments[i];
        if (strcmp(payment->paymentId, paymentId) == 0) {
            if (version <= payment->version) return UMI_STATUS_INVALID_STATE;
            if (payment->state == UMI_FINANCIAL_PAYMENT_SETTLED ||
                payment->state == UMI_FINANCIAL_PAYMENT_REJECTED)
                return UMI_STATUS_INVALID_STATE;
            if ((state == UMI_FINANCIAL_PAYMENT_SENT ||
                 state == UMI_FINANCIAL_PAYMENT_SETTLED) &&
                !payment->approved)
                return UMI_STATUS_PERMISSION_DENIED;
            payment->state = state;
            payment->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
#endif

UmiStatus umi_financial_payment_control_book_transition(
    UmiFinancialPaymentControlBook *book,
    const char *paymentId,
    UmiFinancialPaymentControlState state,
    uint64_t version)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || paymentId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialPaymentControl *payment = &book->payments[i];
        if (strcmp(payment->paymentId, paymentId) == 0) {
            if (version <= payment->version) return UMI_STATUS_INVALID_STATE;
            if (payment->state == UMI_FINANCIAL_PAYMENT_SETTLED ||
                payment->state == UMI_FINANCIAL_PAYMENT_REJECTED)
                return UMI_STATUS_INVALID_STATE;
            if ((state == UMI_FINANCIAL_PAYMENT_SENT ||
                 state == UMI_FINANCIAL_PAYMENT_SETTLED) &&
                !payment->approved)
                return UMI_STATUS_PERMISSION_DENIED;
            payment->state = state;
            payment->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

#endif

UmiStatus umi_financial_payment_control_validate(const UmiFinancialPaymentControl *row)
{
    static const UmiSnapshotTextField fields[] = {
        {"paymentId", offsetof(UmiFinancialPaymentControl, paymentId), sizeof(((UmiFinancialPaymentControl *)0)->paymentId), 1},
        {"debtorAccountId", offsetof(UmiFinancialPaymentControl, debtorAccountId), sizeof(((UmiFinancialPaymentControl *)0)->debtorAccountId), 1},
        {"creditorPartyId", offsetof(UmiFinancialPaymentControl, creditorPartyId), sizeof(((UmiFinancialPaymentControl *)0)->creditorPartyId), 1},
        {"currency", offsetof(UmiFinancialPaymentControl, currency), sizeof(((UmiFinancialPaymentControl *)0)->currency), 1}
    };
    if (row == NULL || UmiSnapshotValidateTextFields(row, sizeof(*row), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(row->amount) ||
        (row->approved != 0 && row->approved != 1) ||
        row->amount <= 0.0 ||
        (row->state < UMI_FINANCIAL_PAYMENT_DRAFT || row->state > UMI_FINANCIAL_PAYMENT_REJECTED)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_payment_control_book_validate(const UmiFinancialPaymentControlBook *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->count > UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (owner->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialPaymentControl *row = &owner->payments[i];
        if (umi_financial_payment_control_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            const UmiFinancialPaymentControl *previous = &owner->payments[j];
            if (strcmp(row->paymentId, previous->paymentId) == 0) return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
void umi_financial_payment_control_book_init(UmiFinancialPaymentControlBook *owner)
{
    if (owner == NULL) return;
    memset(owner, 0, sizeof(*owner));
    owner->revision = 1U;
}
const UmiFinancialPaymentControl *umi_financial_payment_control_book_find(const UmiFinancialPaymentControlBook *owner,
    const char *paymentId)
{
    char paymentIdKey[64];
    if (FinancialControlQuery(paymentId, paymentIdKey, sizeof paymentIdKey) != UMI_STATUS_OK) return NULL;
    if (umi_financial_payment_control_book_validate(owner) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialPaymentControl *row = &owner->payments[i];
        if (strcmp(row->paymentId, paymentIdKey) == 0) return row;
    }
    return NULL;
}
UmiStatus umi_financial_payment_control_book_add(UmiFinancialPaymentControlBook *owner, const UmiFinancialPaymentControl *row)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_payment_control_book_validate(owner);
    if (status == UMI_STATUS_OK) status = umi_financial_payment_control_validate(row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialPaymentControl *current = &owner->payments[i];
        if (strcmp(current->paymentId, row->paymentId) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (owner->count == UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    owner->payments[owner->count++] = *row;
    ++owner->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_payment_control_book_transition(UmiFinancialPaymentControlBook *owner, const char *paymentId,
    UmiFinancialPaymentControlState state, uint64_t version)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (state < UMI_FINANCIAL_PAYMENT_DRAFT || state > UMI_FINANCIAL_PAYMENT_REJECTED) return UMI_STATUS_INVALID_ARGUMENT;
    char key[64];
    UmiStatus status = FinancialControlQuery(paymentId, key, sizeof key);
    if (status == UMI_STATUS_OK) status = umi_financial_payment_control_book_validate(owner);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialPaymentControl *row = &owner->payments[i];
        if (strcmp(row->paymentId, key) == 0) {
            if (version <= row->version || row->state == UMI_FINANCIAL_PAYMENT_SETTLED || row->state == UMI_FINANCIAL_PAYMENT_REJECTED)
                return UMI_STATUS_INVALID_STATE;
            if ((state == UMI_FINANCIAL_PAYMENT_SENT || state == UMI_FINANCIAL_PAYMENT_SETTLED) && !row->approved)
                return UMI_STATUS_PERMISSION_DENIED;
            /* Preserve known-state transitions and existing terminal rules.
             * Updating this observation performs no external financial action. */
            row->state = state;
            row->version = version;
            ++owner->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
