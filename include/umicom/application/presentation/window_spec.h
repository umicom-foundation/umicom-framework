/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/window_spec.h
 *
 * PURPOSE:
 *   Validate one portable application window specification and its workspace
 *   recipe relationship.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_WINDOW_SPEC_H
#define UMICOM_APPLICATION_PRESENTATION_WINDOW_SPEC_H

#include "umicom/application/presentation/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_presentation_window_spec_validate(
    const UmiApplicationPresentationWindowSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
