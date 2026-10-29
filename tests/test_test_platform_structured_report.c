/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_structured_report.c
 *
 * PURPOSE:
 *   Verify deterministic escaped JSON for results and session summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Preserve the original smoke entry point for review; active cases retain
 * it and inspect output with the independent shared JSON parser. */
#if 0
int main(void)
{
    UmiTestPlatformResultSnapshot result;
    UmiTestPlatformRunSessionSnapshot session;
    char json[8192];
    (void)memset(&result, 0, sizeof(result));
    (void)strcpy(result.id, "result.1");
    (void)strcpy(result.session_id, "session.1");
    (void)strcpy(result.item_id, "framework.alpha");
    (void)strcpy(result.message, "assertion failed");
    (void)strcpy(result.failure_details, "expected \"yes\"\nactual no");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    result.duration_ms = 12.5;
    result.exit_code = 1;
    result.sequence = 7U;
    assert(umi_test_platform_report_result_json(&result, json, sizeof(json)) ==
           UMI_STATUS_OK);
    assert(strstr(json, "\"outcome\":\"failed\"") != NULL);
    assert(strstr(json, "expected \\\"yes\\\"\\nactual no") != NULL);

    (void)memset(&session, 0, sizeof(session));
    (void)strcpy(session.id, "session.1");
    (void)strcpy(session.profile_id, "profile.default");
    (void)strcpy(session.suite_id, "framework.ctest");
    session.total = 3U;
    session.passed = 2U;
    session.failed = 1U;
    assert(umi_test_platform_report_session_json(&session, json,
                                                sizeof(json)) ==
           UMI_STATUS_OK);
    assert(strstr(json, "\"total\":3") != NULL);
    assert(strstr(json, "\"failed\":1") != NULL);
    return 0;
}
#endif
static int Smoke(void)
{
    UmiTestPlatformResultSnapshot result;
    UmiTestPlatformRunSessionSnapshot session;
    char json[8192];
    (void)memset(&result, 0, sizeof(result));
    (void)strcpy(result.id, "result.1");
    (void)strcpy(result.session_id, "session.1");
    (void)strcpy(result.item_id, "framework.alpha");
    (void)strcpy(result.message, "assertion failed");
    (void)strcpy(result.failure_details, "expected \"yes\"\nactual no");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    result.duration_ms = 12.5;
    result.exit_code = 1;
    result.sequence = 7U;
    assert(umi_test_platform_report_result_json(&result, json, sizeof(json)) ==
           UMI_STATUS_OK);
    assert(strstr(json, "\"outcome\":\"failed\"") != NULL);
    assert(strstr(json, "expected \\\"yes\\\"\\nactual no") != NULL);

    (void)memset(&session, 0, sizeof(session));
    (void)strcpy(session.id, "session.1");
    (void)strcpy(session.profile_id, "profile.default");
    (void)strcpy(session.suite_id, "framework.ctest");
    session.total = 3U;
    session.passed = 2U;
    session.failed = 1U;
    assert(umi_test_platform_report_session_json(&session, json,
                                                sizeof(json)) ==
           UMI_STATUS_OK);
    assert(strstr(json, "\"total\":3") != NULL);
    assert(strstr(json, "\"failed\":1") != NULL);
    return 0;
}

#include "umicom/language_runtime/json_tree.h"
#include <float.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void Result(UmiTestPlatformResultSnapshot *result)
{
    memset(result,0,sizeof(*result));strcpy(result->id,"result.1");
    strcpy(result->session_id,"session.1");strcpy(result->item_id,"case.1");
    result->outcome=UMI_TEST_PLATFORM_OUTCOME_SKIPPED;result->duration_ms=12.5;
}
static void Session(UmiTestPlatformRunSessionSnapshot *session)
{
    memset(session,0,sizeof(*session));strcpy(session->id,"session.1");
    strcpy(session->profile_id,"profile.1");strcpy(session->suite_id,"suite.1");
    session->total=3U;session->passed=1U;session->failed=1U;session->skipped=1U;
}
static void ReadText(const char *json,const char *field,const char *expected)
{
    UmiJsonTree *tree=NULL;int node=-1;char value[4096];
    assert(UmiJsonTreeCreate(json,strlen(json),NULL,NULL,&tree)==UMI_STATUS_OK);
    assert(UmiJsonTreeMember(tree,0,field,&node)==UMI_STATUS_OK);
    assert(UmiJsonTreeText(tree,node,value,sizeof(value))==UMI_STATUS_OK);
    assert(strcmp(value,expected)==0);UmiJsonTreeDestroy(tree);
}
int main(int argc,char **argv)
{
    if(argc==1)return Smoke();
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiTestPlatformResultSnapshot result;Result(&result);
    UmiTestPlatformRunSessionSnapshot session;Session(&session);
    char *json=malloc(UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);assert(json!=NULL);
    memset(json,0x6b,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
    UmiStatus status=UMI_STATUS_OK;int refused=0,known=1;
    if(strcmp(name,"controls")==0) {
        for(unsigned i=1U;i<32U;++i)result.message[i-1U]=(char)i;
        result.message[31]='\0';
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_OK);ReadText(json,"message",result.message);
        for(const unsigned char *p=(const unsigned char*)json;*p;++p)assert(*p>=0x20U);
    } else if(strcmp(name,"unicode")==0) {
        strcpy(result.message,"caf\xc3\xa9 \xce\xa9 \xf0\x9f\x8c\x8d");
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_OK);ReadText(json,"message",result.message);
    } else if(strcmp(name,"maximum-escapes")==0) {
        memset(result.id,1,sizeof(result.id)-1U);memset(result.session_id,2,sizeof(result.session_id)-1U);
        memset(result.item_id,3,sizeof(result.item_id)-1U);memset(result.message,4,sizeof(result.message)-1U);
        memset(result.failure_details,5,sizeof(result.failure_details)-1U);
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_OK);ReadText(json,"failureDetails",result.failure_details);
    } else if(strcmp(name,"short-result")==0 || strcmp(name,"exact-result")==0) {
        char normal[4096];assert(umi_test_platform_report_result_json(&result,normal,sizeof(normal))==UMI_STATUS_OK);
        size_t bytes=strlen(normal);
        status=umi_test_platform_report_result_json(&result,json,bytes+(strcmp(name,"exact-result")==0?1U:0U));
        if(strcmp(name,"exact-result")==0){assert(status==UMI_STATUS_OK);assert(strcmp(json,normal)==0);}
        else {assert(status==UMI_STATUS_CAPACITY_EXCEEDED);refused=1;}
    } else if(strcmp(name,"nan")==0 || strcmp(name,"infinity")==0 || strcmp(name,"negative-duration")==0) {
        result.duration_ms=strcmp(name,"nan")==0?NAN:strcmp(name,"infinity")==0?INFINITY:-1.0;
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else if(strcmp(name,"maximum-duration")==0) {
        result.duration_ms=DBL_MAX;
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_OK);UmiJsonTree *tree=NULL;
        assert(UmiJsonTreeCreate(json,strlen(json),NULL,NULL,&tree)==UMI_STATUS_OK);UmiJsonTreeDestroy(tree);
    } else if(strcmp(name,"locale-duration")==0) {
        const char *current=setlocale(LC_NUMERIC,NULL);assert(current!=NULL);
        char saved[256];assert(strlen(current)<sizeof(saved));strcpy(saved,current);
        static const char *locales[]={"de_DE.UTF-8","fr_FR.UTF-8","German_Germany.1252","French_France.1252","C"};
        int available=0, alternate=0;
        for(size_t i=0U;i<sizeof(locales)/sizeof(locales[0]);++i) {
            if(setlocale(LC_NUMERIC,locales[i])==NULL)continue;
            if(strcmp(localeconv()->decimal_point,".")!=0)alternate=1;
            ++available;assert(umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY)==UMI_STATUS_OK);
            assert(strstr(json,"\"durationMs\":12.500,")!=NULL);
        }
        assert(setlocale(LC_NUMERIC,saved)!=NULL);assert(available>0);
        if(!alternate){fprintf(stderr,"No installed non-dot numeric locale; alternate-locale coverage unavailable.\n");free(json);return 77;}
    } else if(strcmp(name,"outcome")==0 || strcmp(name,"empty-id")==0) {
        if(strcmp(name,"outcome")==0)result.outcome=99;else result.id[0]='\0';
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else if(strncmp(name,"unterminated-",13U)==0) {
        struct Field{const char *name;char *bytes;size_t size;}fields[]={
            {"unterminated-id",result.id,sizeof(result.id)},
            {"unterminated-session",result.session_id,sizeof(result.session_id)},
            {"unterminated-item",result.item_id,sizeof(result.item_id)},
            {"unterminated-message",result.message,sizeof(result.message)},
            {"unterminated-details",result.failure_details,sizeof(result.failure_details)},
            {"unterminated-attachment",result.attachment_id,sizeof(result.attachment_id)}};
        known=0;for(size_t i=0U;i<sizeof(fields)/sizeof(fields[0]);++i)if(strcmp(name,fields[i].name)==0){memset(fields[i].bytes,'x',fields[i].size);known=1;}
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else if(strncmp(name,"utf8-",5U)==0) {
        const char *bad=strcmp(name,"utf8-overlong")==0?"\xc0\xaf":strcmp(name,"utf8-surrogate")==0?"\xed\xa0\x80":
            strcmp(name,"utf8-range")==0?"\xf4\x90\x80\x80":"\xe2\x82";
        strcpy(result.message,bad);
        status=umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY);
        assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else if(strcmp(name,"skipped-result")==0) {
        assert(umi_test_platform_report_result_json(&result,json,UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY)==UMI_STATUS_OK);
        ReadText(json,"outcome","skipped");
    } else if(strcmp(name,"session-controls")==0 || strcmp(name,"session-maximum")==0) {
        if(strcmp(name,"session-maximum")==0){memset(session.id,1,sizeof(session.id)-1U);memset(session.profile_id,2,sizeof(session.profile_id)-1U);memset(session.suite_id,3,sizeof(session.suite_id)-1U);}
        else strcpy(session.profile_id,"profile\x01\b\f");
        status=umi_test_platform_report_session_json(&session,json,UMI_TEST_PLATFORM_SESSION_JSON_CAPACITY);
        assert(status==UMI_STATUS_OK);ReadText(json,"profileId",session.profile_id);
    } else if(strcmp(name,"session-short")==0 || strcmp(name,"session-exact")==0) {
        char normal[4096];assert(umi_test_platform_report_session_json(&session,normal,sizeof(normal))==UMI_STATUS_OK);
        status=umi_test_platform_report_session_json(&session,json,strlen(normal)+(strcmp(name,"session-exact")==0?1U:0U));
        if(strcmp(name,"session-exact")==0){assert(status==UMI_STATUS_OK);assert(strcmp(json,normal)==0);}
        else {assert(status==UMI_STATUS_CAPACITY_EXCEEDED);refused=1;}
    } else if(strcmp(name,"session-unterminated")==0 || strcmp(name,"session-utf8")==0) {
        if(strcmp(name,"session-unterminated")==0)memset(session.suite_id,'x',sizeof(session.suite_id));
        else strcpy(session.suite_id,"\xc3");
        status=umi_test_platform_report_session_json(&session,json,UMI_TEST_PLATFORM_SESSION_JSON_CAPACITY);
        assert(status==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else if(strcmp(name,"session-skipped")==0) {
        assert(umi_test_platform_report_session_json(&session,json,UMI_TEST_PLATFORM_SESSION_JSON_CAPACITY)==UMI_STATUS_OK);
        assert(strstr(json,"\"passed\":1,\"failed\":1,\"skipped\":1")!=NULL);
    } else if(strcmp(name,"arguments")==0) {
        assert(umi_test_platform_report_result_json(NULL,json,4096U)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_platform_report_result_json(&result,NULL,4096U)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_platform_report_result_json(&result,json,0U)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_platform_report_session_json(NULL,json,4096U)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_platform_report_session_json(&session,NULL,4096U)==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_test_platform_report_session_json(&session,json,0U)==UMI_STATUS_INVALID_ARGUMENT);refused=1;
    } else known=0;
    if(refused)for(size_t i=0U;i<UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY;++i)assert((unsigned char)json[i]==0x6bU);
    free(json);return known?0:2;
}
