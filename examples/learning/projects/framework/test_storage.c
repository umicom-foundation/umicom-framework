/* Umicom Framework teaching project: framework / test_storage.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include "umicom/data/data_server.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do {if (!(condition)) {fprintf(stderr,"Line %d: %s\n",__LINE__,#condition);failed=1;goto cleanup;}} while (0)
int main(void)
{
    UmiDataServer *server=NULL;char text[128]={0};int failed=0;
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(umi_data_server_get(server,"note",text,sizeof text)==UMI_STATUS_NOT_FOUND);
    CHECK(umi_data_server_set(server,"note","Original")==UMI_STATUS_OK);
    CHECK(umi_data_server_begin(server)==UMI_STATUS_OK);
    CHECK(umi_data_server_set(server,"note","Uncommitted")==UMI_STATUS_OK);
    CHECK(umi_data_server_rollback(server)==UMI_STATUS_OK);
    CHECK(umi_data_server_get(server,"note",text,sizeof text)==UMI_STATUS_OK && strcmp(text,"Original")==0);
    CHECK(umi_data_server_begin(server)==UMI_STATUS_OK);
    CHECK(umi_data_server_set(server,"note","Committed")==UMI_STATUS_OK);
    CHECK(umi_data_server_commit(server)==UMI_STATUS_OK);
    CHECK(umi_data_server_get(server,"note",text,sizeof text)==UMI_STATUS_OK && strcmp(text,"Committed")==0);
    puts("Framework storage: read, write, rollback and commit checked.");
cleanup:
    if (server!=NULL && umi_data_server_in_transaction(server)) (void)umi_data_server_rollback(server);
    umi_data_server_destroy(server);return failed;
}
