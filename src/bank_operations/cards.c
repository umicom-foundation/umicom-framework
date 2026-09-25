/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/cards.c
 *
 * PURPOSE:
 *   Implement manual reservations and a clearly separated local card simulator.
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

/* All card commands are the local simulator. No PAN, security code, network
 * credential or claim of live provider acceptance is created by this module. */
UmiStatus BankApplyCardHold(BankState *state, const UmiBankActor *actor,
                          const UmiBankCommand *command)
{
    UmiStatus status;
    int index, accountIndex, cardIndex = -1;
    UmiBankHold *hold;
    UmiBankBalance balance;
    (void)actor;
    if (command->action == UMI_BANK_CARD_ISSUE) {
        UmiBankCard *card;
        if (BankFindCard(state, command->id.value) >= 0) return UMI_STATUS_ALREADY_EXISTS;
        accountIndex = BankFindAccount(state, command->sourceAccountId.value);
        status = BankAccountActive(state, accountIndex);
        if (status != UMI_STATUS_OK) return status;
        if (command->amount.minor_units <= 0 || command->name[0] == '\0' ||
            !BankMoneyMatches(command->amount, &state->accounts[accountIndex])) return UMI_STATUS_INVALID_ARGUMENT;
        if (state->counts.cards >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        card = &state->cards[state->counts.cards++];
        card->id = command->id;
        card->accountId = command->sourceAccountId;
        card->perTransactionLimit = command->amount;
        memcpy(card->name, command->name, sizeof card->name);
        card->state = UMI_BANK_RECORD_ACTIVE;
        return UMI_STATUS_OK;
    }
    if (command->action == UMI_BANK_CARD_SET_STATE) {
        index = BankFindCard(state, command->id.value);
        if (index < 0) return UMI_STATUS_NOT_FOUND;
        if (command->state < UMI_BANK_RECORD_ACTIVE || command->state > UMI_BANK_RECORD_CLOSED ||
            state->cards[index].state == UMI_BANK_RECORD_CLOSED || command->state == state->cards[index].state)
            return UMI_STATUS_INVALID_STATE;
        if (command->state == UMI_BANK_RECORD_CLOSED)
            for (size_t i = 0U; i < state->counts.holds; ++i)
                if (state->holds[i].state == UMI_BANK_HOLD_ACTIVE &&
                    strcmp(state->holds[i].cardId.value, command->id.value) == 0) return UMI_STATUS_BUSY;
        state->cards[index].state = command->state;
        return UMI_STATUS_OK;
    }
    if (command->action == UMI_BANK_HOLD_PLACE || command->action == UMI_BANK_CARD_AUTHORISE) {
        if (BankFindHold(state, command->id.value) >= 0) return UMI_STATUS_ALREADY_EXISTS;
        if (state->counts.holds >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (command->action == UMI_BANK_CARD_AUTHORISE) {
            cardIndex = BankFindCard(state, command->ownerId.value);
            if (cardIndex < 0) return UMI_STATUS_NOT_FOUND;
            if (state->cards[cardIndex].state != UMI_BANK_RECORD_ACTIVE) return UMI_STATUS_INVALID_STATE;
            accountIndex = BankFindAccount(state, state->cards[cardIndex].accountId.value);
            if (command->sourceAccountId.value[0] != '\0' &&
                strcmp(command->sourceAccountId.value, state->cards[cardIndex].accountId.value) != 0)
                return UMI_STATUS_INVALID_ARGUMENT;
            if (command->amount.minor_units > state->cards[cardIndex].perTransactionLimit.minor_units)
                return UMI_STATUS_PERMISSION_DENIED;
        } else accountIndex = BankFindAccount(state, command->sourceAccountId.value);
        status = BankAccountActive(state, accountIndex);
        if (status != UMI_STATUS_OK) return status;
        if (command->amount.minor_units <= 0 || !BankMoneyMatches(command->amount, &state->accounts[accountIndex]))
            return UMI_STATUS_INVALID_ARGUMENT;
        status = BankRequireFunds(state, accountIndex, command->amount.minor_units);
        if (status != UMI_STATUS_OK) return status;
        hold = &state->holds[state->counts.holds++];
        memset(hold, 0, sizeof *hold);
        hold->id = command->id;
        hold->accountId = state->accounts[accountIndex].account.account_id;
        if (cardIndex >= 0) hold->cardId = state->cards[cardIndex].id;
        hold->amount = command->amount;
        hold->state = UMI_BANK_HOLD_ACTIVE;
        return UMI_STATUS_OK;
    }
    index = BankFindHold(state, command->id.value);
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    hold = &state->holds[index];
    if (command->action == UMI_BANK_HOLD_RELEASE || command->action == UMI_BANK_CARD_VOID) {
        if (hold->state != UMI_BANK_HOLD_ACTIVE ||
            (command->action == UMI_BANK_HOLD_RELEASE && hold->cardId.value[0] != '\0') ||
            (command->action == UMI_BANK_CARD_VOID && hold->cardId.value[0] == '\0'))
            return UMI_STATUS_INVALID_STATE;
        hold->state = UMI_BANK_HOLD_RELEASED;
        return UMI_STATUS_OK;
    }
    if (hold->cardId.value[0] == '\0') return UMI_STATUS_INVALID_STATE;
    accountIndex = BankFindAccount(state, hold->accountId.value);
    status = BankAccountActive(state, accountIndex);
    if (status != UMI_STATUS_OK) return status;
    if (command->action == UMI_BANK_CARD_CAPTURE) {
        char systemAccount[UMI_FINANCE_ID_CAPACITY];
        if (hold->state != UMI_BANK_HOLD_ACTIVE) return UMI_STATUS_INVALID_STATE;
        if (command->amount.minor_units <= 0 || command->amount.minor_units > hold->amount.minor_units ||
            !BankMoneyMatches(command->amount, &state->accounts[accountIndex])) return UMI_STATUS_INVALID_ARGUMENT;
        /* One final capture is supported. Any unused authorisation is released
         * in the same transaction; repeated and multiple captures are rejected. */
        hold->state = UMI_BANK_HOLD_CAPTURED;
        status = BankRequireFunds(state, accountIndex, command->amount.minor_units);
        if (status != UMI_STATUS_OK) return status;
        (void)snprintf(systemAccount, sizeof systemAccount, "sys.card.%.3s", command->amount.currency.code);
        status = BankPost(state, command, hold->accountId.value, systemAccount, command->amount, false);
        if (status == UMI_STATUS_OK) {
            hold->capturedMinor = command->amount.minor_units;
            hold->postedRevision = state->counts.revision + 1U;
        }
        return status;
    }
    if (command->action == UMI_BANK_CARD_REFUND) {
        UmiMoney amount = hold->amount;
        char systemAccount[UMI_FINANCE_ID_CAPACITY];
        if (hold->state != UMI_BANK_HOLD_CAPTURED) return UMI_STATUS_INVALID_STATE;
        amount.minor_units = hold->capturedMinor;
        status = BankProjectBalance(state, hold->accountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        if (balance.booked.minor_units > INT64_MAX - amount.minor_units) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)snprintf(systemAccount, sizeof systemAccount, "sys.card.%.3s", amount.currency.code);
        status = BankPost(state, command, systemAccount, hold->accountId.value, amount, true);
        if (status == UMI_STATUS_OK) hold->state = UMI_BANK_HOLD_REFUNDED;
        return status;
    }
    return UMI_STATUS_NOT_IMPLEMENTED;
}
