/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/validation.c
 *
 * PURPOSE:
 *   Validate banking commands, identities, capabilities and account eligibility.
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

/* Identifier policy adds a reserved system-account namespace to the canonical
 * bounded financial identifier. It does not introduce a second ID type. */
bool BankIdValid(const UmiFinancialId *id, bool required)
{
    const char *end;
    if (id == NULL) return false;
    end = memchr(id->value, '\0', sizeof id->value);
    if (end == NULL) return false;
    if (end == id->value) return !required;
    if (!umi_financial_id_is_valid(id) || strncmp(id->value, "sys.", 4U) == 0)
        return false;
    for (const char *p = id->value; p != end; ++p) {
        const unsigned char c = (unsigned char)*p;
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_'))
            return false;
    }
    return true;
}

/* Use one capability gate for direct execution and replay validation. */
static uint32_t RequiredCapability(UmiBankAction action)
{
    switch (action) {
    case UMI_BANK_CUSTOMER_CREATE: case UMI_BANK_CUSTOMER_SET_STATE:
    case UMI_BANK_ACCOUNT_OPEN: case UMI_BANK_ACCOUNT_SET_STATE:
    case UMI_BANK_BENEFICIARY_CREATE: case UMI_BANK_BENEFICIARY_SET_STATE:
    case UMI_BANK_CARD_ISSUE: case UMI_BANK_CARD_SET_STATE:
        return UMI_BANK_CAP_CUSTOMERS;
    case UMI_BANK_TRANSFER_SUBMIT:
        return UMI_BANK_CAP_PAYMENTS;
    case UMI_BANK_TRANSFER_CANCEL:
        return UMI_BANK_CAP_PAYMENTS | UMI_BANK_CAP_OPERATE;
    case UMI_BANK_TRANSFER_APPROVE: case UMI_BANK_TRANSFER_REJECT:
        return UMI_BANK_CAP_APPROVE;
    case UMI_BANK_TEST_CREDIT:
        return UMI_BANK_CAP_TEST_FUNDING;
    default:
        return UMI_BANK_CAP_OPERATE;
    }
}

uint32_t UmiBankActionFields(UmiBankAction action)
{
    switch (action) {
    case UMI_BANK_CUSTOMER_CREATE: return UMI_BANK_FIELD_NAME;
    case UMI_BANK_CUSTOMER_SET_STATE: case UMI_BANK_ACCOUNT_SET_STATE:
    case UMI_BANK_BENEFICIARY_SET_STATE: case UMI_BANK_CARD_SET_STATE:
        return UMI_BANK_FIELD_STATE;
    case UMI_BANK_ACCOUNT_OPEN:
        return UMI_BANK_FIELD_OWNER | UMI_BANK_FIELD_NAME | UMI_BANK_FIELD_MONEY;
    case UMI_BANK_BENEFICIARY_CREATE:
        return UMI_BANK_FIELD_OWNER | UMI_BANK_FIELD_DESTINATION | UMI_BANK_FIELD_NAME;
    case UMI_BANK_TEST_CREDIT: case UMI_BANK_HOLD_PLACE: case UMI_BANK_RECONCILE:
        return UMI_BANK_FIELD_SOURCE | UMI_BANK_FIELD_MONEY;
    case UMI_BANK_TRANSFER_SUBMIT:
        return UMI_BANK_FIELD_OWNER | UMI_BANK_FIELD_SOURCE | UMI_BANK_FIELD_DESTINATION | UMI_BANK_FIELD_MONEY;
    case UMI_BANK_CARD_ISSUE:
        return UMI_BANK_FIELD_SOURCE | UMI_BANK_FIELD_NAME | UMI_BANK_FIELD_MONEY;
    case UMI_BANK_CARD_AUTHORISE:
        return UMI_BANK_FIELD_OWNER | UMI_BANK_FIELD_SOURCE | UMI_BANK_FIELD_MONEY;
    case UMI_BANK_CARD_CAPTURE: return UMI_BANK_FIELD_MONEY;
    default: return 0U;
    }
}

UmiStatus BankCommandValid(const UmiBankActor *actor, const UmiBankCommand *command)
{
    if (actor == NULL || command == NULL ||
        command->action < UMI_BANK_CUSTOMER_CREATE || command->action > UMI_BANK_RECONCILE ||
        !BankIdValid(&actor->id, true) || !BankIdValid(&command->requestId, true) ||
        !BankIdValid(&command->id, true) || !BankIdValid(&command->ownerId, false) ||
        !BankIdValid(&command->sourceAccountId, false) ||
        !BankIdValid(&command->destinationAccountId, false) ||
        memchr(command->name, '\0', sizeof command->name) == NULL ||
        command->amount.minor_units < 0 || command->amount.scale > 9U ||
        (int)command->state < 0 || command->state > UMI_BANK_RECORD_CLOSED ||
        command->timestampMillis < 0 || !umi_financial_date_is_valid(command->businessDate))
        return UMI_STATUS_INVALID_ARGUMENT;
    {
        uint32_t fields = UmiBankActionFields(command->action);
        if (((fields & UMI_BANK_FIELD_NAME) == 0U && command->name[0] != '\0') ||
            ((fields & UMI_BANK_FIELD_OWNER) == 0U && command->ownerId.value[0] != '\0') ||
            ((fields & UMI_BANK_FIELD_SOURCE) == 0U && command->sourceAccountId.value[0] != '\0') ||
            ((fields & UMI_BANK_FIELD_DESTINATION) == 0U && command->destinationAccountId.value[0] != '\0') ||
            ((fields & UMI_BANK_FIELD_STATE) == 0U && (int)command->state != 0) ||
            ((fields & UMI_BANK_FIELD_MONEY) == 0U && (command->amount.minor_units != 0 ||
                command->amount.scale != 0U || command->amount.currency.code[0] != '\0')))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (const unsigned char *p = (const unsigned char *)command->name; *p != 0U; ++p)
        if (*p < 32U || *p == 127U) return UMI_STATUS_INVALID_ARGUMENT;
    if (command->amount.currency.code[0] != '\0') {
        UmiCurrency currency;
        if (umi_accounting_currency_from_code(command->amount.currency.code, &currency) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
    } else if (command->amount.minor_units != 0 || command->amount.scale != 0U ||
               command->amount.currency.code[1] != '\0' ||
               command->amount.currency.code[2] != '\0' ||
               command->amount.currency.code[3] != '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((actor->capabilities & ~((uint32_t)UMI_BANK_CAP_ALL)) != 0U ||
        (actor->capabilities & RequiredCapability(command->action)) == 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}

void UmiBankCommandInit(UmiBankCommand *command, UmiBankAction action)
{
    if (command == NULL) return;
    memset(command, 0, sizeof *command);
    command->action = action;
}

const char *UmiBankActionName(UmiBankAction action)
{
    static const char *const names[] = {
        "Unknown", "Create customer", "Change customer state", "Open account",
        "Change account state", "Create beneficiary", "Change beneficiary state",
        "Test credit", "Submit transfer", "Approve transfer", "Reject transfer",
        "Cancel transfer", "Execute transfer", "Reverse transfer", "Place hold",
        "Release hold", "Issue simulated card", "Change card state",
        "Authorise simulated card", "Capture simulated card", "Void card authorisation",
        "Refund simulated card", "Reconcile account"
    };
    return action >= UMI_BANK_CUSTOMER_CREATE && action <= UMI_BANK_RECONCILE ?
        names[(size_t)action] : names[0];
}

/* Finders are bounded over state owned by this module; no caller can alter the
 * counts or stored identities through a public borrowed pointer. */
#define BANK_FINDER(Name, Member, Count, IdField) \
int Name(const BankState *state, const char *id) \
{ \
    if (state == NULL || id == NULL) return -1; \
    for (size_t i = 0U; i < state->counts.Count; ++i) \
        if (strcmp(state->Member[i].IdField.value, id) == 0) return (int)i; \
    return -1; \
}
BANK_FINDER(BankFindCustomer, customers, customers, party.party_id)
BANK_FINDER(BankFindAccount, accounts, accounts, account.account_id)
BANK_FINDER(BankFindBeneficiary, beneficiaries, beneficiaries, id)
BANK_FINDER(BankFindTransfer, transfers, transfers, id)
BANK_FINDER(BankFindCard, cards, cards, id)
BANK_FINDER(BankFindHold, holds, holds, id)
#undef BANK_FINDER

bool BankMoneyMatches(UmiMoney amount, const UmiBankAccount *account)
{
    return account != NULL && amount.scale == account->scale &&
        umi_accounting_currency_equal(amount.currency, account->account.currency);
}

UmiStatus BankAccountActive(const BankState *state, int index)
{
    int customerIndex;
    if (index < 0 || (size_t)index >= state->counts.accounts) return UMI_STATUS_NOT_FOUND;
    if (state->accounts[index].state != UMI_BANK_RECORD_ACTIVE) return UMI_STATUS_INVALID_STATE;
    customerIndex = BankFindCustomer(state, state->accounts[index].account.party_id.value);
    if (customerIndex < 0 || state->customers[customerIndex].state != UMI_BANK_RECORD_ACTIVE)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus BankRequireFunds(const BankState *state, int accountIndex, int64_t amountMinor)
{
    UmiBankBalance balance;
    UmiStatus status;
    if (accountIndex < 0 || (size_t)accountIndex >= state->counts.accounts)
        return UMI_STATUS_NOT_FOUND;
    status = BankProjectBalance(state, state->accounts[accountIndex].account.account_id.value, &balance);
    if (status != UMI_STATUS_OK) return status;
    return amountMinor > 0 && balance.available.minor_units >= amountMinor ?
        UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
