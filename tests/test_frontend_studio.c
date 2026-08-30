/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_studio.c
 *
 * PURPOSE:
 *   Implement the test frontend studio behavior for
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
int main(void){UmiFrontendStudio *s=calloc(1U,sizeof(*s));UmiFrontendStudioSnapshot snap;UmiFrontendDocument d={0};uint64_t g=0U;assert(s!=NULL);(void)strcpy(d.path,"web/index.html");(void)strcpy(d.media_type,"text/html");(void)strcpy(d.content,"<!doctype html><title>Umicom</title>");assert(umi_frontend_studio_init(s,NULL)==UMI_STATUS_OK);assert(umi_frontend_dev_server_start(&s->server)==UMI_STATUS_OK);assert(umi_frontend_studio_open_document(s,&d)==UMI_STATUS_OK);assert(umi_frontend_studio_publish_changes(s,&g)==UMI_STATUS_OK);assert(umi_frontend_studio_snapshot(s,&snap)==UMI_STATUS_OK);assert(snap.device_count==3U&&snap.document_count==1U&&snap.server_running);assert(strcmp(snap.active_device,"desktop-1440")==0);free(s);return 0;}
