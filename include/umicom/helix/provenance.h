/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/provenance.h
 *
 * PURPOSE:
 *   Track the source revision, provider and agent responsible for a Helix candidate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Provenance lets reviewers reconstruct which source, model and agent produced a candidate without relying on conversational memory.
 */

#ifndef INCLUDE_UMICOM_HELIX_PROVENANCE_H
#define INCLUDE_UMICOM_HELIX_PROVENANCE_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixProvenance {
    char source_revision[UMI_HELIX_ID_CAPACITY];
    char provider_id[UMI_HELIX_ID_CAPACITY];
    char agent_id[UMI_HELIX_ID_CAPACITY];
    char prompt_hash[UMI_HELIX_ID_CAPACITY];
} UmiHelixProvenance;

UmiStatus umi_helix_provenance_init(UmiHelixProvenance *value);

#ifdef __cplusplus
}
#endif

#endif
