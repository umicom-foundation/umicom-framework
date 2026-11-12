/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_variable_admission.c
 *
 * PURPOSE:
 *   Verify bounded variable lookup and complete owned mutation on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); free(set); free(before); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name=argv[1];
    UmiDeveloperProjectVariableSet *set=calloc(1U,sizeof *set),*before=malloc(sizeof *before);
    CHECK(set!=NULL&&before!=NULL);
    umi_developer_project_variable_set_init(set);
    if (!strcmp(name,"empty")) {
        CHECK(umi_developer_project_variable_set_validate(set)==UMI_STATUS_OK);
        CHECK(umi_developer_project_variable_get(set,"missing")==NULL);
        free(set); free(before); return 0;
    }
    CHECK(umi_developer_project_variable_set(set,"FIRST","one")==UMI_STATUS_OK);
    CHECK(umi_developer_project_variable_set(set,"SECOND","two")==UMI_STATUS_OK);
    const char *key="THIRD",*value="three";
    char long_name[UMI_DEVELOPER_PROJECT_VARIABLE_NAME_CAPACITY+1U];
    char long_value[UMI_DEVELOPER_PROJECT_VARIABLE_VALUE_CAPACITY+1U];
    memset(long_name,'n',sizeof long_name-1U);long_name[sizeof long_name-1U]='\0';
    memset(long_value,'v',sizeof long_value-1U);long_value[sizeof long_value-1U]='\0';
    UmiStatus expected=UMI_STATUS_OK;
    if (!strcmp(name,"set")) { }
    else if (!strcmp(name,"replace")) { key="FIRST";value="updated"; }
    else if (!strcmp(name,"empty-value")) value="";
    else if (!strcmp(name,"alias-name")) key=set->variables[0].value;
    else if (!strcmp(name,"alias-value")) { key="FIRST";value=set->variables[1].value; }
    else if (!strcmp(name,"long-name")) { key=long_name;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name,"long-value")) { key="FIRST";value=long_value;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name,"long-new-value")) { value=long_value;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name,"count")) { set->count=UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY+1U;expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(name,"unterminated-name")) {
        memset(set->variables[0].name,'x',sizeof set->variables[0].name);expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name,"unterminated-value")) {
        memset(set->variables[0].value,'x',sizeof set->variables[0].value);expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name,"late-invalid") || !strcmp(name,"render-invalid")) {
        memset(set->variables[1].name,'x',sizeof set->variables[1].name);expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name,"duplicate")) {
        strcpy(set->variables[1].name,"FIRST");expected=UMI_STATUS_ALREADY_EXISTS;
    } else if (!strcmp(name,"empty-name")) { set->variables[1].name[0]='\0';expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(name,"full-append") || !strcmp(name,"full-replace")) {
        for(size_t i=2U;i<UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY;++i) {
            char label[32];snprintf(label,sizeof label,"VAR%zu",i);
            CHECK(umi_developer_project_variable_set(set,label,"occupied")==UMI_STATUS_OK);
        }
        if (!strcmp(name,"full-replace")) key="FIRST"; else expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(name,"revision")) { set->revision=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name,"lookup-missing")) {
        CHECK(umi_developer_project_variable_get(set,"missing")==NULL);free(set);free(before);return 0;
    } else { free(set);free(before);return 2; }
    memcpy(before,set,sizeof *before);
    char expected_key[UMI_DEVELOPER_PROJECT_VARIABLE_NAME_CAPACITY];
    char expected_value[UMI_DEVELOPER_PROJECT_VARIABLE_VALUE_CAPACITY];
    if(expected==UMI_STATUS_OK) { strcpy(expected_key,key);strcpy(expected_value,value); }
    CHECK(umi_developer_project_variable_set(set,key,value)==expected);
    if(expected!=UMI_STATUS_OK) {
        CHECK(memcmp(set,before,sizeof *set)==0);
        if(expected==UMI_STATUS_INVALID_ARGUMENT || expected==UMI_STATUS_ALREADY_EXISTS)
            CHECK(umi_developer_project_variable_get(set,"FIRST")==NULL);
        if(!strcmp(name,"render-invalid")) {
            char output[32]="previous";
            CHECK(umi_developer_project_render_text("{{FIRST}}",set,output,sizeof output)==expected);
            CHECK(!strcmp(output,"previous"));
        }
    } else {
        const char *observed=umi_developer_project_variable_get(set,expected_key);
        CHECK(observed!=NULL&&!strcmp(observed,expected_value));
        CHECK(set->revision==before->revision+1U);
        CHECK(umi_developer_project_variable_set_validate(set)==UMI_STATUS_OK);
    }
    free(set);free(before);return 0;
}
