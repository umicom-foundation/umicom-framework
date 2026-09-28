/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/finance_operations/close_review.h"
#include <stdio.h>
#include <stdlib.h>
static int fail;
void *__real_calloc(size_t,size_t);
void *__wrap_calloc(size_t n,size_t bytes) {return fail?NULL:__real_calloc(n,bytes);}
int main(void)
{
    UmiDataServer *s=NULL;UmiFinanceOperations *o=NULL;UmiFinanceCloseReview *r=NULL;
    if(umi_data_server_create_memory(&s)!=UMI_STATUS_OK||UmiFinanceOperationsCreate(s,&o)!=UMI_STATUS_OK)return 1;
    UmiFinanceOperationCommand c;UmiFinanceOperationCommandInit(&c);c.kind=UMI_FINANCE_OPEN_PERIOD;
    if(umi_financial_id_assign(&c.id,"september")!=UMI_STATUS_OK ||
       umi_financial_id_assign(&c.actorId,"operator")!=UMI_STATUS_OK ||
       umi_financial_id_assign(&c.requestId,"one")!=UMI_STATUS_OK)return 1;
    c.date=(UmiFinancialDate){2026,9U,1U};c.endDate=(UmiFinancialDate){2026,9U,30U};
    UmiFinanceOperationReceipt receipt;if(UmiFinanceOperationsApply(o,&c,&receipt)!=UMI_STATUS_OK)return 1;
    fail=1;UmiStatus status=UmiFinanceCloseReviewCreate(o,"september",&r);fail=0;
    if(status!=UMI_STATUS_OUT_OF_MEMORY||r)return 1;
    if(UmiFinanceCloseReviewCreate(o,"september",&r)!=UMI_STATUS_OK)return 1;
    UmiFinanceCloseReviewDestroy(r);UmiFinanceOperationsDestroy(o);umi_data_server_destroy(s);return 0;
}
