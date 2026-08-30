/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_parser.c
 *
 * PURPOSE:
 *   Verify the line-oriented .umiapp parser produces a semantic application tree.
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
    const char *source="application org.umicom.demo\nversion 1.0.0\ncomponent root window -\nproperty root title Umicom Demo\ncomponent editor editor root\nproperty editor visible true\n";
    UmiDeclDocument *document=NULL;
    UmiDeclDiagnosticList diagnostics;
    UmiDeclDocumentSnapshot snapshot;
    assert(umi_decl_parse_text(source,&document,&diagnostics)==UMI_STATUS_OK);
    assert(diagnostics.count==0U);
    assert(umi_decl_document_snapshot(document,&snapshot)==UMI_STATUS_OK);
    assert(snapshot.node_count==2U);
    umi_decl_document_destroy(document);
    return EXIT_SUCCESS;
}
