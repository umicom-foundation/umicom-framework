/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/experiment.h
 *
 * PURPOSE:
 *   Record one controlled Helix experiment and comparison result.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Experiments compare candidates against a known baseline instead of treating every generated change as an improvement.
 */

#ifndef INCLUDE_UMICOM_HELIX_EXPERIMENT_H
#define INCLUDE_UMICOM_HELIX_EXPERIMENT_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixExperiment {
    char experiment_id[UMI_HELIX_ID_CAPACITY];
    char baseline_candidate[UMI_HELIX_ID_CAPACITY];
    char challenger_candidate[UMI_HELIX_ID_CAPACITY];
    double delta_score;
} UmiHelixExperiment;

UmiStatus umi_helix_experiment_init(UmiHelixExperiment *value);

#ifdef __cplusplus
}
#endif

#endif
