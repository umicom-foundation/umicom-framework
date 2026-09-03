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

/**
 * Check that application presentation window spec satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_presentation_window_spec_validate(
    const UmiApplicationPresentationWindowSpec *spec);

/**
 * Fit a governed window size inside the available desktop area. The function
 * never returns dimensions below the window's readable minimum and reports
 * whether the frontend should apply compact panel policies.
 */
UmiStatus umi_application_presentation_window_spec_fit(
    const UmiApplicationPresentationWindowSpec *spec,
    uint32_t available_width,
    uint32_t available_height,
    uint32_t *out_width,
    uint32_t *out_height,
    int *out_compact);

#ifdef __cplusplus
}
#endif

#endif
