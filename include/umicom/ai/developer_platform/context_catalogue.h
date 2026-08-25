/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_catalogue.h
 *
 * PURPOSE:
 *   Catalogue context sources that can contribute evidence to AI requests.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_CATALOGUE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevContextCatalogueEntry { char id[UMI_AI_DEV_ID_CAPACITY]; uint32_t priority; uint64_t flags; int enabled; } UmiAiDevContextCatalogueEntry;
typedef struct UmiAiDevContextCatalogue { UmiAiDevContextCatalogueEntry entries[UMI_AI_DEV_SMALL_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevContextCatalogue;
void umi_ai_dev_context_catalogue_init(UmiAiDevContextCatalogue *catalogue);
UmiStatus umi_ai_dev_context_catalogue_upsert(UmiAiDevContextCatalogue *catalogue, const char *id, uint32_t priority, uint64_t flags);
const UmiAiDevContextCatalogueEntry *umi_ai_dev_context_catalogue_find(const UmiAiDevContextCatalogue *catalogue, const char *id);
size_t umi_ai_dev_context_catalogue_enabled_count(const UmiAiDevContextCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
