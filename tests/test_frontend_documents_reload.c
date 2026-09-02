/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_documents_reload.c
 *
 * PURPOSE:
 *   Implement the test frontend documents reload behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiFrontendDocumentStore *s=calloc(1U,sizeof(*s));UmiFrontendDocument d={0},out;UmiFrontendHotReload r;uint64_t g=0U;assert(s!=NULL);(void)strcpy(d.path,"web/index.html");(void)strcpy(d.media_type,"text/html");(void)strcpy(d.language,"html");(void)strcpy(d.content,"<main>Umicom</main>");assert(umi_frontend_document_store_init(s)==UMI_STATUS_OK);assert(umi_frontend_document_store_upsert(s,&d)==UMI_STATUS_OK);assert(umi_frontend_document_store_find(s,d.path,&out)==UMI_STATUS_OK&&out.dirty);assert(umi_frontend_hot_reload_init(&r,1)==UMI_STATUS_OK);assert(umi_frontend_hot_reload_queue(&r,d.path,out.revision)==UMI_STATUS_OK);assert(umi_frontend_hot_reload_complete(&r,&g)==UMI_STATUS_OK&&g>1U);free(s);return 0;}
