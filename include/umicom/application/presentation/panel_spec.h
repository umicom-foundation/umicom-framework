/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/panel_spec.h
 *
 * PURPOSE:
 *   Validate one reusable panel presentation contract against its canonical
 *   application component definition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_PANEL_SPEC_H
#define UMICOM_APPLICATION_PRESENTATION_PANEL_SPEC_H

#include "umicom/application/presentation/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_presentation_panel_spec_validate(
    const UmiApplicationPresentationPanelSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
