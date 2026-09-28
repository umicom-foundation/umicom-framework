/* Umicom Foundation | Sammy Hegab | MIT
 * This client consumes installed public headers only. No private model access. */
#include "umicom/finance_operations/close_review.h"
#include <stdio.h>
int main(void)
{
    UmiDataServer *s=NULL;UmiFinanceOperations *o=NULL;UmiFinanceCloseReview *r=NULL;
    UmiStatus status=umi_data_server_create_memory(&s);
    if(status==UMI_STATUS_OK)status=UmiFinanceOperationsCreate(s,&o);
    UmiFinanceOperationCommand c;UmiFinanceOperationCommandInit(&c);c.kind=UMI_FINANCE_OPEN_PERIOD;
    if(status==UMI_STATUS_OK)status=umi_financial_id_assign(&c.id,"september");
    if(status==UMI_STATUS_OK)status=umi_financial_id_assign(&c.actorId,"operator");
    if(status==UMI_STATUS_OK)status=umi_financial_id_assign(&c.requestId,"one");
    c.date=(UmiFinancialDate){2026,9U,1U};c.endDate=(UmiFinancialDate){2026,9U,30U};
    UmiFinanceOperationReceipt receipt;
    if(status==UMI_STATUS_OK)status=UmiFinanceOperationsApply(o,&c,&receipt);
    if(status==UMI_STATUS_OK)status=UmiFinanceCloseReviewCreate(o,"september",&r);
    UmiFinanceOperationsDestroy(o);umi_data_server_destroy(s);
    UmiFinanceCloseReviewInfo info;
    if(status==UMI_STATUS_OK)status=UmiFinanceCloseReviewGetInfo(r,&info);
    if(status==UMI_STATUS_OK&&(!info.canPrepare||info.issueCount!=0U))status=UMI_STATUS_INTERNAL_ERROR;
    UmiFinanceCloseReviewDestroy(r);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Consumer failed: %d\n",(int)status);return 1;}
    puts("Installed public SDK: owned review remains readable after the source service is closed.");return 0;
}
