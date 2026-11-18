/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/performance_budget/test_budget_admission.c
 *
 * PURPOSE:
 *   Exercise the three public budget owners against finite-value, exhausted-sequence and bounded-identity contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/observability/performance/memory_budget.h"
#include "umicom/observability/performance/resource_budget.h"
#include "umicom/observability/performance/performance_budget.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#if UMI_BUDGET_KIND == 0
#define Record UmiPerformanceMemoryBudget
#define Prefix umi_performance_memory_budget
#elif UMI_BUDGET_KIND == 1
#define Record UmiPerformanceResourceBudget
#define Prefix umi_performance_resource_budget
#elif UMI_BUDGET_KIND == 2
#define Record UmiPerformancePerformanceBudget
#define Prefix umi_performance_performance_budget
#else
#error Select an existing public budget owner
#endif
#define JOIN_INNER(a,b) a##b
#define JOIN(a,b) JOIN_INNER(a,b)
#define API(suffix) JOIN(Prefix,suffix)
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    Record value,other,before;
    CHECK(API(_init)(&value,"budget","framework")==UMI_STATUS_OK);
    CHECK(API(_init)(&other,"budget","framework")==UMI_STATUS_OK);
    if(!strcmp(mode,"sequence")) {
        value.sequence=UINT64_MAX-1U;
        CHECK(API(_observe)(&value,12,3,4U,100U)==UMI_STATUS_OK);
        CHECK(value.sequence==UINT64_MAX);
        memcpy(&before,&value,sizeof value);
        CHECK(API(_observe)(&value,99,99,99U,999U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(&value,&before,sizeof value));
    } else if(!strcmp(mode,"nonfinite")) {
        const double bad[3]={NAN,INFINITY,-INFINITY};
        for(size_t i=0U;i<3U;++i) {
            memcpy(&before,&value,sizeof value);
            CHECK(API(_observe)(&value,bad[i],1,2U,3U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(&value,&before,sizeof value));
            CHECK(API(_observe)(&value,1,bad[i],2U,3U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(&value,&before,sizeof value));
        }
    } else if(!strcmp(mode,"finite-budget")) {
        CHECK(API(_within_budget)(0,0));
        CHECK(API(_within_budget)(5,10));
        CHECK(API(_within_budget)(DBL_MAX,DBL_MAX));
        CHECK(!API(_within_budget)(11,10));
        CHECK(!API(_within_budget)(-1,10));
        const double bad[3]={NAN,INFINITY,-INFINITY};
        for(size_t i=0U;i<3U;++i) {
            CHECK(!API(_within_budget)(bad[i],10));
            CHECK(!API(_within_budget)(1,bad[i]));
            CHECK(!API(_within_budget)(bad[i],bad[i]));
        }
    } else if(!strcmp(mode,"identity")) {
        CHECK(API(_same_identity)(&value,&other));
        other.sequence=7U;other.value=19;other.state=UMI_PERFORMANCE_STATE_ACTIVE;
        CHECK(API(_same_identity)(&value,&other));
        memset(value.id,'x',sizeof value.id);
        memset(other.id,'x',sizeof other.id);
        CHECK(!API(_same_identity)(&value,&other));
        CHECK(API(_init)(&value,"budget","framework")==UMI_STATUS_OK);
        CHECK(API(_init)(&other,"budget","framework")==UMI_STATUS_OK);
        memset(value.subject_id,'x',sizeof value.subject_id);
        memset(other.subject_id,'x',sizeof other.subject_id);
        CHECK(!API(_same_identity)(&value,&other));
        CHECK(!API(_same_identity)(NULL,&other));
        CHECK(!API(_same_identity)(&value,NULL));
    } else if(!strcmp(mode,"validation")) {
        value.value=NAN;
        CHECK(API(_validate)(&value)==UMI_STATUS_INVALID_ARGUMENT);
        value.value=0;value.auxiliary=INFINITY;
        CHECK(API(_validate)(&value)==UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before,&value,sizeof value);
        CHECK(API(_observe)(&value,1,1,1U,1U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&value,&before,sizeof value));
    } else if(!strcmp(mode,"finite-observation")) {
        value.state=UMI_PERFORMANCE_STATE_ACTIVE;value.severity=UMI_PERFORMANCE_SEVERITY_WARNING;
        CHECK(API(_observe)(&value,-DBL_MAX,DBL_MAX,UINT64_MAX,UINT64_MAX)==UMI_STATUS_OK);
        CHECK(value.value==-DBL_MAX&&value.auxiliary==DBL_MAX&&value.count==UINT64_MAX);
        CHECK(value.sequence==1U&&value.timestamp_ns==UINT64_MAX);
        CHECK(value.state==UMI_PERFORMANCE_STATE_ACTIVE&&value.severity==UMI_PERFORMANCE_SEVERITY_WARNING);
        /* Timestamps are observations, not a newly introduced monotonic rule. */
        CHECK(API(_observe)(&value,0,0,0U,0U)==UMI_STATUS_OK);
        CHECK(value.sequence==2U&&value.timestamp_ns==0U);
    } else if(!strcmp(mode,"invalid-record")) {
        value.api_version=0U;memcpy(&before,&value,sizeof value);
        CHECK(API(_observe)(&value,1,1,1U,1U)==UMI_STATUS_INVALID_STATE);
        CHECK(!memcmp(&value,&before,sizeof value));
        CHECK(API(_observe)(NULL,1,1,1U,1U)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
