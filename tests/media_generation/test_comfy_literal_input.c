/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_literal_input.c
 *
 * PURPOSE:
 *   Check explicit text and Boolean defaults, opaque options, immutable source observations and unchanged failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_literal_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); failed=1; goto cleanup; } } while(0)
typedef struct Fixture {
    const char *name,*schema;
    UmiStatus status;
    int kind;
    bool known,boolean_value;
    unsigned known_flags,flags;
    size_t unknown;
    const char *text;
} Fixture;
static const Fixture fixtures[] = {
    {"string-minimal", "[\"STRING\"]", UMI_STATUS_OK, 1, false, false, 0U, 0U, 0U, NULL},
    {"boolean-minimal", "[\"BOOLEAN\"]", UMI_STATUS_OK, 2, false, false, 0U, 0U, 0U, NULL},
    {"string-empty", "[\"STRING\",{\"default\":\"\"}]", UMI_STATUS_OK, 1, true, false, 0U, 0U, 0U, ""},
    {"boolean-false", "[\"BOOLEAN\",{\"default\":false}]", UMI_STATUS_OK, 2, true, false, 0U, 0U, 0U, NULL},
    {"boolean-true", "[\"BOOLEAN\",{\"default\":true}]", UMI_STATUS_OK, 2, true, true, 0U, 0U, 0U, NULL},
    {"string-flags", "[\"STRING\",{\"default\":\"hello\",\"forceInput\":true,\"multiline\":true,\"dynamicPrompts\":true}]", UMI_STATUS_OK, 1, true, false, 7U, 7U, 0U, "hello"},
    {"flags-false", "[\"STRING\",{\"default\":\"hello\",\"forceInput\":false,\"multiline\":false,\"dynamicPrompts\":false}]", UMI_STATUS_OK, 1, true, false, 7U, 0U, 0U, "hello"},
    {"boolean-force", "[\"BOOLEAN\",{\"default\":true,\"forceInput\":true}]", UMI_STATUS_OK, 2, true, true, 1U, 1U, 0U, NULL},
    {"boolean-opaque-string-flags", "[\"BOOLEAN\",{\"default\":false,\"multiline\":7,\"dynamicPrompts\":{\"custom\":true},\"label_on\":\"Yes\"}]", UMI_STATUS_OK, 2, true, false, 0U, 0U, 3U, NULL},
    {"unknown-options", "[\"STRING\",{\"default\":\"hello\",\"placeholder\":\"A prompt\",\"tooltip\":{\"opaque\":true},\"custom\":null}]", UMI_STATUS_OK, 1, true, false, 0U, 0U, 3U, "hello"},
    {"missing-default-flags", "[\"STRING\",{\"multiline\":true,\"dynamicPrompts\":false}]", UMI_STATUS_OK, 1, false, false, 6U, 2U, 0U, NULL},
    {"unicode", "[\"STRING\",{\"default\":\"λ🧭é\"}]", UMI_STATUS_OK, 1, true, false, 0U, 0U, 0U, "λ🧭é"},
    {"escaped-controls", "[\"STRING\",{\"default\":\"line1\\nline2\\t\\\"quoted\\\"\\\\path\"}]", UMI_STATUS_OK, 1, true, false, 0U, 0U, 0U, "line1\nline2\t\"quoted\"\\path"},
    {"template-text", "[\"STRING\",{\"default\":\"{red|blue} <script> $(do-not-run)\",\"dynamicPrompts\":true}]", UMI_STATUS_OK, 1, true, false, 4U, 4U, 0U, "{red|blue} <script> $(do-not-run)"},
    {"string-null", "[\"STRING\",{\"default\":null}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 0U, 0U, 0U, NULL},
    {"string-number", "[\"STRING\",{\"default\":7}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 0U, 0U, 0U, NULL},
    {"string-bool", "[\"STRING\",{\"default\":false}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 0U, 0U, 0U, NULL},
    {"bool-null", "[\"BOOLEAN\",{\"default\":null}]", UMI_STATUS_PARSE_ERROR, 2, true, false, 0U, 0U, 0U, NULL},
    {"bool-number", "[\"BOOLEAN\",{\"default\":1}]", UMI_STATUS_PARSE_ERROR, 2, true, false, 0U, 0U, 0U, NULL},
    {"bool-string", "[\"BOOLEAN\",{\"default\":\"true\"}]", UMI_STATUS_PARSE_ERROR, 2, true, false, 0U, 0U, 0U, NULL},
    {"force-number", "[\"STRING\",{\"default\":\"hello\",\"forceInput\":1}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 1U, 1U, 0U, "hello"},
    {"multiline-string", "[\"STRING\",{\"default\":\"hello\",\"multiline\":\"false\"}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 2U, 2U, 0U, "hello"},
    {"dynamic-null", "[\"STRING\",{\"default\":\"hello\",\"dynamicPrompts\":null}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 4U, 0U, 0U, "hello"},
    {"late-invalid-flag", "[\"STRING\",{\"default\":\"hello\",\"forceInput\":true,\"multiline\":true,\"dynamicPrompts\":[]}]", UMI_STATUS_PARSE_ERROR, 1, true, false, 7U, 7U, 0U, "hello"},
    {"options-array", "[\"STRING\",[]]", UMI_STATUS_PARSE_ERROR, 1, false, false, 0U, 0U, 0U, NULL},
    {"options-null", "[\"BOOLEAN\",null]", UMI_STATUS_PARSE_ERROR, 2, false, false, 0U, 0U, 0U, NULL},
    {"empty-descriptor", "[]", UMI_STATUS_PARSE_ERROR, 1, false, false, 0U, 0U, 0U, NULL},
    {"null-type", "[null]", UMI_STATUS_PARSE_ERROR, 1, false, false, 0U, 0U, 0U, NULL},
    {"number-type", "[7]", UMI_STATUS_PARSE_ERROR, 1, false, false, 0U, 0U, 0U, NULL},
    {"object-type", "[{}]", UMI_STATUS_PARSE_ERROR, 1, false, false, 0U, 0U, 0U, NULL},
    {"numeric-type", "[\"INT\",{\"default\":2}]", UMI_STATUS_NOT_IMPLEMENTED, 1, true, false, 0U, 0U, 0U, NULL},
    {"image-type", "[\"IMAGE\"]", UMI_STATUS_NOT_IMPLEMENTED, 1, false, false, 0U, 0U, 0U, NULL},
    {"choice-type", "[[\"one\",\"two\"]]", UMI_STATUS_NOT_IMPLEMENTED, 1, false, false, 0U, 0U, 0U, NULL},
    {"custom-object", "{\"type\":\"STRING\"}", UMI_STATUS_NOT_IMPLEMENTED, 1, false, false, 0U, 0U, 0U, NULL},
    {"extra-elements", "[\"STRING\",{},7]", UMI_STATUS_NOT_IMPLEMENTED, 1, false, false, 0U, 0U, 0U, NULL},
    {"optional-group", "[\"STRING\",{\"default\":\"hello\"}]", UMI_STATUS_OK, 1, true, false, 0U, 0U, 0U, "hello"},
    {"copy-lifetime", "[\"STRING\",{\"default\":\"hello\",\"multiline\":true}]", UMI_STATUS_OK, 1, true, false, 2U, 2U, 0U, "hello"}
};
int main(int argc,char **argv)
{
    int failed=0;
    UmiComfyNodeCatalogue *catalogue=NULL;
    CHECK(argc==2); const char *mode=argv[1];
    const Fixture *fixture=NULL;
    for(size_t i=0;i<sizeof fixtures/sizeof fixtures[0];++i)
        if(strcmp(mode,fixtures[i].name)==0)fixture=&fixtures[i];
    bool special=fixture==NULL;
    const Fixture base={"base","[\"STRING\",{\"default\":\"hello\"}]",UMI_STATUS_OK,1,true,false,0U,0U,0U,"hello"};
    if(special)fixture=&base;
    const char *schema=fixture->schema;
    char longSchema[5100],longExpected[5001];
    if(strcmp(mode,"long-default")==0) {
        memset(longExpected,'x',5000U);longExpected[5000]='\0';
        int n=snprintf(longSchema,sizeof longSchema,"[\"STRING\",{\"default\":\"%s\"}]",longExpected);
        CHECK(n>0 && (size_t)n<sizeof longSchema);schema=longSchema;
    }
    char json[6000],text[5101],savedText[5101];
    UmiComfyInputGroup group=strcmp(mode,"optional-group")==0?UMI_COMFY_INPUT_OPTIONAL:UMI_COMFY_INPUT_REQUIRED;
    int n=snprintf(json,sizeof json,"{\"Example\":{\"input\":{\"%s\":{\"leading\":[\"IMAGE\"],\"setting\":%s}},\"output\":[]}}",
        group==UMI_COMFY_INPUT_OPTIONAL?"optional":"required",schema);
    CHECK(n>0 && (size_t)n<sizeof json);
    UmiProviderConnection connection={0};
    strcpy(connection.id,"literal-fixture");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Captured literals");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;connection.enabled=true;
    CHECK(UmiComfyNodeCatalogueCreate(&connection,19U,json,(size_t)n,NULL,&catalogue)==UMI_STATUS_OK);
    UmiComfyNodeCatalogueReview review={0},after={0};
    CHECK(UmiComfyNodeCatalogueInspect(catalogue,&review)==UMI_STATUS_OK);
    UmiComfyLiteralInput report,savedReport;
    memset(&report,0xa5,sizeof report);memcpy(&savedReport,&report,sizeof report);
    memset(text,0x5a,sizeof text);memcpy(savedText,text,sizeof text);
    const UmiComfyNodeCatalogue *owner=catalogue;
    size_t node=0U,input=1U,capacity=sizeof text;
    UmiComfyInputGroup queryGroup=group;
    UmiComfyLiteralInput *reportOut=&report;char *textOut=text;
    UmiStatus expectedReport=fixture->status,expectedText=fixture->status;
    if(expectedText==UMI_STATUS_OK)
        expectedText=fixture->kind==2?UMI_STATUS_NOT_IMPLEMENTED:fixture->known?UMI_STATUS_OK:UMI_STATUS_NOT_FOUND;
    if(strcmp(mode,"exact-capacity")==0)capacity=6U;
    else if(strcmp(mode,"small-capacity")==0){capacity=5U;expectedText=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"zero-capacity")==0){capacity=0U;expectedText=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-text")==0){textOut=NULL;expectedText=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-report")==0){reportOut=NULL;expectedReport=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"null-catalogue")==0){owner=NULL;expectedReport=expectedText=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"invalid-group")==0){queryGroup=(UmiComfyInputGroup)0;expectedReport=expectedText=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(mode,"missing-node")==0){node=1U;expectedReport=expectedText=UMI_STATUS_NOT_FOUND;}
    else if(strcmp(mode,"missing-input")==0){input=2U;expectedReport=expectedText=UMI_STATUS_NOT_FOUND;}
    else if(strcmp(mode,"missing-group")==0){queryGroup=UMI_COMFY_INPUT_OPTIONAL;expectedReport=expectedText=UMI_STATUS_NOT_FOUND;}
    else if(special && strcmp(mode,"long-default"))CHECK(0);
    CHECK(UmiComfyNodeCatalogueLiteralInput(owner,node,queryGroup,input,reportOut)==expectedReport);
    CHECK(UmiComfyNodeCatalogueStringDefault(owner,node,queryGroup,input,textOut,capacity)==expectedText);
    if(expectedReport==UMI_STATUS_OK) {
        CHECK(strcmp(report.name,"setting")==0 && (int)report.kind==fixture->kind);
        CHECK(report.default_known==fixture->known && report.boolean_default==fixture->boolean_value);
        CHECK(report.known_flags==fixture->known_flags && report.enabled_flags==fixture->flags &&
            report.uninterpreted_options==fixture->unknown);
    } else CHECK(memcmp(&report,&savedReport,sizeof report)==0);
    if(expectedText==UMI_STATUS_OK) {
        const char *expected=strcmp(mode,"long-default")==0?longExpected:fixture->text;
        CHECK(expected!=NULL && strcmp(text,expected)==0);
        size_t used=strlen(expected)+1U;
        CHECK(memcmp(text+used,savedText+used,sizeof text-used)==0);
    } else CHECK(memcmp(text,savedText,sizeof text)==0);
    UmiComfyNodeInput raw={0};
    CHECK(UmiComfyNodeCatalogueInputAt(catalogue,0U,group,1U,&raw)==UMI_STATUS_OK);
    CHECK(raw.schema_bytes==strlen(schema) && memcmp(raw.schema_json,schema,raw.schema_bytes)==0);
    CHECK(UmiComfyNodeCatalogueInspect(catalogue,&after)==UMI_STATUS_OK);
    CHECK(review.connection_revision==after.connection_revision &&
        memcmp(review.connection_sha256,after.connection_sha256,UMI_SHA256_BYTES)==0 &&
        memcmp(review.response_sha256,after.response_sha256,UMI_SHA256_BYTES)==0);
    if(strcmp(mode,"copy-lifetime")==0) {
        UmiComfyNodeCatalogueDestroy(catalogue);catalogue=NULL;
        CHECK(strcmp(text,"hello")==0 && report.default_known && report.enabled_flags==UMI_COMFY_LITERAL_MULTILINE);
    }
cleanup:
    UmiComfyNodeCatalogueDestroy(catalogue);
    return failed;
}
