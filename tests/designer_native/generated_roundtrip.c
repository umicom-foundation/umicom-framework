/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_native/generated_roundtrip.c
 * PURPOSE:
 *   Used twice: build the source fixture, then inspect the compiled constructor.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Used twice: build the source fixture, then inspect the compiled constructor. */
#include "umicom/designer/native_project.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while(0)
static const char CAPTION[] = "A\"B\\C\n\t?é العربية 🙂";
#ifdef BUILD_FIXTURE
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    UmiDeclDocument *doc=NULL; UmiDeclNode node;
    UmiDesignerNativeProject *plan=NULL; UmiDesignerNativePublishResult published;
    CHECK(UmiDesignerNativeNotesDocument(&doc)==UMI_STATUS_OK);
    CHECK(umi_decl_document_find_node(doc,"notes",&node)==UMI_STATUS_OK);
    CHECK(umi_decl_node_set_attribute(&node,"text",UMI_DECL_VALUE_STRING,CAPTION)==UMI_STATUS_OK);
    CHECK(umi_decl_document_update_node(doc,&node)==UMI_STATUS_OK);
    CHECK(umi_decl_document_find_node(doc,"title",&node)==UMI_STATUS_OK);
    CHECK(umi_decl_node_set_attribute(&node,"text",UMI_DECL_VALUE_STRING,"123")==UMI_STATUS_OK);
    CHECK(umi_decl_document_update_node(doc,&node)==UMI_STATUS_OK);
    CHECK(UmiDesignerNativeProjectCreate(doc,"umicom_roundtrip",&plan,NULL,0U)==UMI_STATUS_OK);
    CHECK(UmiDesignerNativeProjectPublish(plan,argv[1],&published)==UMI_STATUS_OK);
    UmiDesignerNativeProjectDestroy(plan); umi_decl_document_destroy(doc); return 0;
}
#else
#include "application.h"
int main(void)
{
    UmiDeclDocument *doc=NULL; UmiDeclNode node; UmiDeclAttribute value;
    CHECK(UmiGeneratedApplicationCreate(&doc)==UMI_STATUS_OK);
    CHECK(UmiDesignerNativeValidate(doc,NULL,0U)==UMI_STATUS_OK);
    CHECK(umi_decl_document_node_count(doc)==7U);
    CHECK(umi_decl_document_find_node(doc,"notes",&node)==UMI_STATUS_OK);
    CHECK(umi_decl_node_get_attribute(&node,"text",&value)==UMI_STATUS_OK);
    CHECK(value.value.kind==UMI_DECL_VALUE_STRING && strcmp(value.value.text,CAPTION)==0);
    CHECK(umi_decl_document_find_node(doc,"title",&node)==UMI_STATUS_OK);
    CHECK(umi_decl_node_get_attribute(&node,"text",&value)==UMI_STATUS_OK);
    CHECK(value.value.kind==UMI_DECL_VALUE_STRING && strcmp(value.value.text,"123")==0);
    umi_decl_document_destroy(doc); puts("Generated constructor retained exact UTF-8 bytes and numeric-looking string type."); return 0;
}
#endif
