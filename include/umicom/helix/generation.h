/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/generation.h
 *
 * PURPOSE:
 *   Describe an immutable promoted software generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A generation identifies the exact artifact and checksum that was promoted, giving operations a stable rollback target.
 */

#ifndef INCLUDE_UMICOM_HELIX_GENERATION_H
#define INCLUDE_UMICOM_HELIX_GENERATION_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix generation data shared with callers of this public contract.
 */
typedef struct UmiHelixGeneration {
    char generation_id[UMI_HELIX_ID_CAPACITY];
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char artifact[UMI_HELIX_TEXT_CAPACITY];
    char checksum[UMI_HELIX_ID_CAPACITY];
    uint64_t promoted_at_ns;
} UmiHelixGeneration;

/**
 * Initialise helix generation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_generation_init(UmiHelixGeneration *value);

#ifdef __cplusplus
}
#endif

#endif
