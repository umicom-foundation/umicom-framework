/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_metadata_lineage.c
 *
 * PURPOSE:
 *   Verify metadata persistence and append-only lineage records.
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
    UmiRepository metadata_repository;
    UmiMetadata metadata = {"doc-1", "document", 3U, "Studio", 10U, 20U};
    UmiMetadata loaded;
    UmiLineageStore lineage;
    UmiLineageRecord record = {0U, "doc-1", "save", "Studio", "Sammy", 7U, 30U};
    UmiLineageRecord loaded_lineage;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_store_from_data_server(server, &store) == UMI_STATUS_OK);
    assert(umi_repository_init(&metadata_repository, &store, "metadata") == UMI_STATUS_OK);
    assert(umi_metadata_save(&metadata_repository, &metadata) == UMI_STATUS_OK);
    assert(umi_metadata_load(&metadata_repository, "doc-1", &loaded) == UMI_STATUS_OK);
    assert(loaded.version == 3U && strcmp(loaded.source, "Studio") == 0);
    assert(umi_lineage_store_init(&lineage, &store) == UMI_STATUS_OK);
    assert(umi_lineage_append(&lineage, &record) == UMI_STATUS_OK);
    assert(umi_lineage_load(&lineage, 1U, &loaded_lineage) == UMI_STATUS_OK);
    assert(strcmp(loaded_lineage.operation, "save") == 0);
    umi_data_server_destroy(server);
    return 0;
}
