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
/**
 * Provide the ai dev model catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_catalogue_upsert(UmiAiDevModelCatalogue *catalogue, const char *id, uint32_t priority, uint64_t flags);
/**
 * Find ai dev model catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiDevModelCatalogueEntry *umi_ai_dev_model_catalogue_find(const UmiAiDevModelCatalogue *catalogue, const char *id);
/**
 * Return the number of records represented by ai dev model catalogue enabled without
 * changing their state.
 */
size_t umi_ai_dev_model_catalogue_enabled_count(const UmiAiDevModelCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
