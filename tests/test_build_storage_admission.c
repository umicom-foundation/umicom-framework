/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_storage_admission.c
 *
 * PURPOSE:
 *   Exercise bounded build evidence, loss counters and history refusal without launching tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#expression); \
    failed=1;goto finished; } } while (0)

int main(int argc,char **argv)
{
    if (argc!=2)return 2;
    const char *name=argv[1];
    int failed=0;
    UmiBuildResult *result=calloc(1,sizeof *result),*before=calloc(1,sizeof *before);
    UmiBuildHistory *history=NULL;
    CHECK(result!=NULL&&before!=NULL);
    umi_build_result_init(result,7U,UMI_BUILD_PHASE_BUILD,"fixture");
    UmiBuildDiagnosticList *list=&result->diagnostics;
    UmiBuildDiagnostic item={0};
    item.severity=UMI_BUILD_DIAGNOSTIC_ERROR;
    strcpy(item.file,"fixture.c");strcpy(item.code,"E1");strcpy(item.message,"synthetic error");
    char summary[512]="previous";
    if (!strcmp(name,"valid.empty")) {
        UmiBuildDiagnostic empty={0};
        CHECK(UmiBuildDiagnosticValidate(&empty)==UMI_STATUS_OK);
        CHECK(UmiBuildDiagnosticListValidate(list)==UMI_STATUS_OK);
        CHECK(UmiBuildResultValidateStorage(result)==UMI_STATUS_OK);
    } else if (!strcmp(name,"valid.inactive")||!strcmp(name,"result.inactive")) {
        memset(&list->items[UMI_BUILD_MAX_DIAGNOSTICS-1U],0xff,sizeof item);
        CHECK(UmiBuildDiagnosticListValidate(list)==UMI_STATUS_OK);
        CHECK(UmiBuildResultValidateStorage(result)==UMI_STATUS_OK);
        CHECK(umi_build_diagnostic_list_at(list,0)==NULL);
    } else if (!strcmp(name,"valid.locations")) {
        item.line=SIZE_MAX;item.column=SIZE_MAX;
        CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_OK);
        CHECK(umi_build_diagnostic_list_at(list,0)->line==SIZE_MAX);
    } else if (!strcmp(name,"null.arguments")) {
        CHECK(UmiBuildDiagnosticValidate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildDiagnosticListValidate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildResultValidateStorage(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_diagnostic_list_add(list,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_diagnostic_list_add(NULL,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_diagnostic_list_at(NULL,0)==NULL);
        CHECK(umi_build_diagnostic_list_count_severity(NULL,UMI_BUILD_DIAGNOSTIC_NOTE)==0U);
    } else if (!strncmp(name,"record.",7)) {
        if (!strcmp(name,"record.file"))memset(item.file,'x',sizeof item.file);
        else if (!strcmp(name,"record.code"))memset(item.code,'x',sizeof item.code);
        else if (!strcmp(name,"record.message"))memset(item.message,'x',sizeof item.message);
        else if (!strcmp(name,"record.severity-high"))item.severity=(UmiBuildDiagnosticSeverity)99;
        else if (!strcmp(name,"record.severity-negative"))item.severity=(UmiBuildDiagnosticSeverity)-1;
        else CHECK(0);
        *before=*result;
        CHECK(UmiBuildDiagnosticValidate(&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(result,before,sizeof *result));
        list->items[0]=item;list->count=1U;
        CHECK(UmiBuildDiagnosticListValidate(list)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_diagnostic_list_at(list,0)==NULL);
        CHECK(umi_build_diagnostic_list_count_severity(list,UMI_BUILD_DIAGNOSTIC_NOTE)==0U);
    } else if (!strcmp(name,"count.over")||!strcmp(name,"count.maximum")) {
        list->count=!strcmp(name,"count.over")?UMI_BUILD_MAX_DIAGNOSTICS+1U:SIZE_MAX;
        *before=*result;
        CHECK(UmiBuildDiagnosticListValidate(list)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_diagnostic_list_at(list,0)==NULL);
        CHECK(umi_build_diagnostic_list_at(list,UMI_BUILD_MAX_DIAGNOSTICS)==NULL);
        CHECK(umi_build_diagnostic_list_count_severity(list,UMI_BUILD_DIAGNOSTIC_NOTE)==0U);
        CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(result,before,sizeof *result));
    } else if (!strcmp(name,"index.boundary")||!strcmp(name,"index.last-invalid")||
               !strcmp(name,"full.dropped")||!strcmp(name,"full.saturated")) {
        for (size_t index=0;index<UMI_BUILD_MAX_DIAGNOSTICS;++index)
            CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_OK);
        CHECK(umi_build_diagnostic_list_at(list,UMI_BUILD_MAX_DIAGNOSTICS)==NULL);
        CHECK(umi_build_diagnostic_list_at(list,SIZE_MAX)==NULL);
        if (!strcmp(name,"index.last-invalid")) {
            memset(list->items[UMI_BUILD_MAX_DIAGNOSTICS-1U].message,'x',sizeof item.message);
            CHECK(UmiBuildDiagnosticListValidate(list)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_build_diagnostic_list_at(list,UMI_BUILD_MAX_DIAGNOSTICS-1U)==NULL);
        } else if (!strncmp(name,"full.",5)) {
            list->dropped=!strcmp(name,"full.saturated")?SIZE_MAX-1U:0U;
            size_t expected=!strcmp(name,"full.saturated")?SIZE_MAX:2U;
            *before=*result;
            CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_CAPACITY_EXCEEDED);
            before->diagnostics.dropped=expected;
            CHECK(!memcmp(result,before,sizeof *result));
        }
    } else if (!strcmp(name,"append.alias")) {
        CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_OK);
        CHECK(umi_build_diagnostic_list_add(list,&list->items[0])==UMI_STATUS_OK);
        CHECK(list->count==2U&&!memcmp(&list->items[0],&list->items[1],sizeof item));
    } else if (!strcmp(name,"severity.thresholds")) {
        for (unsigned severity=0;severity<4U;++severity) {
            item.severity=(UmiBuildDiagnosticSeverity)severity;
            CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_OK);
        }
        for (unsigned severity=0;severity<4U;++severity)
            CHECK(umi_build_diagnostic_list_count_severity(list,(UmiBuildDiagnosticSeverity)severity)==4U-severity);
    } else if (!strcmp(name,"severity.invalid-high")||!strcmp(name,"severity.invalid-negative")) {
        CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_OK);
        UmiBuildDiagnosticSeverity minimum=(UmiBuildDiagnosticSeverity)(!strcmp(name,"severity.invalid-high")?99:-1);
        CHECK(umi_build_diagnostic_list_count_severity(list,minimum)==0U);
    } else if (!strncmp(name,"result.",7)||!strncmp(name,"summary.",8)) {
        if (!strcmp(name,"result.profile"))memset(result->profile_id,'x',sizeof result->profile_id);
        else if (!strcmp(name,"result.command"))memset(result->command,'x',sizeof result->command);
        else if (!strcmp(name,"result.output"))memset(result->output,'x',sizeof result->output);
        else if (!strcmp(name,"result.diagnostics")||!strcmp(name,"summary.invalid"))list->count=SIZE_MAX;
        else if (!strcmp(name,"result.unknown-phase")) {
            result->phase=(UmiBuildPhase)99;
            CHECK(UmiBuildResultValidateStorage(result)==UMI_STATUS_OK);
            goto finished;
        } else if (!strcmp(name,"summary.valid")||!strcmp(name,"summary.capacity")) {
            CHECK(umi_build_diagnostic_list_add(list,&item)==UMI_STATUS_OK);
            CHECK(umi_build_result_summary(result,summary,!strcmp(name,"summary.valid")?sizeof summary:1U)==
                (!strcmp(name,"summary.valid")?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED));
            if (!strcmp(name,"summary.valid"))CHECK(strstr(summary,"errors=1")&&strstr(summary,"unrepresented=0"));
            goto finished;
        } else CHECK(0);
        *before=*result;
        CHECK(UmiBuildResultValidateStorage(result)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_result_summary(result,summary,sizeof summary)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!strcmp(summary,"previous")&&!memcmp(result,before,sizeof *result));
    } else if (!strncmp(name,"history.",8)) {
        CHECK(umi_build_history_create(2U,&history)==UMI_STATUS_OK);
        CHECK(umi_build_history_append(history,result)==UMI_STATUS_OK);
        result->operation_id=8U;
        CHECK(umi_build_history_append(history,result)==UMI_STATUS_OK);
        result->operation_id=UINT64_MAX;
        if (!strcmp(name,"history.profile"))memset(result->profile_id,'x',sizeof result->profile_id);
        else if (!strcmp(name,"history.command"))memset(result->command,'x',sizeof result->command);
        else if (!strcmp(name,"history.output"))memset(result->output,'x',sizeof result->output);
        else if (!strcmp(name,"history.live-message")) {
            list->count=1U;list->items[0]=item;
            memset(list->items[0].message,'x',sizeof item.message);
        } else if (!strcmp(name,"history.diagnostics")||!strcmp(name,"history.full-refusal")||
                   !strcmp(name,"history.identity-refusal"))list->count=SIZE_MAX;
        else CHECK(0);
        *before=*result;
        CHECK(umi_build_history_append(history,result)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(result,before,sizeof *result));
        CHECK(umi_build_history_count(history)==2U);
        CHECK(umi_build_history_at(history,0U,before)==UMI_STATUS_OK&&before->operation_id==7U);
        CHECK(umi_build_history_latest(history,before)==UMI_STATUS_OK&&before->operation_id==8U);
        uint64_t next=0;
        CHECK(UmiBuildHistoryReserveOperationId(history,&next)==UMI_STATUS_OK&&next==9U);
    } else CHECK(0);
finished:
    umi_build_history_destroy(history);
    free(before);free(result);
    return failed;
}
