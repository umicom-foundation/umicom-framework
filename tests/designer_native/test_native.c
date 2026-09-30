/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_native/test_native.c
 * PURPOSE:
 *   Regression assertions stay active in Release builds.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Regression assertions stay active in Release builds. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/designer/native_project.h"
#include "umicom/declarative/lexer.h"
#include "umicom/declarative/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while(0)
#define OK(call) CHECK((call)==UMI_STATUS_OK)

static UmiStatus Add(UmiDeclDocument *doc,const char *id,const char *type,const char *parent)
{
    UmiDeclNode node;
    UmiStatus status=umi_decl_node_init(&node,id,type,parent);
    return status==UMI_STATUS_OK ? umi_decl_document_add_node(doc,&node) : status;
}
static UmiStatus Set(UmiDeclDocument *doc,const char *id,const char *name,UmiDeclValueKind kind,const char *value)
{
    UmiDeclNode node;
    UmiStatus status=umi_decl_document_find_node(doc,id,&node);
    if(status==UMI_STATUS_OK) status=umi_decl_node_set_attribute(&node,name,kind,value);
    return status==UMI_STATUS_OK ? umi_decl_document_update_node(doc,&node) : status;
}
static int Same(const UmiDeclDocument *a,const UmiDeclDocument *b)
{
    size_t count=umi_decl_document_node_count(a);
    if(count!=umi_decl_document_node_count(b)) return 0;
    for(size_t i=0U;i<count;++i) {
        UmiDeclNode na,nb;
        if(umi_decl_document_node_at(a,i,&na)!=UMI_STATUS_OK ||
           umi_decl_document_node_at(b,i,&nb)!=UMI_STATUS_OK || memcmp(&na,&nb,sizeof na)!=0) return 0;
    }
    return 1;
}

static int Removal(const char *mode)
{
    UmiDeclDocument *doc=NULL,*before=NULL;
    UmiDeclNode node;
    UmiDeclDocumentSnapshot s,t;
    OK(umi_decl_document_create("practice",&doc));
    const char *ids[]={"root","child","grandchild","other","otherchild"};
    const char *parents[]={"","root","child","","other"};
    for(size_t j=0U;j<5U;++j) {
        size_t i=strcmp(mode,"remove_reverse")==0 ? 4U-j : j;
        OK(Add(doc,ids[i],"pane",parents[i]));
    }
    OK(umi_decl_document_snapshot(doc,&s));
    OK(umi_decl_document_clone(doc,&before));
    if(strcmp(mode,"remove_missing")==0) {
        CHECK(umi_decl_document_remove_node(doc,"missing")==UMI_STATUS_NOT_FOUND);
        CHECK(Same(doc,before));
        OK(umi_decl_document_snapshot(doc,&t)); CHECK(t.revision==s.revision);
    } else {
        OK(umi_decl_document_remove_node(doc,"root"));
        CHECK(umi_decl_document_node_count(doc)==2U);
        CHECK(umi_decl_document_find_node(doc,"grandchild",&node)==UMI_STATUS_NOT_FOUND);
        OK(umi_decl_document_snapshot(doc,&t)); CHECK(t.revision==s.revision+1U);
        OK(umi_decl_document_node_at(doc,0U,&node));
        CHECK(strcmp(node.node_id,strcmp(mode,"remove_reverse")==0 ? "otherchild" : "other")==0);
        CHECK(umi_decl_document_node_count(before)==5U);
    }
    umi_decl_document_destroy(doc); umi_decl_document_destroy(before); return 0;
}
static int DeepRemoval(void)
{
    UmiDeclDocument *doc=NULL;
    OK(umi_decl_document_create("practice",&doc));
    for(size_t j=0U;j<512U;++j) {
        size_t i=511U-j; char id[32],parent[32];
        snprintf(id,sizeof id,"node.%zu",i); snprintf(parent,sizeof parent,"node.%zu",i==0U?511U:i-1U);
        OK(Add(doc,id,"pane",parent));
    }
    OK(umi_decl_document_remove_node(doc,"node.0"));
    CHECK(umi_decl_document_node_count(doc)==0U);
    umi_decl_document_destroy(doc); return 0;
}

static int NodeChecks(const char *mode)
{
    UmiDeclNode node,before;
    UmiDeclAttribute attribute;
    OK(umi_decl_node_init(&node,"title","text","pane"));
    OK(umi_decl_node_set_attribute(&node,"value",UMI_DECL_VALUE_INTEGER,"12"));
    before=node;
    if(strcmp(mode,"failed_property")==0) {
        CHECK(umi_decl_node_set_attribute(&node,"value",UMI_DECL_VALUE_INTEGER,"bad")==UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&node,&before,sizeof node)==0);
    } else if(strcmp(mode,"alias_property")==0) {
        OK(umi_decl_node_set_attribute(&node,node.attributes[0].name,UMI_DECL_VALUE_INTEGER,node.attributes[0].value.text));
        CHECK(memcmp(&node,&before,sizeof node)==0);
        OK(umi_decl_node_init(&node,node.node_id,node.component_type,node.parent_id));
        CHECK(strcmp(node.node_id,"title")==0 && node.attribute_count==0U);
    } else if(strcmp(mode,"full_attributes")==0) {
        for(size_t i=1U;i<32U;++i) { char id[32]; snprintf(id,sizeof id,"p%zu",i); OK(umi_decl_node_set_attribute(&node,id,UMI_DECL_VALUE_STRING,"text")); }
        before=node; CHECK(umi_decl_node_set_attribute(&node,"extra",UMI_DECL_VALUE_STRING,"value")==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&node,&before,sizeof node)==0);
        OK(umi_decl_node_set_attribute(&node,"value",UMI_DECL_VALUE_INTEGER,"14"));
        OK(umi_decl_node_remove_attribute(&node,"value")); CHECK(node.attribute_count==31U);
    } else if(strcmp(mode,"bad_attribute_count")==0) {
        node.attribute_count=SIZE_MAX; before=node;
        CHECK(UmiDeclNodeValidate(&node)!=UMI_STATUS_OK);
        CHECK(umi_decl_node_get_attribute(&node,"value",&attribute)!=UMI_STATUS_OK);
        CHECK(umi_decl_node_remove_attribute(&node,"value")!=UMI_STATUS_OK);
        CHECK(umi_decl_node_set_attribute(&node,"value",UMI_DECL_VALUE_INTEGER,"1")!=UMI_STATUS_OK);
        CHECK(memcmp(&node,&before,sizeof node)==0);
    } else if(strcmp(mode,"unterminated_node")==0) {
        memset(node.node_id,'x',sizeof node.node_id); CHECK(UmiDeclNodeValidate(&node)!=UMI_STATUS_OK);
    } else if(strcmp(mode,"duplicate_attribute")==0) {
        node.attributes[1]=node.attributes[0]; node.attribute_count=2U;
        CHECK(UmiDeclNodeValidate(&node)!=UMI_STATUS_OK);
    } else if(strcmp(mode,"invalid_parent")==0) {
        CHECK(umi_decl_node_init(&node,"title","text","bad parent")!=UMI_STATUS_OK);
        CHECK(memcmp(&node,&before,sizeof node)==0);
    } else if(strcmp(mode,"rejected_document_node")==0) {
        UmiDeclDocument *doc=NULL; OK(umi_decl_document_create("practice",&doc));
        node.attribute_count=33U; CHECK(umi_decl_document_add_node(doc,&node)!=UMI_STATUS_OK);
        CHECK(umi_decl_document_node_count(doc)==0U); umi_decl_document_destroy(doc);
    } else return 1;
    return 0;
}

static int LexerChecks(const char *mode)
{
    UmiDeclTokenLine tokens;
    if(strcmp(mode,"unclosed_quote")==0) {
        CHECK(umi_decl_lexer_split_line("property notes text \"unfinished",&tokens)==UMI_STATUS_PARSE_ERROR);
        CHECK(umi_decl_lexer_split_line("property notes text 'unfinished",&tokens)==UMI_STATUS_PARSE_ERROR);
    } else {
        OK(umi_decl_lexer_split_line("property title text \"\" # comment",&tokens));
        CHECK(tokens.count==4U && tokens.tokens[3][0]=='\0');
        OK(umi_decl_lexer_split_line("property title text 'a # b'",&tokens));
        CHECK(strcmp(tokens.tokens[3],"a # b")==0);
    }
    return 0;
}

static int Profile(const char *mode)
{
    UmiDeclDocument *doc=NULL,*before=NULL;
    UmiDeclNode node;
    char message[256]; UmiStatus expected=UMI_STATUS_OK;
    OK(UmiDesignerNativeNotesDocument(&doc));
    if(strcmp(mode,"unknown_component")==0) { OK(Add(doc,"extra","browser","content")); expected=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"unknown_property")==0) { OK(Set(doc,"notes","script",UMI_DECL_VALUE_STRING,"run")); expected=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"wrong_type")==0) { OK(Set(doc,"window","width",UMI_DECL_VALUE_STRING,"800")); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"size_range")==0) { OK(Set(doc,"window","width",UMI_DECL_VALUE_INTEGER,"9000")); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"bad_utf8")==0) { OK(Set(doc,"notes","text",UMI_DECL_VALUE_STRING,"\300\257")); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"numeric_string")==0) OK(Set(doc,"notes","text",UMI_DECL_VALUE_STRING,"123"));
    else if(strcmp(mode,"contradictory_value")==0) {
        OK(umi_decl_document_find_node(doc,"window",&node)); node.attributes[1].value.integer_value=700;
        OK(umi_decl_document_update_node(doc,&node)); expected=UMI_STATUS_INVALID_STATE;
    } else if(strcmp(mode,"orphan")==0) { OK(Add(doc,"extra","pane","missing")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"cycle")==0) { OK(Add(doc,"a","pane","b")); OK(Add(doc,"b","pane","a")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"nested_window")==0) { OK(Add(doc,"extra","window","content")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"second_root")==0) { OK(Add(doc,"extra","pane","")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"hidden_root")==0) { OK(Set(doc,"window","visible",UMI_DECL_VALUE_BOOLEAN,"false")); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"leaf_children")==0) { OK(Add(doc,"extra","label","notes")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"bad_action")==0) { OK(Set(doc,"count","action",UMI_DECL_VALUE_STRING,"shell.run")); expected=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"action_source")==0) { OK(Set(doc,"count","source",UMI_DECL_VALUE_STRING,"heading")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"action_target")==0) { OK(Set(doc,"count","target",UMI_DECL_VALUE_STRING,"missing")); expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"missing_action")==0) {
        OK(umi_decl_document_find_node(doc,"count",&node)); OK(umi_decl_node_remove_attribute(&node,"action"));
        OK(umi_decl_document_update_node(doc,&node)); expected=UMI_STATUS_NOT_IMPLEMENTED;
    } else if(strcmp(mode,"future_schema")==0) { OK(umi_decl_document_set_version(doc,(UmiDeclVersion){2,0,0})); expected=UMI_STATUS_NOT_IMPLEMENTED; }
    else if(strcmp(mode,"maximum_nodes")==0 || strcmp(mode,"too_many_nodes")==0) {
        size_t total=strcmp(mode,"maximum_nodes")==0 ? 128U : 129U;
        for(size_t i=7U;i<total;++i) { char id[32]; snprintf(id,sizeof id,"node%zu",i); OK(Add(doc,id,"label","content")); }
        if(total>128U) expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(strcmp(mode,"depth_limit")==0) {
        for(size_t i=0U;i<33U;++i) { char id[32],parent[32]; snprintf(id,sizeof id,"node%zu",i); snprintf(parent,sizeof parent,"node%zu",i==0U?0U:i-1U); OK(Add(doc,id,"pane",i==0U?"content":parent)); }
        expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(strcmp(mode,"split_tabs")==0 || strcmp(mode,"empty_tabs")==0 || strcmp(mode,"one_split_child")==0) {
        OK(Add(doc,"split","split","content")); OK(Add(doc,"tabs","tabs","split"));
        if(strcmp(mode,"one_split_child")!=0) OK(Add(doc,"right","pane","split"));
        if(strcmp(mode,"empty_tabs")!=0) OK(Add(doc,"page","label","tabs"));
        if(strcmp(mode,"split_tabs")!=0) expected=UMI_STATUS_INVALID_STATE;
    } else if(strcmp(mode,"notes_profile")!=0) return 1;
    OK(umi_decl_document_clone(doc,&before));
    CHECK(UmiDesignerNativeValidate(doc,message,sizeof message)==expected);
    CHECK(Same(doc,before));
    umi_decl_document_destroy(before); umi_decl_document_destroy(doc); return 0;
}

static int Plan(const char *mode)
{
    UmiDeclDocument *doc=NULL;
    UmiDesignerNativeProject *a=NULL,*b=NULL;
    UmiDesignerNativeProjectSummary summary;
    UmiDesignerNativeFileView file;
    OK(UmiDesignerNativeNotesDocument(&doc));
    if(strcmp(mode,"invalid_names")==0) {
        const char *names[]={"","A","1bad","a-b","a b","a\nexecute_process(COMMAND bad)","../bad"};
        for(size_t i=0U;i<sizeof names/sizeof names[0];++i)
            CHECK(UmiDesignerNativeProjectCreate(doc,names[i],&a,NULL,0U)==UMI_STATUS_INVALID_ARGUMENT && a==NULL);
        char longName[65]; memset(longName,'a',64U); longName[64]='\0';
        CHECK(UmiDesignerNativeProjectCreate(doc,longName,&a,NULL,0U)==UMI_STATUS_INVALID_ARGUMENT);
    } else {
        if(strcmp(mode,"escaped_source")==0) OK(Set(doc,"notes","text",UMI_DECL_VALUE_STRING,"A\"\\\n\t?é العربية 🙂"));
        OK(UmiDesignerNativeProjectCreate(doc,"umicom_notes",&a,NULL,0U));
        OK(UmiDesignerNativeProjectCreate(doc,"umicom_notes",&b,NULL,0U));
        OK(UmiDesignerNativeProjectGetSummary(a,&summary)); CHECK(summary.fileCount==6U && summary.nodeCount==7U);
        size_t total=0U;
        for(size_t i=0U;i<6U;++i) {
            UmiDesignerNativeFileView other;
            OK(UmiDesignerNativeProjectFile(a,i,&file)); OK(UmiDesignerNativeProjectFile(b,i,&other));
            CHECK(file.length==strlen(file.text) && strcmp(file.text,other.text)==0); total+=file.length;
        }
        CHECK(total==summary.totalBytes);
        OK(Set(doc,"notes","text",UMI_DECL_VALUE_STRING,"newer draft"));
        umi_decl_document_destroy(doc); doc=NULL;
        OK(UmiDesignerNativeProjectFile(a,1U,&file)); CHECK(strstr(file.text,"newer draft")==NULL);
        if(strcmp(mode,"escaped_source")==0) CHECK(strstr(file.text,"\\042")!=NULL && strstr(file.text,"\\012")!=NULL);
        CHECK(UmiDesignerNativeProjectFile(a,SIZE_MAX,&file)==UMI_STATUS_NOT_FOUND && file.text==NULL);
    }
    UmiDesignerNativeProjectDestroy(a); UmiDesignerNativeProjectDestroy(b); umi_decl_document_destroy(doc); return 0;
}

static int Publish(const char *mode,const char *directory)
{
    UmiDeclDocument *doc=NULL;
    UmiDesignerNativeProject *plan=NULL;
    UmiDesignerNativePublishResult report;
    OK(UmiDesignerNativeNotesDocument(&doc)); OK(UmiDesignerNativeProjectCreate(doc,"umicom_notes",&plan,NULL,0U));
    if(strcmp(mode,"invalid_paths")==0) {
        const char *paths[]={"","relative","../bad","/tmp/../bad","/tmp//bad","/tmp/.","/tmp/bad/"};
        for(size_t i=0U;i<sizeof paths/sizeof paths[0];++i) {
            CHECK(UmiDesignerNativeProjectPublish(plan,paths[i],&report)!=UMI_STATUS_OK);
            CHECK(report.directoryCreated==0 && report.complete==0);
        }
    } else {
        OK(UmiDesignerNativeProjectPublish(plan,directory,&report)); CHECK(report.complete && report.filesWritten==6U);
        size_t total=0U;
        for(size_t i=0U;i<6U;++i) {
            UmiDesignerNativeFileView file; char path[4096];
            OK(UmiDesignerNativeProjectFile(plan,i,&file));
            int n=snprintf(path,sizeof path,"%s/%s",directory,file.path); CHECK(n>0 && (size_t)n<sizeof path);
            FILE *in=fopen(path,"rb"); CHECK(in!=NULL);
            char *read=malloc(file.length+1U); CHECK(read!=NULL);
            CHECK(fread(read,1U,file.length+1U,in)==file.length && !ferror(in));
            CHECK(memcmp(read,file.text,file.length)==0); free(read); CHECK(fclose(in)==0); total+=file.length;
        }
        CHECK(report.bytesWritten==total);
        CHECK(UmiDesignerNativeProjectPublish(plan,directory,&report)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(!report.complete && !report.directoryCreated && report.filesWritten==0U);
    }
    UmiDesignerNativeProjectDestroy(plan); umi_decl_document_destroy(doc); return 0;
}

int main(int argc,char **argv)
{
    if(argc<2) return 2;
    const char *name=argv[1];
    if(strncmp(name,"remove_",7U)==0) return Removal(name);
    if(strcmp(name,"deep_cycle_removal")==0) return DeepRemoval();
    const char *nodeCases[]={"failed_property","alias_property","full_attributes","bad_attribute_count","unterminated_node","duplicate_attribute","invalid_parent","rejected_document_node"};
    for(size_t i=0;i<sizeof nodeCases/sizeof nodeCases[0];++i) if(strcmp(name,nodeCases[i])==0) return NodeChecks(name);
    if(strcmp(name,"unclosed_quote")==0 || strcmp(name,"quoted_comments")==0) return LexerChecks(name);
    if(strcmp(name,"plan_lifetime")==0 || strcmp(name,"invalid_names")==0 || strcmp(name,"escaped_source")==0) return Plan(name);
    if(strcmp(name,"invalid_paths")==0 || strcmp(name,"publish_exact")==0) return Publish(name,argc>2?argv[2]:"");
    return Profile(name);
}
