/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/interest.c
 * PURPOSE: Apply reviewed practice interest through the canonical event and journal owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/finance/banking/interest_accrual.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* No parallel ledger or database is introduced. The normal candidate pipeline
 * owns this state and discards it if validation, review or persistence fails. */
UmiStatus BankApplyInterest(BankState *state, const UmiBankActor *actor,
    const UmiBankCommand *command)
{
    int index = BankFindInterest(state, command->id.value);
    UmiBankInterestRequest *request;
    UmiStatus status;
    UmiBankBalance balance;
    char expense[UMI_FINANCE_ID_CAPACITY];
    if (command->action == UMI_BANK_INTEREST_SUBMIT) {
        UmiBankingInterestAccrual calculation;
        int64_t amount = 0;
        int accountIndex = BankFindAccount(state, command->sourceAccountId.value);
        if (index >= 0) return UMI_STATUS_ALREADY_EXISTS;
        status = BankAccountActive(state, accountIndex);
        if (status != UMI_STATUS_OK) return status;
        for (size_t i = 0U; i < state->counts.interestRequests; ++i) {
            const UmiBankInterestRequest *prior = &state->interestRequests[i];
            if (strcmp(prior->accountId.value, command->sourceAccountId.value) == 0 &&
                strcmp(prior->periodId.value, command->ownerId.value) == 0 &&
                (prior->state == UMI_BANK_TRANSFER_PENDING || prior->state == UMI_BANK_TRANSFER_APPROVED ||
                 prior->state == UMI_BANK_TRANSFER_EXECUTED)) return UMI_STATUS_ALREADY_EXISTS;
        }
        if (state->counts.interestRequests >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = BankProjectBalance(state, command->sourceAccountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        status = umi_banking_interest_accrual_init(&calculation, command->id.value,
            balance.booked.minor_units, command->interest.annualRateBps,
            command->interest.days, command->interest.dayCountBasis);
        if (status == UMI_STATUS_OK) status = umi_banking_interest_accrual_calculate(&calculation, &amount);
        if (status != UMI_STATUS_OK) return status;
        if (amount <= 0) return UMI_STATUS_INVALID_ARGUMENT;
        if (balance.booked.minor_units > INT64_MAX - amount) return UMI_STATUS_CAPACITY_EXCEEDED;
        request = &state->interestRequests[state->counts.interestRequests++];
        memset(request, 0, sizeof *request);
        request->id = command->id; request->periodId = command->ownerId;
        request->accountId = command->sourceAccountId; request->makerId = actor->id;
        request->terms = command->interest; request->principal = balance.booked;
        request->amount = balance.booked; request->amount.minor_units = amount;
        request->state = UMI_BANK_TRANSFER_PENDING;
        request->submittedRevision = state->counts.revision + 1U;
        return UMI_STATUS_OK;
    }
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    request = &state->interestRequests[index];
    switch (command->action) {
    case UMI_BANK_INTEREST_APPROVE: case UMI_BANK_INTEREST_REJECT:
        if (request->state != UMI_BANK_TRANSFER_PENDING) return UMI_STATUS_INVALID_STATE;
        if (strcmp(actor->id.value, request->makerId.value) == 0) return UMI_STATUS_PERMISSION_DENIED;
        if (command->action == UMI_BANK_INTEREST_APPROVE) {
            status = BankAccountActive(state, BankFindAccount(state, request->accountId.value));
            if (status != UMI_STATUS_OK) return status;
            request->state = UMI_BANK_TRANSFER_APPROVED;
        } else request->state = UMI_BANK_TRANSFER_REJECTED;
        request->checkerId = actor->id;
        return UMI_STATUS_OK;
    case UMI_BANK_INTEREST_CANCEL:
        if (request->state != UMI_BANK_TRANSFER_PENDING && request->state != UMI_BANK_TRANSFER_APPROVED)
            return UMI_STATUS_INVALID_STATE;
        if (strcmp(actor->id.value, request->makerId.value) != 0 &&
            (actor->capabilities & UMI_BANK_CAP_OPERATE) == 0U) return UMI_STATUS_PERMISSION_DENIED;
        request->state = UMI_BANK_TRANSFER_CANCELLED;
        return UMI_STATUS_OK;
    case UMI_BANK_INTEREST_POST: case UMI_BANK_INTEREST_REVERSE:
        if (command->action == UMI_BANK_INTEREST_POST) {
            if (request->state != UMI_BANK_TRANSFER_APPROVED || request->checkerId.value[0] == '\0' ||
                strcmp(request->makerId.value, request->checkerId.value) == 0) return UMI_STATUS_INVALID_STATE;
        } else if (request->state != UMI_BANK_TRANSFER_EXECUTED) return UMI_STATUS_INVALID_STATE;
        status = BankAccountActive(state, BankFindAccount(state, request->accountId.value));
        if (status != UMI_STATUS_OK) return status;
        status = BankProjectBalance(state, request->accountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        (void)snprintf(expense, sizeof expense, "sys.interest.%.3s", request->amount.currency.code);
        if (command->action == UMI_BANK_INTEREST_POST) {
            if (balance.booked.minor_units > INT64_MAX - request->amount.minor_units)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            status = BankPost(state, command, expense, request->accountId.value, request->amount, false);
            if (status == UMI_STATUS_OK) {
                request->state = UMI_BANK_TRANSFER_EXECUTED;
                request->postedRevision = state->counts.revision + 1U;
            }
        } else {
            status = BankRequireFunds(state, BankFindAccount(state, request->accountId.value), request->amount.minor_units);
            if (status == UMI_STATUS_OK)
                status = BankPost(state, command, request->accountId.value, expense, request->amount, true);
            if (status == UMI_STATUS_OK) {
                request->state = UMI_BANK_TRANSFER_REVERSED;
                request->reversedRevision = state->counts.revision + 1U;
            }
        }
        return status;
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
}
