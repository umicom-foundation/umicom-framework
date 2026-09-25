/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/transfers.c
 *
 * PURPOSE:
 *   Enforce reservation, maker/checker approval, transfer execution and reversal.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <limits.h>
#include <string.h>

static UmiStatus TransferAccountsActive(const BankState *state, const UmiBankTransfer *transfer)
{
    UmiStatus status;
    int beneficiaryIndex = BankFindBeneficiary(state, transfer->beneficiaryId.value);
    if (beneficiaryIndex < 0 || state->beneficiaries[beneficiaryIndex].state != UMI_BANK_RECORD_ACTIVE)
        return UMI_STATUS_INVALID_STATE;
    status = BankAccountActive(state, BankFindAccount(state, transfer->sourceAccountId.value));
    if (status == UMI_STATUS_OK)
        status = BankAccountActive(state, BankFindAccount(state, transfer->destinationAccountId.value));
    return status;
}

/* Transfers reserve funds at submission. Approval changes authority, not cash;
 * execution atomically releases that reservation and posts the journal. */
UmiStatus BankApplyTransfer(BankState *state, const UmiBankActor *actor,
                           const UmiBankCommand *command)
{
    int index = BankFindTransfer(state, command->id.value);
    UmiBankTransfer *transfer;
    UmiStatus status;
    UmiBankBalance balance;
    if (command->action == UMI_BANK_TRANSFER_SUBMIT) {
        int sourceIndex = BankFindAccount(state, command->sourceAccountId.value);
        int beneficiaryIndex = BankFindBeneficiary(state, command->ownerId.value);
        int destinationIndex;
        if (index >= 0) return UMI_STATUS_ALREADY_EXISTS;
        if (sourceIndex < 0 || beneficiaryIndex < 0) return UMI_STATUS_NOT_FOUND;
        destinationIndex = BankFindAccount(state, state->beneficiaries[beneficiaryIndex].accountId.value);
        if (destinationIndex < 0) return UMI_STATUS_NOT_FOUND;
        if (sourceIndex == destinationIndex || command->amount.minor_units <= 0 ||
            !BankMoneyMatches(command->amount, &state->accounts[sourceIndex]) ||
            !BankMoneyMatches(command->amount, &state->accounts[destinationIndex]))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (command->destinationAccountId.value[0] != '\0' &&
            strcmp(command->destinationAccountId.value, state->beneficiaries[beneficiaryIndex].accountId.value) != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(state->beneficiaries[beneficiaryIndex].customerId.value,
                   state->accounts[sourceIndex].account.party_id.value) != 0)
            return UMI_STATUS_PERMISSION_DENIED;
        if (state->counts.transfers >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        transfer = &state->transfers[state->counts.transfers];
        memset(transfer, 0, sizeof *transfer);
        transfer->id = command->id;
        transfer->beneficiaryId = command->ownerId;
        transfer->sourceAccountId = command->sourceAccountId;
        transfer->destinationAccountId = state->beneficiaries[beneficiaryIndex].accountId;
        transfer->makerId = actor->id;
        transfer->amount = command->amount;
        status = TransferAccountsActive(state, transfer);
        if (status == UMI_STATUS_OK) status = BankRequireFunds(state, sourceIndex, command->amount.minor_units);
        if (status != UMI_STATUS_OK) return status;
        transfer->state = UMI_BANK_TRANSFER_PENDING;
        transfer->submittedRevision = state->counts.revision + 1U;
        ++state->counts.transfers;
        return UMI_STATUS_OK;
    }
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    transfer = &state->transfers[index];
    switch (command->action) {
    case UMI_BANK_TRANSFER_APPROVE:
    case UMI_BANK_TRANSFER_REJECT:
        if (transfer->state != UMI_BANK_TRANSFER_PENDING) return UMI_STATUS_INVALID_STATE;
        if (strcmp(actor->id.value, transfer->makerId.value) == 0) return UMI_STATUS_PERMISSION_DENIED;
        if (command->action == UMI_BANK_TRANSFER_APPROVE) {
            status = TransferAccountsActive(state, transfer);
            if (status != UMI_STATUS_OK) return status;
            transfer->state = UMI_BANK_TRANSFER_APPROVED;
        } else transfer->state = UMI_BANK_TRANSFER_REJECTED;
        transfer->checkerId = actor->id;
        return UMI_STATUS_OK;
    case UMI_BANK_TRANSFER_CANCEL:
        if (transfer->state != UMI_BANK_TRANSFER_PENDING && transfer->state != UMI_BANK_TRANSFER_APPROVED)
            return UMI_STATUS_INVALID_STATE;
        if (strcmp(actor->id.value, transfer->makerId.value) != 0 &&
            (actor->capabilities & UMI_BANK_CAP_OPERATE) == 0U) return UMI_STATUS_PERMISSION_DENIED;
        transfer->state = UMI_BANK_TRANSFER_CANCELLED;
        return UMI_STATUS_OK;
    case UMI_BANK_TRANSFER_EXECUTE:
        if (transfer->state != UMI_BANK_TRANSFER_APPROVED || transfer->checkerId.value[0] == '\0' ||
            strcmp(transfer->makerId.value, transfer->checkerId.value) == 0) return UMI_STATUS_INVALID_STATE;
        status = TransferAccountsActive(state, transfer);
        if (status != UMI_STATUS_OK) return status;
        status = BankProjectBalance(state, transfer->destinationAccountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        if (balance.booked.minor_units > INT64_MAX - transfer->amount.minor_units)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        /* Only the private candidate changes before persistence. On any later
         * failure the caller discards it, including this reservation release. */
        transfer->state = UMI_BANK_TRANSFER_EXECUTED;
        status = BankRequireFunds(state, BankFindAccount(state, transfer->sourceAccountId.value),
                                  transfer->amount.minor_units);
        if (status != UMI_STATUS_OK) return status;
        status = BankPost(state, command, transfer->sourceAccountId.value,
                          transfer->destinationAccountId.value, transfer->amount, false);
        if (status == UMI_STATUS_OK) transfer->postedRevision = state->counts.revision + 1U;
        return status;
    case UMI_BANK_TRANSFER_REVERSE:
        if (transfer->state != UMI_BANK_TRANSFER_EXECUTED) return UMI_STATUS_INVALID_STATE;
        /* A reversal does not need an active beneficiary, but both bank
         * accounts must still be eligible and the recipient must have funds. */
        status = BankAccountActive(state, BankFindAccount(state, transfer->sourceAccountId.value));
        if (status == UMI_STATUS_OK)
            status = BankAccountActive(state, BankFindAccount(state, transfer->destinationAccountId.value));
        if (status == UMI_STATUS_OK)
            status = BankRequireFunds(state, BankFindAccount(state, transfer->destinationAccountId.value),
                                      transfer->amount.minor_units);
        if (status != UMI_STATUS_OK) return status;
        status = BankProjectBalance(state, transfer->sourceAccountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        if (balance.booked.minor_units > INT64_MAX - transfer->amount.minor_units)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = BankPost(state, command, transfer->destinationAccountId.value,
                          transfer->sourceAccountId.value, transfer->amount, true);
        if (status == UMI_STATUS_OK) transfer->state = UMI_BANK_TRANSFER_REVERSED;
        return status;
    default:
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
}
