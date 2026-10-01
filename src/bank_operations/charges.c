/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/charges.c
 * PURPOSE: Post reviewed fixed account charges through the existing journal and event owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* All mutations target the normal disposable BankState candidate. The shared
 * repository transaction publishes it only after every invariant succeeds. */
UmiStatus BankApplyCharge(BankState *state, const UmiBankActor *actor,
    const UmiBankCommand *command)
{
    int index = BankFindCharge(state, command->id.value);
    UmiBankChargeRequest *request;
    UmiStatus status;
    if (command->action == UMI_BANK_CHARGE_SUBMIT) {
        int accountIndex = BankFindAccount(state, command->sourceAccountId.value);
        if (index >= 0) return UMI_STATUS_ALREADY_EXISTS;
        status = BankAccountActive(state, accountIndex);
        if (status != UMI_STATUS_OK) return status;
        if (command->amount.minor_units <= 0 || !BankMoneyMatches(command->amount, &state->accounts[accountIndex]))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < state->counts.chargeRequests; ++i) {
            const UmiBankChargeRequest *prior = &state->chargeRequests[i];
            if (strcmp(prior->accountId.value, command->sourceAccountId.value) == 0 &&
                strcmp(prior->referenceId.value, command->ownerId.value) == 0 &&
                (prior->state == UMI_BANK_TRANSFER_PENDING || prior->state == UMI_BANK_TRANSFER_APPROVED ||
                 prior->state == UMI_BANK_TRANSFER_EXECUTED)) return UMI_STATUS_ALREADY_EXISTS;
        }
        if (state->counts.chargeRequests >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        request = &state->chargeRequests[state->counts.chargeRequests++];
        memset(request, 0, sizeof(*request));
        request->id = command->id; request->referenceId = command->ownerId;
        request->accountId = command->sourceAccountId; request->makerId = actor->id;
        memcpy(request->reason, command->name, sizeof(request->reason));
        request->amount = command->amount; request->state = UMI_BANK_TRANSFER_PENDING;
        request->submittedRevision = state->counts.revision + 1U;
        return UMI_STATUS_OK;
    }
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    request = &state->chargeRequests[index];
    switch (command->action) {
    case UMI_BANK_CHARGE_APPROVE: case UMI_BANK_CHARGE_REJECT:
        if (request->state != UMI_BANK_TRANSFER_PENDING) return UMI_STATUS_INVALID_STATE;
        if (strcmp(actor->id.value, request->makerId.value) == 0) return UMI_STATUS_PERMISSION_DENIED;
        if (command->action == UMI_BANK_CHARGE_APPROVE) {
            status = BankAccountActive(state, BankFindAccount(state, request->accountId.value));
            if (status != UMI_STATUS_OK) return status;
            request->state = UMI_BANK_TRANSFER_APPROVED;
        } else request->state = UMI_BANK_TRANSFER_REJECTED;
        request->checkerId = actor->id;
        return UMI_STATUS_OK;
    case UMI_BANK_CHARGE_CANCEL:
        if (request->state != UMI_BANK_TRANSFER_PENDING && request->state != UMI_BANK_TRANSFER_APPROVED)
            return UMI_STATUS_INVALID_STATE;
        if (strcmp(actor->id.value, request->makerId.value) != 0 &&
            (actor->capabilities & UMI_BANK_CAP_OPERATE) == 0U) return UMI_STATUS_PERMISSION_DENIED;
        request->state = UMI_BANK_TRANSFER_CANCELLED;
        return UMI_STATUS_OK;
    case UMI_BANK_CHARGE_POST: case UMI_BANK_CHARGE_REVERSE: {
        UmiBankBalance balance;
        char income[UMI_FINANCE_ID_CAPACITY];
        if (command->action == UMI_BANK_CHARGE_POST) {
            if (request->state != UMI_BANK_TRANSFER_APPROVED || request->checkerId.value[0] == '\0' ||
                strcmp(request->makerId.value, request->checkerId.value) == 0) return UMI_STATUS_INVALID_STATE;
        } else if (request->state != UMI_BANK_TRANSFER_EXECUTED) return UMI_STATUS_INVALID_STATE;
        int accountIndex = BankFindAccount(state, request->accountId.value);
        status = BankAccountActive(state, accountIndex);
        if (status != UMI_STATUS_OK) return status;
        (void)snprintf(income, sizeof(income), "sys.charges.%.3s", request->amount.currency.code);
        if (command->action == UMI_BANK_CHARGE_POST) {
            /* Approval is not a reservation: outstanding holds still protect
             * their funds when this separate debit is posted. */
            status = BankRequireFunds(state, accountIndex, request->amount.minor_units);
            if (status == UMI_STATUS_OK)
                status = BankPost(state, command, request->accountId.value, income, request->amount, false);
            if (status == UMI_STATUS_OK) {
                request->state = UMI_BANK_TRANSFER_EXECUTED;
                request->postedRevision = state->counts.revision + 1U;
            }
        } else {
            status = BankProjectBalance(state, request->accountId.value, &balance);
            if (status == UMI_STATUS_OK && balance.booked.minor_units > INT64_MAX - request->amount.minor_units)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            if (status == UMI_STATUS_OK)
                status = BankPost(state, command, income, request->accountId.value, request->amount, true);
            if (status == UMI_STATUS_OK) {
                request->state = UMI_BANK_TRANSFER_REVERSED;
                request->reversedRevision = state->counts.revision + 1U;
            }
        }
        return status;
    }
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
}
