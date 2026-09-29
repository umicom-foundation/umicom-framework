/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/test_allocations.c
 * PURPOSE: Inject native allocation failures without replacing Data Server or enterprise logic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
void *__real_malloc(size_t);
void *__real_calloc(size_t,size_t);
void *__real_realloc(void *,size_t);
static size_t callCount,failAt;
static bool active;
static bool Fail(void){if(!active)return false;++callCount;return failAt!=0U&&callCount==failAt;}
void *__wrap_malloc(size_t n){return Fail()?NULL:__real_malloc(n);}
void *__wrap_calloc(size_t n,size_t size){return Fail()?NULL:__real_calloc(n,size);}
void *__wrap_realloc(void *p,size_t n){return Fail()?NULL:__real_realloc(p,n);}
int main(void)
{
    size_t failures=0U;
    for(unsigned operation=0U;operation<3U;++operation){size_t limit=1U;
        for(size_t attempt=0U;attempt<=limit;++attempt){
            TestFixture f;UmiEnterpriseDatasetView *view=NULL;UmiEnterpriseRecoveryReview *review=NULL;
            REQUIRE(TestOpen(&f,NULL)==0);REQUIRE(TestStale(&f)==0);uint64_t revision=TestRevision(&f);
            if(operation==2U)OK(UmiEnterpriseRecoveryInspect(f.workspace,ACT(1),"delivery",&review,NULL));
            UmiEnterpriseRowQuery query;UmiEnterpriseRowQueryInit(&query);
            callCount=0U;failAt=attempt;active=true;
            UmiStatus status=operation==0U?UmiEnterpriseDatasetViewCapture(f.workspace,ACT(0),"supplies",&query,&view):
                operation==1U?UmiEnterpriseRecoveryInspect(f.workspace,ACT(1),"delivery",&review,NULL):
                UmiEnterpriseRecoveryPrepare(f.workspace,ACT(1),review,"after-failure",NULL);
            active=false;
            if(attempt==0U){REQUIRE(status==UMI_STATUS_OK);limit=callCount;REQUIRE(limit>0U&&limit<2048U);}
            else {REQUIRE(status!=UMI_STATUS_OK);++failures;REQUIRE(TestRevision(&f)==revision);
                if(operation==0U)REQUIRE(view==NULL);
                if(operation==1U)REQUIRE(review==NULL);
                if(operation==2U){UmiEnterpriseJob job;STATUS(UmiEnterpriseWorkspaceJobFind(f.workspace,"after-failure",&job),UMI_STATUS_NOT_FOUND);}
            }
            UmiEnterpriseDatasetViewDestroy(view);UmiEnterpriseRecoveryReviewDestroy(review);TestClose(&f);
        }
    }
    printf("Rejected allocation-failure points: %zu\n",failures);return 0;
}
