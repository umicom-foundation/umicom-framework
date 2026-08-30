/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_serializer.c
 *
 * PURPOSE:
 *   Verify parse/serialize round trips preserve semantic nodes.
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
    const char *source="application org.umicom.demo\ncomponent root window -\nproperty root title Demo\n";
    UmiDeclDocument *document=NULL,*roundtrip=NULL;
    UmiDeclDiagnosticList diagnostics;
    char output[4096];
    assert(umi_decl_parse_text(source,&document,&diagnostics)==UMI_STATUS_OK);
    assert(umi_decl_serialize(document,output,sizeof(output),NULL)==UMI_STATUS_OK);
    assert(umi_decl_parse_text(output,&roundtrip,&diagnostics)==UMI_STATUS_OK);
    assert(umi_decl_document_node_count(roundtrip)==1U);
    umi_decl_document_destroy(roundtrip);umi_decl_document_destroy(document);
    return EXIT_SUCCESS;
}
