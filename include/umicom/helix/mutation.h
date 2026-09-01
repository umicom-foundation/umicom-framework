/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/mutation.h
 *
 * PURPOSE:
 *   Describe one bounded source mutation proposed by an agent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Mutations identify the exact path and whether an operation is destructive, giving policy a precise review point.
 */

#ifndef INCLUDE_UMICOM_HELIX_MUTATION_H
#define INCLUDE_UMICOM_HELIX_MUTATION_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixMutation {
    char mutation_id[UMI_HELIX_ID_CAPACITY];
    char path[UMI_HELIX_TEXT_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    int destructive;
} UmiHelixMutation;

UmiStatus umi_helix_mutation_init(UmiHelixMutation *value);

#ifdef __cplusplus
}
#endif

#endif
