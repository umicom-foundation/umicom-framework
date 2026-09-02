/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_lsp_extended.c
 *
 * PURPOSE:
 *   Implement the test lsp extended behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework extended LSP test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/protocol/lsp_extended.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiProtocolTransport*t=NULL;UmiProtocolClient*c=NULL;UmiLspClient lsp;UmiLspPosition p={2U,4U};UmiLspRange r={{1U,0U},{3U,8U}};int64_t id=0;assert(umi_protocol_transport_create_memory(64U,&t)==UMI_STATUS_OK);assert(umi_protocol_client_create(t,&c)==UMI_STATUS_OK);assert(umi_protocol_client_start(c)==UMI_STATUS_OK);assert(umi_lsp_client_init(&lsp,c,"file:///workspace")==UMI_STATUS_OK);assert(umi_lsp_hover(&lsp,"file:///main.c",p,&id)==UMI_STATUS_OK&&id>0);assert(umi_lsp_definition(&lsp,"file:///main.c",p,&id)==UMI_STATUS_OK);assert(umi_lsp_references(&lsp,"file:///main.c",p,1,&id)==UMI_STATUS_OK);assert(umi_lsp_rename(&lsp,"file:///main.c",p,"renamed",&id)==UMI_STATUS_OK);assert(umi_lsp_code_action(&lsp,"file:///main.c",r,&id)==UMI_STATUS_OK);assert(umi_lsp_formatting(&lsp,"file:///main.c",4U,1,&id)==UMI_STATUS_OK);assert(umi_lsp_semantic_tokens_full(&lsp,"file:///main.c",&id)==UMI_STATUS_OK);assert(umi_lsp_did_save(&lsp,"file:///main.c","int main(void){}") == UMI_STATUS_OK);assert(umi_lsp_did_close(&lsp,"file:///main.c")==UMI_STATUS_OK);assert(umi_protocol_transport_count(t)==9U);umi_protocol_client_destroy(c);umi_protocol_transport_destroy(t);return 0;}
