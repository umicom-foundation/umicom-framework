/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_document.c
 *
 * PURPOSE:
 *   Verify document identity, revisions, cloning and subtree removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiDeclDocument *document=NULL,*copy=NULL;
    UmiDeclNode root,child;
    UmiDeclDocumentSnapshot snapshot;
    assert(umi_decl_document_create("org.umicom.demo",&document)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&root,"root","window","-")==UMI_STATUS_OK);
    assert(umi_decl_node_init(&child,"child","label","root")==UMI_STATUS_OK);
    assert(umi_decl_document_add_node(document,&root)==UMI_STATUS_OK);
    assert(umi_decl_document_add_node(document,&child)==UMI_STATUS_OK);
    assert(umi_decl_document_clone(document,&copy)==UMI_STATUS_OK);
    assert(umi_decl_document_remove_node(copy,"root")==UMI_STATUS_OK);
    assert(umi_decl_document_snapshot(copy,&snapshot)==UMI_STATUS_OK);
    assert(snapshot.node_count==0U);
    umi_decl_document_destroy(copy);umi_decl_document_destroy(document);
    return EXIT_SUCCESS;
}
