/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/store.h
 *
 * PURPOSE:
 *   Keep an in-memory bounded store of generations for deterministic tests and simple local release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The store is a reference implementation that can later be replaced by the Data Server without changing callers.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_STORE_H
#define INCLUDE_UMICOM_DELIVERY_STORE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/generation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGenerationStore {
    UmiGeneration generations[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiGenerationStore;
void umi_generation_store_init(UmiGenerationStore *store);
UmiStatus umi_generation_store_add(UmiGenerationStore *store,
                                   const UmiGeneration *generation);
const UmiGeneration *umi_generation_store_active(const UmiGenerationStore *store);

#ifdef __cplusplus
}
#endif

#endif
