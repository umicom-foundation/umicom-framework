/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/evidence.h
 *
 * PURPOSE:
 *   Record one immutable piece of evidence used by a Helix decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Evidence is recorded before planning so candidate changes can always be traced back to the observations that justified them.
 */

#ifndef INCLUDE_UMICOM_HELIX_EVIDENCE_H
#define INCLUDE_UMICOM_HELIX_EVIDENCE_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixEvidence {
    char evidence_id[UMI_HELIX_ID_CAPACITY];
    char kind[UMI_HELIX_ID_CAPACITY];
    char source[UMI_HELIX_TEXT_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    uint64_t observed_at_ns;
} UmiHelixEvidence;

UmiStatus umi_helix_evidence_init(UmiHelixEvidence *value);

#ifdef __cplusplus
}
#endif

#endif
