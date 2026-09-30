/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/bank_review/lesson.c
 * PURPOSE:
 *   Complete lesson: inspect the reservation, approval and local posting separately. All
 *   setup actors are trusted fictional test identities, not authenticated users.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Complete lesson: inspect the reservation, approval and local posting separately.
 * All setup actors are trusted fictional test identities, not authenticated users.
 *---------------------------------------------------------------------------*/
#include "lesson.h"
#include "umicom/bank_operations/review.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus Command(UmiBankOperations *bank, UmiBankCommand *command,
    UmiBankAction action, const char *id)
{
    UmiBankCounts counts;
    UmiStatus status = UmiBankOperationsCounts(bank, &counts);
    if (status != UMI_STATUS_OK) return status;
    UmiBankCommandInit(command, action);
    command->expectedRevision = counts.revision;
    command->businessDate = (UmiFinancialDate){2026, 9U, 28U};
    command->timestampMillis = (int64_t)counts.revision;
    (void)snprintf(command->requestId.value, sizeof command->requestId.value,
        "lesson-request-%" PRIu64, counts.revision + 1U);
    return umi_financial_id_assign(&command->id, id);
}
static void Money(UmiMoney *out, int64_t minor)
{
    memset(out, 0, sizeof *out);
    memcpy(out->currency.code, "GBP", 4U); out->scale = 2U; out->minor_units = minor;
}
#define TRY(call) do { status = (call); if (status != UMI_STATUS_OK) goto done; } while (0)
int UmiBankReviewLesson(bool verbose)
{
    UmiBankOperations *bank = NULL;
    UmiBankReview *review = NULL;
    UmiBankReviewSnapshot *snapshot = NULL;
    UmiBankCommand command;
    UmiBankReceipt receipt;
    UmiBankActor maker = {0}, checker = {0};
    UmiBankBalance balance;
    char *text = NULL;
    UmiStatus status;
    maker.capabilities = checker.capabilities = UMI_BANK_CAP_ALL;
    TRY(umi_financial_id_assign(&maker.id, "workshop-maker"));
    TRY(umi_financial_id_assign(&checker.id, "workshop-checker"));
    TRY(UmiBankOperationsOpenMemory(&bank));
    for (unsigned i = 0U; i < 2U; ++i) {
        TRY(Command(bank, &command, UMI_BANK_CUSTOMER_CREATE, i == 0U ? "workshop" : "supplier"));
        strcpy(command.name, i == 0U ? "Workshop" : "Supplier");
        TRY(UmiBankOperationsExecute(bank, &maker, &command, &receipt));
        TRY(Command(bank, &command, UMI_BANK_ACCOUNT_OPEN, i == 0U ? "workshop-gbp" : "supplier-gbp"));
        strcpy(command.ownerId.value, i == 0U ? "workshop" : "supplier");
        strcpy(command.name, "GBP current account"); Money(&command.amount, 0);
        TRY(UmiBankOperationsExecute(bank, &maker, &command, &receipt));
    }
    TRY(Command(bank, &command, UMI_BANK_BENEFICIARY_CREATE, "supplier-beneficiary"));
    strcpy(command.ownerId.value, "workshop"); strcpy(command.destinationAccountId.value, "supplier-gbp");
    strcpy(command.name, "Workshop supplier");
    TRY(UmiBankOperationsExecute(bank, &maker, &command, &receipt));
    TRY(Command(bank, &command, UMI_BANK_TEST_CREDIT, "practice-funding"));
    strcpy(command.sourceAccountId.value, "workshop-gbp"); Money(&command.amount, 100000);
    TRY(UmiBankOperationsExecute(bank, &maker, &command, &receipt));

    TRY(Command(bank, &command, UMI_BANK_TRANSFER_SUBMIT, "invoice-payment"));
    strcpy(command.sourceAccountId.value, "workshop-gbp"); strcpy(command.ownerId.value, "supplier-beneficiary");
    Money(&command.amount, 25000);
    TRY(UmiBankOperationsReview(bank, &maker, &command, &review));
    snapshot = malloc(sizeof *snapshot);
    if (snapshot == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    TRY(UmiBankReviewSnapshotRead(review, snapshot));
    TRY(UmiBankOperationsBalance(bank, "workshop-gbp", &balance));
    if (balance.available.minor_units != 100000 || snapshot->accountCount != 1U ||
        snapshot->accounts[0].after.available.minor_units != 75000) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    puts("Before applying: GBP 1000.00 is still available.");
    puts("Review predicts: GBP 250.00 reserved; GBP 750.00 available.");
    if (verbose) {
        text = malloc(UMI_BANK_REVIEW_TEXT_CAPACITY);
        if (text == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
        TRY(UmiBankReviewDescribe(review, text, UMI_BANK_REVIEW_TEXT_CAPACITY, NULL));
        puts(text);
    }
    TRY(UmiBankOperationsExecuteReviewed(bank, &maker, review, &receipt));
    UmiBankReviewDestroy(review); review = NULL;
    TRY(Command(bank, &command, UMI_BANK_TRANSFER_APPROVE, "invoice-payment"));
    TRY(UmiBankOperationsReview(bank, &checker, &command, &review));
    TRY(UmiBankReviewSnapshotRead(review, snapshot));
    if (snapshot->hasJournal || snapshot->accountCount != 0U) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    TRY(UmiBankOperationsExecuteReviewed(bank, &checker, review, &receipt));
    puts("Approval changes the transfer state, not the booked balances.");
    UmiBankReviewDestroy(review); review = NULL;
    TRY(Command(bank, &command, UMI_BANK_TRANSFER_EXECUTE, "invoice-payment"));
    TRY(UmiBankOperationsReview(bank, &maker, &command, &review));
    TRY(UmiBankOperationsExecuteReviewed(bank, &maker, review, &receipt));
    TRY(UmiBankOperationsBalance(bank, "supplier-gbp", &balance));
    if (balance.booked.minor_units != 25000) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    puts("Local posting: Workshop GBP 750.00; Supplier GBP 250.00.");
    puts("Practice complete. Memory only; no bank network or real payment was used.");
done:
    if (status != UMI_STATUS_OK) fprintf(stderr, "Lesson failed: status %d\n", (int)status);
    free(text); free(snapshot); UmiBankReviewDestroy(review); UmiBankOperationsDestroy(bank);
    return status == UMI_STATUS_OK ? 0 : 1;
}
#undef TRY
