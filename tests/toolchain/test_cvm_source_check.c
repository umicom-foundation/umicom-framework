/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_source_check.c
 *
 * PURPOSE:
 *   Verify revision freshness, stale-result reset, storage admission and compiler-only warning behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_source_check.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
static const char good[] = "int main(void){int unused=7;return 42;}";
static int accepted(UmiCvmSourceCheckResult *result, uint64_t revision)
{
    CHECK(umi_cvm_check_source(good,strlen(good),revision,result,sizeof *result)==UMI_STATUS_OK);
    CHECK(result->completed && result->source_revision==revision);
    CHECK(result->diagnostic.status==UMC_OK && result->report.warning_count==1U);
    CHECK(result->report.instruction_count==4U);
    CHECK(umi_cvm_source_check_matches(result,sizeof *result,revision));
    return 0;
}
static int success(void)
{
    UmiCvmSourceCheckResult result;
    CHECK(!accepted(&result,41U));
    CHECK(!umi_cvm_source_check_matches(&result,sizeof result,42U));
    CHECK(result.report.warnings[0].source_length==6U);
    CHECK(!accepted(&result,0U));
    CHECK(!accepted(&result,UINT64_MAX));
    return 0;
}
static int reset(void)
{
    UmiCvmSourceCheckResult result;
    const char *bad[]={"int main(void){return missing;}","int main(void){return 2147483648;}"};
    for(size_t i=0;i<2U;++i){
        CHECK(!accepted(&result,1U));
        CHECK(umi_cvm_check_source(bad[i],strlen(bad[i]),2U,&result,sizeof result)==UMI_STATUS_PARSE_ERROR);
        CHECK(!result.completed && result.source_revision==2U);
        CHECK(result.report.warning_count==0U && result.report.instruction_count==0U);
        CHECK(result.report.warnings[0].source_length==0U);
        CHECK(!umi_cvm_source_check_matches(&result,sizeof result,1U));
        CHECK(!umi_cvm_source_check_matches(&result,sizeof result,2U));
        CHECK(result.diagnostic.status!=UMC_OK && result.diagnostic.line==1U);
    }
    CHECK(!accepted(&result,3U));
    return 0;
}
static int arguments(void)
{
    UmiCvmSourceCheckResult result;
    CHECK(!accepted(&result,9U));
    CHECK(umi_cvm_check_source(NULL,0U,10U,&result,sizeof result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!result.completed && result.source_revision==10U && result.report.warning_count==0U);
    CHECK(result.diagnostic.status==UMC_INVALID_ARGUMENT);
    CHECK(umi_cvm_check_source(good,SIZE_MAX,11U,&result,sizeof result)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!result.completed && result.source_revision==11U && result.report.instruction_count==0U);
    CHECK(result.diagnostic.status==UMC_SOURCE_TOO_LARGE);
    CHECK(umi_cvm_check_source(good,strlen(good),12U,NULL,sizeof result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_cvm_source_check_matches(NULL,sizeof result,12U));
    return 0;
}
static int storage(void)
{
    UmiCvmSourceCheckResult result,before;
    CHECK(!accepted(&result,1U));
    memcpy(&before,&result,sizeof result);
    const size_t sizes[]={0U,sizeof result-1U,sizeof result+1U,SIZE_MAX};
    for(size_t i=0;i<sizeof sizes/sizeof sizes[0];++i){
        CHECK(umi_cvm_check_source(NULL,SIZE_MAX,2U,&result,sizes[i])==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before,&result,sizeof result)==0);
        CHECK(!umi_cvm_source_check_matches(&result,sizes[i],1U));
    }
    return 0;
}
static int nonexecution(void)
{
    const char source[]="int main(void){int unused=1/0;return 42;}";
    UmiCvmSourceCheckResult result;
    CHECK(umi_cvm_check_source(source,strlen(source),1U,&result,sizeof result)==UMI_STATUS_OK);
    CHECK(result.completed && result.report.warning_count==1U && result.report.instruction_count==6U);
    return 0;
}
static int parity(void)
{
    const char *sources[]={good,"int main(void){int used=42;return used;}",
        "int main(void){int answer=0;{int x=1;}{int x=42;answer=x;}return answer;}",
        "int main(void){int x=1;if(0){x++;}return 42;}"};
    for(size_t i=0;i<sizeof sources/sizeof sources[0];++i){
        UmcSourceReport report; UmiCvmSourceCheckResult result;
        CHECK(umc_check_source(sources[i],strlen(sources[i]),&report,NULL)==UMC_OK);
        CHECK(umi_cvm_check_source(sources[i],strlen(sources[i]),(uint64_t)i,&result,sizeof result)==UMI_STATUS_OK);
        CHECK(result.report.instruction_count==report.instruction_count && result.report.warning_count==report.warning_count);
        for(size_t j=0;j<report.warning_count;++j){
            CHECK(result.report.warnings[j].kind==report.warnings[j].kind);
            CHECK(result.report.warnings[j].source_offset==report.warnings[j].source_offset);
            CHECK(result.report.warnings[j].source_length==report.warnings[j].source_length);
            CHECK(result.report.warnings[j].line==report.warnings[j].line);
            CHECK(result.report.warnings[j].column==report.warnings[j].column);
        }
    }
    return 0;
}
static int independent(void)
{
    UmiCvmSourceCheckResult first,second;
    CHECK(!accepted(&first,1U));
    CHECK(!accepted(&second,2U));
    CHECK(umi_cvm_check_source("",0U,3U,&second,sizeof second)==UMI_STATUS_PARSE_ERROR);
    CHECK(umi_cvm_source_check_matches(&first,sizeof first,1U));
    CHECK(first.report.warning_count==1U && !second.completed);
    char source[]="int main(void){return 42;}";
    CHECK(umi_cvm_check_source(source,strlen(source),4U,&second,sizeof second)==UMI_STATUS_OK);
    source[0]='!';
    /* No hash is promised: the caller, not this token comparison, must advance
     * a revision after an edit. This intentional misuse explains the boundary. */
    CHECK(umi_cvm_source_check_matches(&second,sizeof second,4U));
    CHECK(!umi_cvm_source_check_matches(&second,sizeof second,5U));
    CHECK(second.report.instruction_count==2U);
    return 0;
}
static int prefixes(void)
{
    UmiCvmSourceCheckResult result;
    for(size_t n=0;n<strlen(good);++n){
        CHECK(!accepted(&result,1U));
        CHECK(umi_cvm_check_source(good,n,2U,&result,sizeof result)!=UMI_STATUS_OK);
        CHECK(!result.completed && result.source_revision==2U);
        CHECK(result.report.warning_count==0U && result.report.instruction_count==0U);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const struct {const char *name;int (*run)(void);} cases[]={
        {"success",success},{"reset",reset},{"arguments",arguments},{"storage",storage},
        {"nonexecution",nonexecution},{"parity",parity},{"independent",independent},{"prefixes",prefixes}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)
        if(strcmp(argv[1],cases[i].name)==0)return cases[i].run();
    return 2;
}
