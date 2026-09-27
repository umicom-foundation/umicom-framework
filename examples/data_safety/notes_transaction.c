/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Complete beginner example. Memory only; no files or device operations. */
#include "umicom/data/data_server.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiDataServer *server=NULL;
    UmiStatus status=umi_data_server_create_memory(&server);
    if (status!=UMI_STATUS_OK) return 1;
    char text[128]; size_t count=0;
    status=umi_data_server_set(server,"notes.workshop","Prepare the Notes exercise.");
    if (status==UMI_STATUS_OK) status=umi_data_server_begin(server);
    if (status==UMI_STATUS_OK) status=umi_data_server_set(server,"notes.workshop","An unsaved experiment.");
    if (status==UMI_STATUS_OK) status=umi_data_server_rollback(server);
    if (status==UMI_STATUS_OK) status=umi_data_server_get(server,"notes.workshop",text,sizeof text);
    if (status==UMI_STATUS_OK && strcmp(text,"Prepare the Notes exercise.")) status=UMI_STATUS_INVALID_STATE;
    if (status==UMI_STATUS_OK) printf("After rollback: %s\n",text);
    if (status==UMI_STATUS_OK) status=umi_data_server_begin(server);
    if (status==UMI_STATUS_OK) status=umi_data_server_set(server,"notes.workshop","Prepare and test the Notes exercise.");
    if (status==UMI_STATUS_OK) status=umi_data_server_set(server,"notes.state","reviewed");
    if (status==UMI_STATUS_OK) status=umi_data_server_commit(server);
    if (status==UMI_STATUS_OK) status=UmiDataServerCountChecked(server,&count);
    if (status==UMI_STATUS_OK) printf("After commit: %zu saved records\n",count);
    if (status!=UMI_STATUS_OK && umi_data_server_in_transaction(server))
        (void)umi_data_server_rollback(server);
    umi_data_server_destroy(server);
    if (status!=UMI_STATUS_OK) { fprintf(stderr,"The example stopped with status %d.\n",(int)status); return 1; }
    puts("Practice complete. Memory only; no files, devices or network connections were opened.");
    return 0;
}
