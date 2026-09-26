/* Umicom Framework teaching project: framework / main.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "umicom/data/data_server.h"
#include <stdio.h>
int main(void)
{
    UmiDataServer *server=NULL;
    UmiStatus status=umi_data_server_create_memory(&server);
    char text[128]={0};
    if (status==UMI_STATUS_OK) status=umi_data_server_set(server,"note","Workshop plan");
    if (status==UMI_STATUS_OK) status=umi_data_server_get(server,"note",text,sizeof text);
    if (status==UMI_STATUS_OK) printf("Framework note: %s\n",text);
    else fprintf(stderr,"Data Server status: %d\n",(int)status);
    umi_data_server_destroy(server);
    return status==UMI_STATUS_OK?0:1;
}
