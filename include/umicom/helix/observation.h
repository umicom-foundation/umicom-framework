/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/observation.h
 *
 * PURPOSE:
 *   Describe an observed product or runtime condition before diagnosis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Observations are facts, not proposed fixes; keeping them separate prevents diagnostic guesses from overwriting raw evidence.
 */

#ifndef INCLUDE_UMICOM_HELIX_OBSERVATION_H
#define INCLUDE_UMICOM_HELIX_OBSERVATION_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix observation data shared with callers of this public contract.
 */
typedef struct UmiHelixObservation {
    char observation_id[UMI_HELIX_ID_CAPACITY];
    char component_id[UMI_HELIX_ID_CAPACITY];
    char message[UMI_HELIX_TEXT_CAPACITY];
    int severity;
} UmiHelixObservation;

/**
 * Initialise helix observation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_observation_init(UmiHelixObservation *value);

#ifdef __cplusplus
}
#endif

#endif
