/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/diagnosis.h
 *
 * PURPOSE:
 *   Record a Helix diagnosis, confidence and responsible component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A diagnosis explains why Helix believes a problem exists while keeping confidence explicit for later approval policy.
 */

#ifndef INCLUDE_UMICOM_HELIX_DIAGNOSIS_H
#define INCLUDE_UMICOM_HELIX_DIAGNOSIS_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixDiagnosis {
    char diagnosis_id[UMI_HELIX_ID_CAPACITY];
    char component_id[UMI_HELIX_ID_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    double confidence;
} UmiHelixDiagnosis;

UmiStatus umi_helix_diagnosis_init(UmiHelixDiagnosis *value);

#ifdef __cplusplus
}
#endif

#endif
