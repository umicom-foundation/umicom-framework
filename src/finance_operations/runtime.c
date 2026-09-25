/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/runtime.c
 *
 * PURPOSE:
 *   Coordinate domain commands, commit candidate states and expose copied records without application-local financial logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdlib.h>
#include <string.h>

static bool Usable(const UmiFinanceOperations *operations)
{
    return operations != NULL && operations->state != NULL && !operations->poisoned;
}

UmiStatus FinanceExecute(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    UmiStatus status = FinanceCommandValidate(command);
    if (status != UMI_STATUS_OK) return status;
    if (command->expectedRevision != state->counts.revision) return UMI_STATUS_BUSY;
    if (state->counts.events >= UMI_FINANCE_OPERATIONS_EVENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < state->counts.events; ++index)
        if (FinanceIdEqual(state->commands[index].requestId, command->requestId)) return UMI_STATUS_ALREADY_EXISTS;
    switch (command->kind) {
    case UMI_FINANCE_REGISTER_PARTICIPANT: case UMI_FINANCE_LIST_INSTRUMENT:
    case UMI_FINANCE_SET_MARKET_STATE: case UMI_FINANCE_DEPOSIT_LOTS:
    case UMI_FINANCE_PLACE_ORDER: case UMI_FINANCE_CANCEL_ORDER:
        status = FinanceApplyExchange(state, command); break;
    case UMI_FINANCE_CLEAR_FILL: case UMI_FINANCE_SETTLE_FILL:
        status = FinanceApplySettlement(state, command); break;
    default: status = FinanceApplyAccounting(state, command); break;
    }
    if (status == UMI_STATUS_OK) status = FinanceAllReservationsValid(state);
    if (status != UMI_STATUS_OK) return status;
    state->commands[state->counts.events++] = *command;
    ++state->counts.revision;
    return UMI_STATUS_OK;
}

UmiStatus UmiFinanceOperationsCreate(UmiDataServer *server, UmiFinanceOperations **out)
{
    UmiFinanceOperations *operations;
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    operations = calloc(1U, sizeof(*operations));
    if (operations == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    operations->server = server;
    status = FinanceRepositoryLoad(operations, &operations->state);
    if (status != UMI_STATUS_OK) { free(operations); return status; }
    *out = operations;
    return UMI_STATUS_OK;
}

void UmiFinanceOperationsDestroy(UmiFinanceOperations *operations)
{
    if (operations != NULL) { free(operations->state); free(operations); }
}

UmiStatus UmiFinanceOperationsReload(UmiFinanceOperations *operations)
{
    FinanceState *candidate = NULL;
    UmiStatus status;
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    status = FinanceRepositoryLoad(operations, &candidate);
    if (status != UMI_STATUS_OK) { operations->writesBlocked = true; return status; }
    operations->writesBlocked = false;
    free(operations->state);
    operations->state = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiFinanceOperationsApply(UmiFinanceOperations *operations,
    const UmiFinanceOperationCommand *command, UmiFinanceOperationReceipt *out)
{
    FinanceState *candidate;
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    if (operations->writesBlocked) return UMI_STATUS_INVALID_STATE;
    status = FinanceCommandValidate(command);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < operations->state->counts.events; ++index) {
        const UmiFinanceOperationCommand *previous = &operations->state->commands[index];
        if (!FinanceIdEqual(previous->requestId, command->requestId)) continue;
        if (!FinanceSameCommand(previous, command)) return UMI_STATUS_ALREADY_EXISTS;
        out->revision = (uint64_t)index + 1U;
        out->requestId = previous->requestId;
        out->entityId = previous->id;
        out->duplicate = true;
        return UMI_STATUS_OK;
    }
    if (command->expectedRevision != operations->state->counts.revision) return UMI_STATUS_BUSY;
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *operations->state;
    status = FinanceExecute(candidate, command);
    if (status == UMI_STATUS_OK) status = FinanceRepositoryCommit(operations, command);
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    free(operations->state);
    operations->state = candidate;
    out->revision = candidate->counts.revision;
    out->requestId = command->requestId;
    out->entityId = command->id;
    return UMI_STATUS_OK;
}

UmiStatus UmiFinanceOperationsCounts(const UmiFinanceOperations *operations, UmiFinanceOperationCounts *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    *out = operations->state->counts;
    return UMI_STATUS_OK;
}

/* All views get copies. A widget cannot mutate a ledger record or keep a
 * dangling pointer across the next successfully committed command. */
#define FINANCE_QUERY(Name, Type, field, count) \
UmiStatus UmiFinanceOperations##Name##At(const UmiFinanceOperations *operations, size_t index, Type *out) \
{ \
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE; \
    if (index >= operations->state->counts.count) return UMI_STATUS_NOT_FOUND; \
    *out = operations->state->field[index]; \
    return UMI_STATUS_OK; \
}
FINANCE_QUERY(Account, UmiFinanceOperationAccount, accounts, accounts)
FINANCE_QUERY(Period, UmiFinanceOperationPeriod, periods, periods)
FINANCE_QUERY(Journal, UmiFinanceOperationJournal, journals, journals)
FINANCE_QUERY(Participant, UmiFinanceOperationParticipant, participants, participants)
FINANCE_QUERY(Instrument, UmiFinanceOperationInstrument, instruments, instruments)
FINANCE_QUERY(Holding, UmiFinanceOperationHolding, holdings, holdings)
FINANCE_QUERY(Order, UmiFinanceOperationOrder, orders, orders)
FINANCE_QUERY(Fill, UmiFinanceOperationFill, fills, fills)
FINANCE_QUERY(Reconciliation, UmiFinanceOperationReconciliation, reconciliations, reconciliations)
#undef FINANCE_QUERY

UmiStatus UmiFinanceOperationsAuditAt(const UmiFinanceOperations *operations, size_t index, UmiFinanceOperationAudit *out)
{
    const UmiFinanceOperationCommand *command;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    if (index >= operations->state->counts.events) return UMI_STATUS_NOT_FOUND;
    command = &operations->state->commands[index];
    out->requestId = command->requestId;
    out->actorId = command->actorId;
    out->entityId = command->id;
    out->kind = command->kind;
    out->revision = (uint64_t)index + 1U;
    return UMI_STATUS_OK;
}

UmiStatus UmiFinanceOperationsAccountBalance(const UmiFinanceOperations *operations, const char *accountId, int64_t *out)
{
    UmiFinancialId id;
    UmiStatus status = FinanceSetId(&id, accountId);
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    return status != UMI_STATUS_OK ? status : FinanceAccountBalance(operations->state, id, out);
}
UmiStatus UmiFinanceOperationsCash(const UmiFinanceOperations *operations, const char *participantId, UmiFinanceOperationAvailability *out)
{
    UmiFinancialId id;
    UmiStatus status = FinanceSetId(&id, participantId);
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    return status != UMI_STATUS_OK ? status : FinanceCash(operations->state, id, out);
}
UmiStatus UmiFinanceOperationsLots(const UmiFinanceOperations *operations, const char *participantId,
    const char *instrumentId, UmiFinanceOperationAvailability *out)
{
    UmiFinancialId participant, instrument;
    UmiStatus status = FinanceSetId(&participant, participantId);
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = FinanceSetId(&instrument, instrumentId);
    return status != UMI_STATUS_OK ? status : FinanceLots(operations->state, participant, instrument, out);
}
UmiStatus UmiFinanceOperationsTrialBalance(const UmiFinanceOperations *operations, const char *periodId,
    UmiCurrency currency, uint8_t scale, UmiFinanceTrialBalance *out)
{
    UmiFinancialId id;
    UmiStatus status = FinanceSetId(&id, periodId);
    if (!Usable(operations)) return UMI_STATUS_INVALID_STATE;
    return status != UMI_STATUS_OK ? status : FinanceTrialBalance(operations->state, id, currency, scale, out);
}
