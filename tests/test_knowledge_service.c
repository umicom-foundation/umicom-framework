/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_service.c
 * PURPOSE: Verify collection ingestion, incremental refresh and cited queries.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/knowledge/service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiKnowledgeServiceConfig config = umi_knowledge_service_config_default();
    UmiKnowledgeService *service = NULL;
    UmiKnowledgeCollection collection;
    UmiKnowledgeSource source;
    UmiKnowledgeIngestionReport report;
    UmiKnowledgeQuery query;
    UmiKnowledgeMatch matches[8];
    size_t count = 0U;
    config.source_capacity = 8U;
    config.vector_capacity = 32U;
    assert(umi_knowledge_service_create(&config, &service) == UMI_STATUS_OK);
    assert(umi_knowledge_collection_init(
        &collection, "framework", "Framework", "Framework manuals")
        == UMI_STATUS_OK);
    assert(umi_knowledge_service_add_collection(service, &collection)
           == UMI_STATUS_OK);
    assert(umi_knowledge_source_init(
        &source, "manual.architecture", "framework", "Architecture Manual",
        "file://architecture.md", UMI_KNOWLEDGE_SOURCE_DOCUMENT)
        == UMI_STATUS_OK);
    (void)strcpy(source.language, "en");
    assert(umi_knowledge_service_ingest_text(
        service, &source,
        "The Umicom Framework provides reusable C23 services. Native RAG "
        "keeps citations and source provenance offline.", &report)
        == UMI_STATUS_OK);
    assert(report.decision == UMI_KNOWLEDGE_REFRESH_NEW);
    assert(umi_knowledge_service_ingest_text(
        service, &source,
        "The Umicom Framework provides reusable C23 services. Native RAG "
        "keeps citations and source provenance offline.", &report)
        == UMI_STATUS_OK);
    assert(report.decision == UMI_KNOWLEDGE_REFRESH_UNCHANGED);
    assert(umi_knowledge_query_init(&query, "offline native rag citations")
           == UMI_STATUS_OK);
    (void)strcpy(query.filter.collection_id, "framework");
    assert(umi_knowledge_service_query(
        service, &query, matches, 8U, &count) == UMI_STATUS_OK);
    assert(count > 0U);
    assert(strcmp(matches[0].citation.title, "Architecture Manual") == 0);
    assert(matches[0].citation.line_start == 1U);
    umi_knowledge_service_destroy(service);
    return 0;
}
