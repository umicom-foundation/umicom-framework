/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/rag/rag_pipeline.inc
 *
 * PURPOSE:
 *   Define the umicom.rag.pipeline reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.rag.pipeline", "rag", "Ingestion Pipeline", "umicom.ai.retrieval",
         "Chunking, metadata, embeddings, indexing and failures.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "knowledge retrieval documents search"),
