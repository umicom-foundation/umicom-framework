/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/healthcare.h
 * PURPOSE: Expose the synthetic Healthcare experience to shared launch selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_HEALTHCARE_H
#define UMICOM_APPLICATION_EXPERIENCES_HEALTHCARE_H
#include "umicom/application/experience.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Borrow the immutable Healthcare experience. Foundation status describes the
 * synthetic host and is not a claim of clinical or release qualification. */
const UmiApplicationExperienceDefinition *umi_application_experience_healthcare(void);
#ifdef __cplusplus
}
#endif
#endif
