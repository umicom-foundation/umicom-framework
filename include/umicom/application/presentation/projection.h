/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/projection.h
 *
 * PURPOSE:
 *   Project a named workspace recipe into one complete, ordered panel and
 *   window plan that any supported frontend can render.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_PROJECTION_H
#define UMICOM_APPLICATION_PRESENTATION_PROJECTION_H

#include "umicom/application/presentation/window_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application presentation project operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_presentation_project(
    const char *recipe_id,
    UmiApplicationPresentationPlan *out_plan);

#ifdef __cplusplus
}
#endif

#endif
