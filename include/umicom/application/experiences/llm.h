/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/llm.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom LLM.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_LLM_H
#define UMICOM_APPLICATION_EXPERIENCES_LLM_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiApplicationExperienceDefinition *umi_application_experience_llm(void);

#ifdef __cplusplus
}
#endif

#endif
