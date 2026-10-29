/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_text_query.c
 *
 * PURPOSE:
 *   Verify pre-dispatch query validation is bounded, side-effect free and shared by SDK callers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_text.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);return 1; } } while(0)
static int Cancel(void *user)
{
    ++*(unsigned *)user;
    return 1;
}
static int Check(const char *name)
{
    UmiLearningTextQuery query, before;
    UmiLearningTextQueryInit(&query);
    memcpy(query.phrase, "ownership", sizeof("ownership"));
    UmiStatus expected = UMI_STATUS_OK;
    unsigned calls = 0U;
    int known = 1;
    if (strcmp(name,"null") == 0) {
        CHECK(UmiLearningTextQueryValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);return 0;
    } else if (strcmp(name,"empty") == 0) {
        query.phrase[0]='\0';expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"phrase-capacity") == 0) {
        memset(query.phrase,'x',sizeof(query.phrase));expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name,"product-capacity") == 0) {
        memset(query.product,'x',sizeof(query.product));expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name,"locale-capacity") == 0) {
        memset(query.locale,'x',sizeof(query.locale));expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name,"phrase-utf8") == 0) {
        query.phrase[0]=(char)0xff;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"product-utf8") == 0) {
        query.product[0]=(char)0xff;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"locale-utf8") == 0) {
        query.locale[0]=(char)0xff;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"control") == 0) {
        query.product[0]='\n';expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"del") == 0) {
        query.locale[0]=(char)127;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"zero-results") == 0) {
        query.maximum_results=0U;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"many-results") == 0) {
        query.maximum_results=UMI_LEARNING_TEXT_RESULTS_MAX+1U;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"kind") == 0) {
        query.kind=99U;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"case") == 0) {
        query.case_sensitive=-1;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"word") == 0) {
        query.whole_word=2;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name,"callback-not-called") == 0) {
        query.cancellation=Cancel;query.cancellation_user=&calls;
    } else if (strcmp(name,"maximum") == 0) {
        memset(query.phrase,'x',sizeof(query.phrase)-1U);
        query.phrase[sizeof(query.phrase)-1U]='\0';
        query.maximum_results=UMI_LEARNING_TEXT_RESULTS_MAX;
    } else if (strcmp(name,"locale") == 0) memcpy(query.locale,"en-GB",sizeof("en-GB"));
    else if (strcmp(name,"unicode") == 0) memcpy(query.phrase,"caf\xc3\xa9",sizeof("caf\xc3\xa9"));
    else if (strcmp(name,"image") == 0) query.kind=UMI_LEARNING_RESOURCE_IMAGE;
    else if (strcmp(name,"default") != 0) known=0;
    CHECK(known);
    memcpy(&before,&query,sizeof(query));
    CHECK(UmiLearningTextQueryValidate(&query) == expected);
    CHECK(memcmp(&query,&before,sizeof(query)) == 0 && calls == 0U);
    return 0;
}
int main(int argc,char **argv)
{
    return argc == 3 ? Check(argv[1]) : 2;
}
