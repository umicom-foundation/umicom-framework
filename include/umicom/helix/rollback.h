/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/rollback.h
 *
 * PURPOSE:
 *   Describe a requested rollback from one generation to another.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback is modeled explicitly so recovery cannot silently choose an arbitrary previous artifact.
 */

#ifndef INCLUDE_UMICOM_HELIX_ROLLBACK_H
#define INCLUDE_UMICOM_HELIX_ROLLBACK_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix rollback data shared with callers of this public contract.
 */
typedef struct UmiHelixRollback {
    char from_generation[UMI_HELIX_ID_CAPACITY];
    char to_generation[UMI_HELIX_ID_CAPACITY];
    char reason[UMI_HELIX_TEXT_CAPACITY];
    int approved;
} UmiHelixRollback;

/**
 * Initialise helix rollback from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_rollback_init(UmiHelixRollback *value);

#ifdef __cplusplus
}
#endif

#endif
