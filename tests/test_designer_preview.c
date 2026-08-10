/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_preview.c
 *
 * PURPOSE:
 *   Verify the designer compiles and previews a semantic document through the headless renderer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiDeclComponentRegistry *registry=NULL;UmiDeclSchema schema;UmiDesignerDocument *document=NULL;UmiDeclNode root;UmiDeclDiagnosticList diagnostics;char preview[1024];
    assert(umi_designer_catalogue_create(&registry,&schema)==UMI_STATUS_OK);
    assert(umi_designer_document_create("org.umicom.demo",&document)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&root,"root","window","-")==UMI_STATUS_OK);
    assert(umi_decl_document_add_node(umi_designer_document_declarative(document),&root)==UMI_STATUS_OK);
    assert(umi_designer_preview(document,&schema,preview,sizeof(preview),&diagnostics)==UMI_STATUS_OK);
    assert(strstr(preview,"root:window")!=NULL);
    umi_designer_document_destroy(document);umi_decl_component_registry_destroy(registry);
    return EXIT_SUCCESS;
}
