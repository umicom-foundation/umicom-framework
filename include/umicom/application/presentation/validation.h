/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/validation.h
 *
 * PURPOSE:
 *   Validate complete component, panel, workspace and window catalogue
 *   coverage before an application starts rendering its user interface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_VALIDATION_H
#define UMICOM_APPLICATION_PRESENTATION_VALIDATION_H

#include "umicom/application/presentation/projection.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_presentation_catalogues_validate(void);

#ifdef __cplusplus
}
#endif

#endif
