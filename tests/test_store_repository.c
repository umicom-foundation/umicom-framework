/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_store_repository.c
 *
 * PURPOSE:
 *   Verify provider-neutral store calls and namespaced repository records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataServer *server = NULL;
    UmiStore store;
    UmiRepository repository;
    char value[64];
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_store_from_data_server(server, &store) == UMI_STATUS_OK);
    assert(umi_repository_init(&repository, &store, "documents") == UMI_STATUS_OK);
    assert(umi_repository_save(&repository, "1", "content") == UMI_STATUS_OK);
    assert(umi_repository_load(&repository, "1", value, sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "content") == 0);
    assert(umi_repository_remove(&repository, "1") == UMI_STATUS_OK);
    umi_data_server_destroy(server);
    return 0;
}
