/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/generation.h
 *
 * PURPOSE:
 *   Track immutable installed generations and their release identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A generation is the deployable unit that supports safe promotion and rollback without mutating the previous release in place.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_GENERATION_H
#define INCLUDE_UMICOM_DELIVERY_GENERATION_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the generation data shared with callers of this public contract.
 */
typedef struct UmiGeneration {
    uint64_t number;
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    char root[UMI_DELIVERY_PATH_CAPACITY];
    UmiEvidenceStatus health;
    int active;
} UmiGeneration;

/**
 * Initialise generation from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_generation_init(UmiGeneration *generation,
                              uint64_t number,
                              const char *release_id,
                              const char *root);
/**
 * Provide the generation mark active operation used by this module and its client
 * applications.
 */
void umi_generation_mark_active(UmiGeneration *generation, int active);

#ifdef __cplusplus
}
#endif

#endif
