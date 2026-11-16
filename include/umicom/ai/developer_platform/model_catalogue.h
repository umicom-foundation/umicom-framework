/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_catalogue.h
 *
 * PURPOSE:
 *   Maintain the bounded catalogue of AI model endpoints available to developer workflows.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_CATALOGUE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model catalogue entry data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevModelCatalogueEntry { char id[UMI_AI_DEV_ID_CAPACITY]; uint32_t priority; uint64_t flags; int enabled; } UmiAiDevModelCatalogueEntry;
/**
 * Represent the ai dev model catalogue data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelCatalogue { UmiAiDevModelCatalogueEntry entries[UMI_AI_DEV_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevModelCatalogue;
/**
 * Initialise ai dev model catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_catalogue_init(UmiAiDevModelCatalogue *catalogue);
/*
 * Validate the bounded public state before reading it. Empty initialized state
 * is valid. Stored IDs must be nonempty, terminated and unique; revision must
 * be at least count. Corrupt state returns INVALID_STATE, NULL INVALID_ARGUMENT.
 * Unknown flag bits, case-sensitive IDs and nonzero enabled values are retained.
 * Serialize all access. A revision is a stale-result token, not a thread lock.
 */
UmiStatus UmiAiDevModelCatalogueValidate(const UmiAiDevModelCatalogue *catalogue);
/*
 * Replace or insert only at the reviewed catalogue revision. An ID must contain
 * 1..UMI_AI_DEV_ID_CAPACITY-1 bytes before its NUL; IDs are never shortened.
 * BUSY reports a stale revision; exhausted capacity/revision returns
 * CAPACITY_EXCEEDED. Every refusal leaves the complete catalogue unchanged.
 * IDs may alias stored IDs: input is captured before mutation. This operation
 * changes metadata only; it neither resolves credentials nor contacts a model.
 */
UmiStatus UmiAiDevModelCatalogueUpsertIfRevision(
    UmiAiDevModelCatalogue *catalogue, const char *id,
    uint32_t priority, uint64_t flags, uint64_t expected_revision);

/**
 * Provide the ai dev model catalogue upsert operation used by this module and its client
 * applications.
 */
/* Compatibility entry uses the current revision after validating state. */
UmiStatus umi_ai_dev_model_catalogue_upsert(UmiAiDevModelCatalogue *catalogue, const char *id, uint32_t priority, uint64_t flags);
/**
 * Find ai dev model catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
/* NULL on invalid/missing input or corrupt state. Borrow until mutation. */
const UmiAiDevModelCatalogueEntry *umi_ai_dev_model_catalogue_find(const UmiAiDevModelCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by ai dev model catalogue enabled without
 * changing their state.
 */
/* Zero on invalid state; Validate distinguishes that from an empty catalogue. */
size_t umi_ai_dev_model_catalogue_enabled_count(const UmiAiDevModelCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
