/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/ai.h
 *
 * PURPOSE:
 *   Aggregate the public Umicom AI platform contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Applications may include this umbrella header, while smaller modules can continue including only the specific contracts they need.
 */

#ifndef INCLUDE_UMICOM_AI_AI_H
#define INCLUDE_UMICOM_AI_AI_H

#include "umicom/ai/types.h"
#include "umicom/ai/model.h"
#include "umicom/ai/message.h"
#include "umicom/ai/request.h"
#include "umicom/ai/response.h"
#include "umicom/ai/stream.h"
#include "umicom/ai/provider.h"
#include "umicom/ai/provider_registry.h"
#include "umicom/ai/prompt.h"
#include "umicom/ai/context.h"
#include "umicom/ai/token_budget.h"
#include "umicom/ai/embedding.h"
#include "umicom/ai/document.h"
#include "umicom/ai/chunk.h"
#include "umicom/ai/chunker.h"
#include "umicom/ai/vector_store.h"
#include "umicom/ai/retrieval.h"
#include "umicom/ai/source.h"
#include "umicom/ai/citation.h"
#include "umicom/ai/tool.h"
#include "umicom/ai/tool_registry.h"
#include "umicom/ai/approval.h"
#include "umicom/ai/policy.h"
#include "umicom/ai/conversation.h"
#include "umicom/ai/health.h"
#include "umicom/ai/evaluation.h"
#include "umicom/ai/dataset.h"
#include "umicom/ai/session.h"
#include "umicom/ai/authorengine.h"
#include "umicom/ai/runtime_catalogue.h"
#include "umicom/ai/privacy_policy.h"
#include "umicom/ai/context_broker.h"
#include "umicom/ai/conversation_archive.h"
#include "umicom/ai/authorengine_service.h"
#include "umicom/ai/coding_types.h"
#include "umicom/ai/coding_context.h"
#include "umicom/ai/coding_patch.h"
#include "umicom/ai/coding_assistant.h"
#include "umicom/ai/local_runtime.h"
#include "umicom/ai/remote_provider.h"
#include "umicom/ai/rag.h"
#include "umicom/ai/runtime.h"
#include "umicom/ai/reranker.h"
#include "umicom/ai/hybrid_retrieval.h"
#include "umicom/ai/mcp/mcp.h"
#include "umicom/ai/helix/ai_helix.h"

#endif
