/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_editor_platform.c
 *
 * PURPOSE:
 *   Exercise the reusable editor session, diagnostics, completions and diff models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#include <string.h>
#include "umicom/editor/editor.h"
int main(void){UmiEditorSession*s=NULL;UmiEditorDocumentSnapshot d={0};UmiEditorDiagnosticSnapshot x={0};UmiEditorCompletionSnapshot c={0};UmiEditorSessionSnapshot snap;if(umi_editor_session_create(&s)!=UMI_STATUS_OK)return 1;strcpy(d.id,"doc");strcpy(d.uri,"file:///main.c");strcpy(d.language_id,"c");d.line_count=10U;if(umi_editor_document_registry_upsert(umi_editor_session_document(s),&d)!=UMI_STATUS_OK)return 2;strcpy(x.id,"diag");strcpy(x.document_id,"doc");strcpy(x.message,"example");if(umi_editor_diagnostic_registry_upsert(umi_editor_session_diagnostic(s),&x)!=UMI_STATUS_OK)return 3;strcpy(c.id,"printf");strcpy(c.document_id,"doc");strcpy(c.label,"printf");if(umi_editor_completion_registry_upsert(umi_editor_session_completion(s),&c)!=UMI_STATUS_OK)return 4;if(umi_editor_session_snapshot(s,&snap)!=UMI_STATUS_OK||snap.document_count!=1U||snap.diagnostic_count!=1U||snap.completion_count!=1U)return 5;umi_editor_session_destroy(s);return 0;}
