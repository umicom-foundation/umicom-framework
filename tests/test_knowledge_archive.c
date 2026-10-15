/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_archive.c
 * PURPOSE: Verify offline snapshot persistence and restoration.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "value_archive/construction_cases.h"
#include "value_archive/construction_cases.h"
#include <stdio.h>
#include "umicom/knowledge/archive.h"

/*
 * Exercise create service and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiKnowledgeService *create_service(void)
{
    UmiKnowledgeServiceConfig config = umi_knowledge_service_config_default();
    UmiKnowledgeService *service = NULL;
    config.source_capacity = 8U;
    config.vector_capacity = 32U;
    assert(umi_knowledge_service_create(&config, &service) == UMI_STATUS_OK);
    return service;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted collection value. */
static int UmiKnowledgeCollectionConstructionCases(void)
{
    UmiKnowledgeCollection *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_knowledge_collection_init(value, "manuals", "Manuals", "Offline manuals") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->collection_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_collection_init(value, oversized, "Manuals", "Offline manuals") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_collection_init(value, value->collection_id, "Manuals", "Offline manuals") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->collection_id, "manuals") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->display_name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_collection_init(value, "manuals", oversized, "Offline manuals") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_collection_init(value, "manuals", value->display_name, "Offline manuals") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->display_name, "Manuals") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}


/* A rejected replacement must preserve the previously accepted source value. */
static int UmiKnowledgeSourceConstructionCases(void)
{
    UmiKnowledgeSource *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", "manuals", "Manual One", "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->source_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, oversized, "manuals", "Manual One", "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, value->source_id, "manuals", "Manual One", "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_id, "manual.one") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->collection_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", oversized, "Manual One", "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", value->collection_id, "Manual One", "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->collection_id, "manuals") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", "manuals", oversized, "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", "manuals", value->title, "file://manual-one", UMI_KNOWLEDGE_SOURCE_DOCUMENT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Manual One") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->uri) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", "manuals", "Manual One", oversized, UMI_KNOWLEDGE_SOURCE_DOCUMENT) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_knowledge_source_init(value, "manual.one", "manuals", "Manual One", value->uri, UMI_KNOWLEDGE_SOURCE_DOCUMENT) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->uri, "file://manual-one") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiKnowledgeSourceConstructionCases() != 0) return 1;

    if (UmiKnowledgeCollectionConstructionCases() != 0) return 1;

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
