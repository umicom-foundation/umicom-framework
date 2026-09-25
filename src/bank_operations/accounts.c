/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/accounts.c
 *
 * PURPOSE:
 *   Implement customer, account, beneficiary, test-funding and reconciliation workflows.
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
#include <stdio.h>
#include <string.h>

static bool StateChangeValid(UmiBankRecordState before, UmiBankRecordState after)
{
    return before != UMI_BANK_RECORD_CLOSED && before != after &&
        after >= UMI_BANK_RECORD_ACTIVE && after <= UMI_BANK_RECORD_CLOSED;
}

/* Customer and account lifecycle belong to Framework. Application views only
 * submit commands, then refresh copies from the committed state. */
UmiStatus BankApplyCustomerAccount(BankState *state, const UmiBankActor *actor,
                                 const UmiBankCommand *command)
{
    int index, ownerIndex;
    UmiStatus status;
    UmiBankBalance balance;
    (void)actor;
    switch (command->action) {
    case UMI_BANK_CUSTOMER_CREATE: {
        UmiBankCustomer *customer;
        if (command->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
        if (BankFindCustomer(state, command->id.value) >= 0) return UMI_STATUS_ALREADY_EXISTS;
        if (state->counts.customers >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        customer = &state->customers[state->counts.customers++];
        customer->party.party_id = command->id;
        memcpy(customer->party.display_name, command->name, sizeof command->name);
        customer->state = UMI_BANK_RECORD_ACTIVE;
        return UMI_STATUS_OK;
    }
    case UMI_BANK_CUSTOMER_SET_STATE:
        index = BankFindCustomer(state, command->id.value);
        if (index < 0) return UMI_STATUS_NOT_FOUND;
        if (!StateChangeValid(state->customers[index].state, command->state)) return UMI_STATUS_INVALID_STATE;
        if (command->state == UMI_BANK_RECORD_CLOSED)
            for (size_t i = 0U; i < state->counts.accounts; ++i)
                if (strcmp(state->accounts[i].account.party_id.value, command->id.value) == 0 &&
                    state->accounts[i].state != UMI_BANK_RECORD_CLOSED) return UMI_STATUS_BUSY;
        state->customers[index].state = command->state;
        return UMI_STATUS_OK;
    case UMI_BANK_ACCOUNT_OPEN: {
        UmiBankAccount *account;
        UmiCurrency currency;
        if (command->name[0] == '\0' || command->amount.minor_units != 0 ||
            umi_accounting_currency_from_code(command->amount.currency.code, &currency) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (BankFindAccount(state, command->id.value) >= 0) return UMI_STATUS_ALREADY_EXISTS;
        ownerIndex = BankFindCustomer(state, command->ownerId.value);
        if (ownerIndex < 0) return UMI_STATUS_NOT_FOUND;
        if (state->customers[ownerIndex].state != UMI_BANK_RECORD_ACTIVE) return UMI_STATUS_INVALID_STATE;
        if (state->counts.accounts >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        /* One explicit scale per currency in this bank book. No inferred FX
         * conversion, no implicit two-decimal assumption and no mixed scales. */
        for (size_t i = 0U; i < state->counts.accounts; ++i)
            if (umi_accounting_currency_equal(currency, state->accounts[i].account.currency) &&
                command->amount.scale != state->accounts[i].scale) return UMI_STATUS_INVALID_ARGUMENT;
        account = &state->accounts[state->counts.accounts++];
        account->account.account_id = command->id;
        account->account.party_id = command->ownerId;
        account->account.currency = currency;
        account->scale = command->amount.scale;
        memcpy(account->name, command->name, sizeof account->name);
        account->state = UMI_BANK_RECORD_ACTIVE;
        return UMI_STATUS_OK;
    }
    case UMI_BANK_ACCOUNT_SET_STATE:
        index = BankFindAccount(state, command->id.value);
        if (index < 0) return UMI_STATUS_NOT_FOUND;
        if (!StateChangeValid(state->accounts[index].state, command->state)) return UMI_STATUS_INVALID_STATE;
        if (command->state == UMI_BANK_RECORD_CLOSED) {
            status = BankProjectBalance(state, command->id.value, &balance);
            if (status != UMI_STATUS_OK) return status;
            if (balance.booked.minor_units != 0 || balance.reserved.minor_units != 0) return UMI_STATUS_BUSY;
            for (size_t i = 0U; i < state->counts.transfers; ++i) {
                const UmiBankTransfer *transfer = &state->transfers[i];
                if ((transfer->state == UMI_BANK_TRANSFER_PENDING || transfer->state == UMI_BANK_TRANSFER_APPROVED) &&
                    (strcmp(transfer->sourceAccountId.value, command->id.value) == 0 ||
                     strcmp(transfer->destinationAccountId.value, command->id.value) == 0)) return UMI_STATUS_BUSY;
            }
        }
        state->accounts[index].state = command->state;
        return UMI_STATUS_OK;
    case UMI_BANK_BENEFICIARY_CREATE: {
        UmiBankBeneficiary *beneficiary;
        if (command->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
        if (BankFindBeneficiary(state, command->id.value) >= 0) return UMI_STATUS_ALREADY_EXISTS;
        ownerIndex = BankFindCustomer(state, command->ownerId.value);
        index = BankFindAccount(state, command->destinationAccountId.value);
        if (ownerIndex < 0 || index < 0) return UMI_STATUS_NOT_FOUND;
        if (state->customers[ownerIndex].state != UMI_BANK_RECORD_ACTIVE) return UMI_STATUS_INVALID_STATE;
        status = BankAccountActive(state, index);
        if (status != UMI_STATUS_OK) return status;
        if (state->counts.beneficiaries >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        beneficiary = &state->beneficiaries[state->counts.beneficiaries++];
        beneficiary->id = command->id;
        beneficiary->customerId = command->ownerId;
        beneficiary->accountId = command->destinationAccountId;
        memcpy(beneficiary->name, command->name, sizeof beneficiary->name);
        beneficiary->state = UMI_BANK_RECORD_ACTIVE;
        return UMI_STATUS_OK;
    }
    case UMI_BANK_BENEFICIARY_SET_STATE:
        index = BankFindBeneficiary(state, command->id.value);
        if (index < 0) return UMI_STATUS_NOT_FOUND;
        if (!StateChangeValid(state->beneficiaries[index].state, command->state)) return UMI_STATUS_INVALID_STATE;
        state->beneficiaries[index].state = command->state;
        return UMI_STATUS_OK;
    case UMI_BANK_TEST_CREDIT: {
        char systemAccount[UMI_FINANCE_ID_CAPACITY];
        index = BankFindAccount(state, command->sourceAccountId.value);
        status = BankAccountActive(state, index);
        if (status != UMI_STATUS_OK) return status;
        if (command->amount.minor_units <= 0 || !BankMoneyMatches(command->amount, &state->accounts[index]))
            return UMI_STATUS_INVALID_ARGUMENT;
        status = BankProjectBalance(state, command->sourceAccountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        if (balance.booked.minor_units > INT64_MAX - command->amount.minor_units)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)snprintf(systemAccount, sizeof systemAccount, "sys.funding.%.3s", command->amount.currency.code);
        return BankPost(state, command, systemAccount, command->sourceAccountId.value, command->amount, false);
    }
    case UMI_BANK_RECONCILE: {
        UmiBankReconciliation *reconciliation;
        index = BankFindAccount(state, command->sourceAccountId.value);
        if (index < 0) return UMI_STATUS_NOT_FOUND;
        if (!BankMoneyMatches(command->amount, &state->accounts[index])) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < state->counts.reconciliations; ++i)
            if (strcmp(state->reconciliations[i].id.value, command->id.value) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        if (state->counts.reconciliations >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = BankProjectBalance(state, command->sourceAccountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        reconciliation = &state->reconciliations[state->counts.reconciliations++];
        reconciliation->id = command->id;
        reconciliation->accountId = command->sourceAccountId;
        reconciliation->externalBalance = command->amount;
        reconciliation->bookedBalance = balance.booked;
        reconciliation->revision = state->counts.revision + 1U;
        reconciliation->matched = command->amount.minor_units == balance.booked.minor_units;
        /* A break is successful reconciliation evidence, not permission to
         * rewrite the ledger until it happens to match an external figure. */
        return UMI_STATUS_OK;
    }
    default:
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
}
