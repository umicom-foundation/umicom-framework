/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/diagnosis.c
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

/* BEGINNER NOTE:
 * A diagnosis explains why Helix believes a problem exists while keeping confidence explicit for later approval policy.
 */

#include "umicom/helix/diagnosis.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_diagnosis_init(UmiHelixDiagnosis *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
