/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/conditional_restore/main.c
 *
 * PURPOSE:
 *   Demonstrate caller-owned scratch and stale-review protection using a web configuration without starting a server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/server_config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    UmiWebServerConfig expected=umi_web_server_config_default();
    UmiWebServerConfig current=expected, incoming=expected, workspace;
    size_t input_size=0U, snapshot_size=0U;
    unsigned char *archive=NULL,*comparison=NULL;
    int result=1;
    incoming.port=8123U;
    if(umi_web_server_config_archive_encode(&incoming,NULL,0U,&input_size)!=UMI_STATUS_OK ||
       umi_web_server_config_archive_encode(&expected,NULL,0U,&snapshot_size)!=UMI_STATUS_OK ||
       snapshot_size>SIZE_MAX/2U)goto done;
    archive=malloc(input_size);comparison=malloc(snapshot_size*2U);
    if(!archive||!comparison)goto done;
    size_t written=0U;
    if(umi_web_server_config_archive_encode(&incoming,archive,input_size,&written)!=UMI_STATUS_OK)goto done;
    UmiStatus status=umi_web_server_config_archive_decode_if_unchanged_with_workspace(
        archive,written,&expected,&workspace,comparison,snapshot_size*2U,&current);
    if(status!=UMI_STATUS_OK||current.port!=8123U)goto done;
    /* Reusing a now-stale review must not replace current a second time. */
    status=umi_web_server_config_archive_decode_if_unchanged_with_workspace(
        archive,written,&expected,&workspace,comparison,snapshot_size*2U,&current);
    if(status!=UMI_STATUS_INVALID_STATE||current.port!=8123U)goto done;
    puts("Restored port 8123; stale review refused. No listener was opened.");
    result=0;
done:
    free(comparison);free(archive);
    if(fflush(stdout)!=0||ferror(stdout))result=1;
    return result;
}
