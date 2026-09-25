/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/runtime.c
 *
 * PURPOSE:
 *   Coordinate failure-atomic banking commands, public queries and service ownership.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>

/* Replay and live requests share exactly one domain transition. The candidate
 * belongs to the caller; it is discarded on any error, including a late
 * invariant failure. No mutation is published before the repository commits. */
UmiStatus BankApply(BankState *state, const UmiBankActor *actor,
                   const UmiBankCommand *command)
{
    UmiStatus status = BankCommandValid(actor, command);
    UmiBankAuditEvent *event;
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) return status;
    if (state->counts.events >= UMI_BANK_EVENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (command->expectedRevision != state->counts.revision) return UMI_STATUS_BUSY;
    for (size_t i = 0U; i < state->counts.events; ++i)
        if (strcmp(state->events[i].command.requestId.value, command->requestId.value) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    switch (command->action) {
    case UMI_BANK_CUSTOMER_CREATE: case UMI_BANK_CUSTOMER_SET_STATE:
    case UMI_BANK_ACCOUNT_OPEN: case UMI_BANK_ACCOUNT_SET_STATE:
    case UMI_BANK_BENEFICIARY_CREATE: case UMI_BANK_BENEFICIARY_SET_STATE:
    case UMI_BANK_TEST_CREDIT: case UMI_BANK_RECONCILE:
        status = BankApplyCustomerAccount(state, actor, command); break;
    case UMI_BANK_TRANSFER_SUBMIT: case UMI_BANK_TRANSFER_APPROVE:
    case UMI_BANK_TRANSFER_REJECT: case UMI_BANK_TRANSFER_CANCEL:
    case UMI_BANK_TRANSFER_EXECUTE: case UMI_BANK_TRANSFER_REVERSE:
        status = BankApplyTransfer(state, actor, command); break;
    default:
        status = BankApplyCardHold(state, actor, command); break;
    }
    if (status != UMI_STATUS_OK) return status;
    /* Every accepted transition must leave all booked and available balances
     * representable and nonnegative, not merely the account shown in the UI. */
    for (size_t i = 0U; i < state->counts.accounts; ++i) {
        UmiBankBalance balance;
        status = BankProjectBalance(state, state->accounts[i].account.account_id.value, &balance);
        if (status != UMI_STATUS_OK) return status;
    }
    event = &state->events[state->counts.events];
    memset(event, 0, sizeof *event);
    event->revision = state->counts.revision + 1U;
    event->actor = *actor;
    event->command = *command;
    state->counts.events++;
    state->counts.revision++;
    return UMI_STATUS_OK;
}

/* The native launcher chooses the location. Reject relative and special URI
 * names so a shortcut's working directory cannot choose a different ledger. */
static bool AbsolutePath(const char *path)
{
    size_t length;
    if (path == NULL) return false;
    length = strlen(path);
    if (length == 0U || length >= 1024U) return false;
#ifdef _WIN32
    if (length >= 3U && ((path[0] >= 'A' && path[0] <= 'Z') ||
        (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':' &&
        (path[2] == '\\' || path[2] == '/')) return true;
    return length > 4U && path[0] == '\\' && path[1] == '\\' &&
        path[2] != '?' && path[2] != '.';
#else
    return path[0] == '/';
#endif
}

static UmiStatus FinishOpen(UmiDataServer *server, UmiBankOperations **out)
{
    UmiBankOperations *operations = calloc(1U, sizeof *operations);
    UmiStatus status;
    if (operations == NULL) { umi_data_server_destroy(server); return UMI_STATUS_OUT_OF_MEMORY; }
    operations->server = server;
    status = BankRepositoryLoad(operations, &operations->state);
    if (status != UMI_STATUS_OK) { UmiBankOperationsDestroy(operations); return status; }
    *out = operations;
    return UMI_STATUS_OK;
}

UmiStatus UmiBankOperationsOpenMemory(UmiBankOperations **outOperations)
{
    UmiDataServer *server = NULL;
    UmiStatus status;
    if (outOperations == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outOperations = NULL;
    status = umi_data_server_create_memory(&server);
    return status == UMI_STATUS_OK ? FinishOpen(server, outOperations) : status;
}

UmiStatus UmiBankOperationsOpenSqlite(const char *absolutePath, UmiBankOperations **outOperations)
{
    UmiDataServer *server = NULL;
    UmiStatus status;
    if (outOperations == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outOperations = NULL;
    if (!AbsolutePath(absolutePath)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_server_create_sqlite(absolutePath, &server);
    return status == UMI_STATUS_OK ? FinishOpen(server, outOperations) : status;
}

void UmiBankOperationsDestroy(UmiBankOperations *operations)
{
    if (operations == NULL) return;
    umi_data_server_destroy(operations->server);
    free(operations->state);
    free(operations);
}

UmiStatus UmiBankOperationsReload(UmiBankOperations *operations)
{
    BankState *loaded = NULL;
    UmiStatus status;
    if (operations == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned) return UMI_STATUS_INVALID_STATE;
    status = BankRepositoryLoad(operations, &loaded);
    if (status == UMI_STATUS_OK) {
        free(operations->state);
        operations->state = loaded;
    }
    return status;
}

UmiStatus UmiBankOperationsExecute(UmiBankOperations *operations,
    const UmiBankActor *actor, const UmiBankCommand *command, UmiBankReceipt *outReceipt)
{
    BankState *candidate;
    UmiStatus status;
    if (outReceipt != NULL) memset(outReceipt, 0, sizeof *outReceipt);
    if (operations == NULL || outReceipt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    status = BankCommandValid(actor, command);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < operations->state->counts.events; ++i) {
        const UmiBankAuditEvent *event = &operations->state->events[i];
        if (strcmp(event->command.requestId.value, command->requestId.value) == 0) {
            if (!BankSameRequest(event, actor, command)) return UMI_STATUS_ALREADY_EXISTS;
            outReceipt->revision = event->revision;
            outReceipt->requestId = command->requestId;
            outReceipt->idempotent = true;
            return UMI_STATUS_OK;
        }
    }
    if (command->expectedRevision != operations->state->counts.revision) return UMI_STATUS_BUSY;
    candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(candidate, operations->state, sizeof *candidate);
    status = BankApply(candidate, actor, command);
    if (status == UMI_STATUS_OK)
        status = BankRepositoryCommit(operations,
            &candidate->events[candidate->counts.events - 1U], operations->state->counts.revision);
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    free(operations->state);
    operations->state = candidate;
    outReceipt->revision = candidate->counts.revision;
    outReceipt->requestId = command->requestId;
    return UMI_STATUS_OK;
}

UmiStatus UmiBankOperationsCounts(const UmiBankOperations *operations, UmiBankCounts *outCounts)
{
    if (outCounts != NULL) memset(outCounts, 0, sizeof *outCounts);
    if (operations == NULL || outCounts == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    *outCounts = operations->state->counts;
    return UMI_STATUS_OK;
}

UmiStatus UmiBankOperationsBalance(const UmiBankOperations *operations,
    const char *accountId, UmiBankBalance *outBalance)
{
    if (outBalance != NULL) memset(outBalance, 0, sizeof *outBalance);
    if (operations == NULL || accountId == NULL || outBalance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    return BankProjectBalance(operations->state, accountId, outBalance);
}

/* Copies rather than borrowed mutable records preserve the Framework authority
 * boundary while allowing any frontend to build a toolkit-neutral projection. */
#define BANK_COPY_QUERY(Name, Type, Member, Count) \
UmiStatus Name(const UmiBankOperations *operations, size_t index, Type *out) \
{ \
    if (out != NULL) memset(out, 0, sizeof *out); \
    if (operations == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE; \
    if (index >= operations->state->counts.Count) return UMI_STATUS_NOT_FOUND; \
    *out = operations->state->Member[index]; \
    return UMI_STATUS_OK; \
}
BANK_COPY_QUERY(UmiBankOperationsCustomerAt, UmiBankCustomer, customers, customers)
BANK_COPY_QUERY(UmiBankOperationsAccountAt, UmiBankAccount, accounts, accounts)
BANK_COPY_QUERY(UmiBankOperationsBeneficiaryAt, UmiBankBeneficiary, beneficiaries, beneficiaries)
BANK_COPY_QUERY(UmiBankOperationsTransferAt, UmiBankTransfer, transfers, transfers)
BANK_COPY_QUERY(UmiBankOperationsCardAt, UmiBankCard, cards, cards)
BANK_COPY_QUERY(UmiBankOperationsHoldAt, UmiBankHold, holds, holds)
BANK_COPY_QUERY(UmiBankOperationsJournalAt, UmiBankJournal, journals, journals)
BANK_COPY_QUERY(UmiBankOperationsAuditAt, UmiBankAuditEvent, events, events)
BANK_COPY_QUERY(UmiBankOperationsReconciliationAt, UmiBankReconciliation, reconciliations, reconciliations)
#undef BANK_COPY_QUERY
