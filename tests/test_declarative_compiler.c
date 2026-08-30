/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_compiler.c
 *
 * PURPOSE:
 *   Verify validated documents compile into immutable application plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    UmiDeclComponentRegistry *registry=NULL;UmiDeclSchema schema;UmiDeclDocument *document=NULL;UmiDeclNode root;UmiDeclApplicationPlan plan;UmiDeclDiagnosticList diagnostics;
    assert(umi_designer_catalogue_create(&registry,&schema)==UMI_STATUS_OK);
    assert(umi_decl_document_create("org.umicom.demo",&document)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&root,"root","window","-")==UMI_STATUS_OK);
    assert(umi_decl_document_add_node(document,&root)==UMI_STATUS_OK);
    assert(umi_decl_compile(document,&schema,&plan,&diagnostics)==UMI_STATUS_OK);
    assert(plan.component_count==1U);
    umi_decl_plan_dispose(&plan);
    umi_decl_document_destroy(document);umi_decl_component_registry_destroy(registry);
    return EXIT_SUCCESS;
}
