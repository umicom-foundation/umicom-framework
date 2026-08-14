/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_archive.c
 * PURPOSE: Verify offline snapshot persistence and restoration.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include "umicom/knowledge/archive.h"

static UmiKnowledgeService *create_service(void)
{
    UmiKnowledgeServiceConfig config = umi_knowledge_service_config_default();
    UmiKnowledgeService *service = NULL;
    config.source_capacity = 8U;
    config.vector_capacity = 32U;
    assert(umi_knowledge_service_create(&config, &service) == UMI_STATUS_OK);
    return service;
}

int main(void)
{
    const char *path = "umicom-knowledge-test.bin";
    UmiKnowledgeService *source_service = create_service();
    UmiKnowledgeService *restored_service = create_service();
    UmiKnowledgeCollection collection;
    UmiKnowledgeSource source;
    UmiKnowledgeIngestionReport report;
    UmiKnowledgeServiceSnapshot snapshot;
    assert(umi_knowledge_collection_init(
        &collection, "manuals", "Manuals", "Offline manuals")
        == UMI_STATUS_OK);
    assert(umi_knowledge_service_add_collection(source_service, &collection)
           == UMI_STATUS_OK);
    assert(umi_knowledge_source_init(
        &source, "manual.one", "manuals", "Manual One",
        "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT)
        == UMI_STATUS_OK);
    assert(umi_knowledge_service_ingest_text(
        source_service, &source, "Offline-first knowledge archive.", &report)
        == UMI_STATUS_OK);
    assert(umi_knowledge_archive_save(source_service, path) == UMI_STATUS_OK);
    assert(umi_knowledge_archive_load(restored_service, path) == UMI_STATUS_OK);
    assert(umi_knowledge_service_snapshot(restored_service, &snapshot)
           == UMI_STATUS_OK);
    assert(snapshot.collection_count == 1U);
    assert(snapshot.source_count == 1U);
    assert(snapshot.chunk_count == 1U);
    umi_knowledge_service_destroy(restored_service);
    umi_knowledge_service_destroy(source_service);
    assert(remove(path) == 0);
    return 0;
}
