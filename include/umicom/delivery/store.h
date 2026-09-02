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

/*
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

/**
 * Represent the generation store data shared with callers of this public contract.
 */
typedef struct UmiGenerationStore {
    UmiGeneration generations[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiGenerationStore;
/**
 * Initialise generation store from caller-provided values so later operations receive a
 * known state.
 */
void umi_generation_store_init(UmiGenerationStore *store);
/**
 * Add generation store only after its inputs and available capacity have been checked.
 */
UmiStatus umi_generation_store_add(UmiGenerationStore *store,
                                   const UmiGeneration *generation);
/**
 * Provide the generation store active operation used by this module and its client
 * applications.
 */
const UmiGeneration *umi_generation_store_active(const UmiGenerationStore *store);

#ifdef __cplusplus
}
#endif

#endif
