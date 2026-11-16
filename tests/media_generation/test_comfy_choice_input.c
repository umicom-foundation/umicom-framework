/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_choice_input.c
 *
 * PURPOSE:
 *   Verify captured string-choice schemas, decoded values, copied lifetime and unchanged outputs on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_choice_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); failed=1; goto cleanup; } } while (0)
typedef struct Fixture {
    const char *mode, *schema;
    UmiStatus status;
    size_t index;
    const char *value;
    size_t count, options;
} Fixture;
static const Fixture fixtures[] = {
    {"one", "[[\"euler\"]]", UMI_STATUS_OK, 0U, "euler", 1U, 0U},
    {"ordered", "[[\"z\",\"a\",\"m\"]]", UMI_STATUS_OK, 2U, "m", 3U, 0U},
    {"empty-list", "[[]]", UMI_STATUS_OK, 0U, NULL, 0U, 0U},
    {"empty-value", "[[\"\",\"x\"]]", UMI_STATUS_OK, 0U, "", 2U, 0U},
    {"duplicates", "[[\"same\",\"same\"]]", UMI_STATUS_OK, 1U, "same", 2U, 0U},
    {"escaped-duplicate", "[[\"x\",\"\\u0078\"]]", UMI_STATUS_OK, 1U, "x", 2U, 0U},
    {"unicode", "[[\"\\u03bb\",\"\\ud83d\\ude80\"]]", UMI_STATUS_OK, 1U, "🚀", 2U, 0U},
    {"escaped-controls", "[[\"quote\\\" slash\\\\ line\\n\"]]", UMI_STATUS_OK, 0U, "quote\" slash\\ line\n", 1U, 0U},
    {"markup-text", "[[\"<script>not code</script>\"]]", UMI_STATUS_OK, 0U, "<script>not code</script>", 1U, 0U},
    {"opaque-options", "[[\"a\",\"b\"],{\"default\":\"b\",\"forceInput\":true,\"custom\":{\"nested\":7}}]", UMI_STATUS_OK, 0U, "a", 2U, 3U},
    {"empty-options", "[[\"a\"],{}]", UMI_STATUS_OK, 0U, "a", 1U, 0U},
    {"optional-group", "[[\"optional\"]]", UMI_STATUS_OK, 0U, "optional", 1U, 0U},
    {"copy-lifetime", "[[\"copied\"]]", UMI_STATUS_OK, 0U, "copied", 1U, 0U},
    {"late-number", "[[\"first\",7]]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"boolean-choice", "[[true]]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"null-choice", "[[null]]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"object-choice", "[[{\"label\":\"x\",\"value\":\"x\"}]]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"array-choice", "[[[\"nested\"]]]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"named-combo", "[\"COMBO\",{}]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"numeric-type", "[\"INT\",{\"default\":1}]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"string-type", "[\"STRING\"]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"custom-object", "{\"custom\":true}", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"extra-elements", "[[\"x\"],{},7]", UMI_STATUS_NOT_IMPLEMENTED, 0U, NULL, 0U, 0U},
    {"empty-descriptor", "[]", UMI_STATUS_PARSE_ERROR, 0U, NULL, 0U, 0U},
    {"null-type", "[null]", UMI_STATUS_PARSE_ERROR, 0U, NULL, 0U, 0U},
    {"number-type", "[7]", UMI_STATUS_PARSE_ERROR, 0U, NULL, 0U, 0U},
    {"object-type", "[{}]", UMI_STATUS_PARSE_ERROR, 0U, NULL, 0U, 0U},
    {"array-options", "[[\"x\"],[]]", UMI_STATUS_PARSE_ERROR, 0U, NULL, 0U, 0U},
    {"null-options", "[[\"x\"],null]", UMI_STATUS_PARSE_ERROR, 0U, NULL, 0U, 0U}
};
int main(int argc,char **argv)
{
    if (argc != 2) return 2;
    int failed = 0;
    const char *mode = argv[1];
    const Fixture *fixture = NULL;
    for (size_t i=0U;i<sizeof(fixtures)/sizeof(fixtures[0]);++i)
        if (!strcmp(fixtures[i].mode,mode)) fixture=&fixtures[i];
    UmiComfyNodeCatalogue *catalogue = NULL;
    UmiProviderConnection connection = {0};
    strcpy(connection.id,"choice-fixture"); strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Choice fixture"); strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK; connection.timeout_ms=30000; connection.enabled=true;
    char dynamic[12000], json[16000], text[2048], beforeText[sizeof text];
    const char *schema=fixture ? fixture->schema : "[[\"first\",\"second\"]]";
    size_t choice=fixture ? fixture->index : 1U;
    const char *expected=fixture ? fixture->value : "second";
    size_t expectedCount=fixture ? fixture->count : 2U;
    if (!strcmp(mode,"long-choice")) {
        memcpy(dynamic,"[[\"",3U); memset(dynamic+3U,'x',1024U);
        memcpy(dynamic+1027U,"\"]]",4U); schema=dynamic; choice=0U; expectedCount=1U;
    } else if (!strcmp(mode,"many-choices")) {
        size_t used=0U; dynamic[used++]='[';dynamic[used++]='[';
        for(size_t i=0U;i<2048U;++i) {
            if(i)dynamic[used++]=',';
            memcpy(dynamic+used,"\"x\"",3U);used+=3U;
        }
        dynamic[used++]=']';dynamic[used++]=']';dynamic[used]='\0';
        schema=dynamic;choice=2047U;expected="x";expectedCount=2048U;
    }
    UmiComfyInputGroup group=!strcmp(mode,"optional-group")?UMI_COMFY_INPUT_OPTIONAL:UMI_COMFY_INPUT_REQUIRED;
    int bytes=snprintf(json,sizeof json,
        "{\"N\":{\"input\":{\"%s\":{\"prior\":[\"INT\"],\"setting\":%s}},\"output\":[]}}",
        group==UMI_COMFY_INPUT_OPTIONAL?"optional":"required",schema);
    CHECK(bytes>0 && (size_t)bytes<sizeof json);
    CHECK(UmiComfyNodeCatalogueCreate(&connection,9U,json,(size_t)bytes,NULL,&catalogue)==UMI_STATUS_OK);
    UmiComfyNodeCatalogueReview review={0},afterReview={0};
    CHECK(UmiComfyNodeCatalogueInspect(catalogue,&review)==UMI_STATUS_OK);
    UmiComfyNodeInput raw={0};
    CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0U,group,1U,&raw)==UMI_STATUS_OK);
    CHECK(raw.schema_bytes==strlen(schema) && !memcmp(raw.schema_json,schema,raw.schema_bytes));
    UmiComfyChoiceInput report,beforeReport;
    memset(&report,0xa5,sizeof report);memcpy(&beforeReport,&report,sizeof report);
    memset(text,0x5a,sizeof text);memcpy(beforeText,text,sizeof text);
    size_t node=0U,input=1U,capacity=sizeof text;
    const UmiComfyNodeCatalogue *owner=catalogue;
    UmiComfyInputGroup queryGroup=group;
    char *destination=text;
    UmiComfyChoiceInput *reportDestination=&report;
    UmiStatus wanted=fixture?fixture->status:UMI_STATUS_OK;
    if(!strcmp(mode,"null-catalogue")) { owner=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(mode,"invalid-group")) { queryGroup=(UmiComfyInputGroup)0;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(mode,"missing-node")) { node=1U;wanted=UMI_STATUS_NOT_FOUND; }
    else if(!strcmp(mode,"missing-input")) { input=2U;wanted=UMI_STATUS_NOT_FOUND; }
    else if(!strcmp(mode,"missing-group")) { queryGroup=UMI_COMFY_INPUT_OPTIONAL;input=0U;wanted=UMI_STATUS_NOT_FOUND; }
    else if(!strcmp(mode,"null-report")) reportDestination=NULL;
    UmiStatus reportStatus=UmiComfyNodeCatalogueChoiceInput(owner,node,queryGroup,input,reportDestination);
    CHECK(reportStatus==(!reportDestination?UMI_STATUS_INVALID_ARGUMENT:wanted));
    if(reportStatus!=UMI_STATUS_OK) CHECK(!memcmp(&report,&beforeReport,sizeof report));
    else CHECK(!strcmp(report.name,"setting") && report.choice_count==expectedCount &&
        report.uninterpreted_options==(fixture?fixture->options:0U));
    if(!strcmp(mode,"exact-capacity")) capacity=7U;
    else if(!strcmp(mode,"small-capacity")) { capacity=6U;wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(!strcmp(mode,"zero-capacity")) { capacity=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(mode,"null-text")) { destination=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(!strcmp(mode,"missing-choice")) { choice=SIZE_MAX;wanted=UMI_STATUS_NOT_FOUND; }
    if(fixture && fixture->status==UMI_STATUS_OK && fixture->count==0U) wanted=UMI_STATUS_NOT_FOUND;
    UmiStatus textStatus=UmiComfyNodeCatalogueChoiceAt(owner,node,queryGroup,input,choice,destination,capacity);
    CHECK(textStatus==wanted);
    if(textStatus!=UMI_STATUS_OK) CHECK(!memcmp(text,beforeText,sizeof text));
    else if(!strcmp(mode,"long-choice")) {
        CHECK(strlen(text)==1024U);for(size_t i=0U;i<1024U;++i) CHECK(text[i]=='x');
    } else {
        CHECK(expected && !strcmp(text,expected));
        /* Exact-capacity success must not overwrite the rest of the caller buffer. */
        CHECK(!memcmp(text+strlen(expected)+1U,beforeText+strlen(expected)+1U,sizeof text-strlen(expected)-1U));
    }
    CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0U,group,1U,&raw)==UMI_STATUS_OK);
    CHECK(raw.schema_bytes==strlen(schema) && !memcmp(raw.schema_json,schema,raw.schema_bytes));
    CHECK(UmiComfyNodeCatalogueInspect(catalogue,&afterReview)==UMI_STATUS_OK);
    CHECK(review.connection_revision==afterReview.connection_revision);
    CHECK(!memcmp(review.connection_sha256,afterReview.connection_sha256,UMI_SHA256_BYTES));
    CHECK(!memcmp(review.response_sha256,afterReview.response_sha256,UMI_SHA256_BYTES));
    if(!strcmp(mode,"copy-lifetime")) {
        UmiComfyNodeCatalogueDestroy(catalogue);catalogue=NULL;
        CHECK(report.choice_count==1U && !strcmp(report.name,"setting") && !strcmp(text,"copied"));
    }
cleanup:
    UmiComfyNodeCatalogueDestroy(catalogue);
    return failed;
}
