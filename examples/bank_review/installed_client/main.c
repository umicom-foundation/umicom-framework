/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Public installed headers only. Fictional data, actual memory banking service. */
#include <umicom/bank_operations/review.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiBankOperations *bank = NULL;
    UmiBankReview *review = NULL;
    UmiBankActor actor = {0};
    UmiBankCommand command;
    UmiBankCounts counts;
    UmiBankReceipt receipt;
    int result = 1;
    if (UmiBankOperationsOpenMemory(&bank) != UMI_STATUS_OK) goto done;
    if (umi_financial_id_assign(&actor.id, "practice-maker") != UMI_STATUS_OK) goto done;
    actor.capabilities = UMI_BANK_CAP_CUSTOMERS;
    UmiBankCommandInit(&command, UMI_BANK_CUSTOMER_CREATE);
    if (umi_financial_id_assign(&command.id, "workshop") != UMI_STATUS_OK ||
        umi_financial_id_assign(&command.requestId, "create-workshop") != UMI_STATUS_OK) goto done;
    memcpy(command.name, "Workshop", sizeof "Workshop");
    command.businessDate = (UmiFinancialDate){2026, 9U, 28U};
    command.timestampMillis = 1;
    if (UmiBankOperationsReview(bank, &actor, &command, &review) != UMI_STATUS_OK) goto done;
    if (UmiBankOperationsCounts(bank, &counts) != UMI_STATUS_OK || counts.customers != 0U) goto done;
    if (UmiBankOperationsExecuteReviewed(bank, &actor, review, &receipt) != UMI_STATUS_OK ||
        receipt.revision != 1U || receipt.idempotent) goto done;
    if (UmiBankOperationsCounts(bank, &counts) != UMI_STATUS_OK || counts.customers != 1U) goto done;
    puts("Installed Framework client: review left zero customers; applying created one practice customer.");
    result = 0;
done:
    UmiBankReviewDestroy(review);
    UmiBankOperationsDestroy(bank);
    return result;
}
