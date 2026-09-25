/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_risk_book.c
 *
 * PURPOSE:
 *   Verify treasury risk limit utilisation and breach detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include "umicom/open_tms/risk_book.h"

int main(void)
{
    UmiOpenTmsRiskBook book; UmiOpenTmsRiskLimit limit={0};
    umi_open_tms_risk_book_init(&book);
    (void)snprintf(limit.id,sizeof(limit.id),"%s","market");
    (void)snprintf(limit.scopeId,sizeof(limit.scopeId),"%s","portfolio-1");
    limit.limit=100.0;limit.utilisation=80.0;limit.version=1U;
    assert(umi_open_tms_risk_book_upsert(&book,&limit)==UMI_STATUS_OK);
    assert(umi_open_tms_risk_limit_utilisation_percent(&limit)==80.0);
    assert(umi_open_tms_risk_book_breach_count(&book)==0U);
    limit.utilisation=101.0;limit.version=2U;
    assert(umi_open_tms_risk_book_upsert(&book,&limit)==UMI_STATUS_OK);
    assert(umi_open_tms_risk_book_breach_count(&book)==1U);
    return 0;
}
