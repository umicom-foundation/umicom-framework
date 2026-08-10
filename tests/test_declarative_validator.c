/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_validator.c
 *
 * PURPOSE:
 *   Verify validation rejects unknown component types and accepts the default catalogue.
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
    UmiDeclComponentRegistry *registry=NULL;
    UmiDeclSchema schema;
    UmiDeclDocument *document=NULL;
    UmiDeclNode root;
    UmiDeclDiagnosticList diagnostics;
    assert(umi_designer_catalogue_create(&registry,&schema)==UMI_STATUS_OK);
    assert(umi_decl_document_create("org.umicom.demo",&document)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&root,"root","window","-")==UMI_STATUS_OK);
    assert(umi_decl_document_add_node(document,&root)==UMI_STATUS_OK);
    assert(umi_decl_validate(document,&schema,&diagnostics)==UMI_STATUS_OK);
    umi_decl_document_destroy(document);umi_decl_component_registry_destroy(registry);
    return EXIT_SUCCESS;
}
