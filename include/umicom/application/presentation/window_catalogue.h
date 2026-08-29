/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/window_catalogue.h
 *
 * PURPOSE:
 *   Expose portable application windows for every Framework workspace recipe
 *   and audience profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_WINDOW_CATALOGUE_H
#define UMICOM_APPLICATION_PRESENTATION_WINDOW_CATALOGUE_H

#include "umicom/application/presentation/window_spec.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_presentation_window_catalogue_count(void);
const UmiApplicationPresentationWindowSpec *
umi_application_presentation_window_catalogue_at(size_t index);
const UmiApplicationPresentationWindowSpec *
umi_application_presentation_window_catalogue_find(const char *recipe_id);
size_t umi_application_presentation_window_catalogue_application_count(
    const char *application_id);
const UmiApplicationPresentationWindowSpec *
umi_application_presentation_window_catalogue_application_at(
    const char *application_id,
    size_t application_index);

#ifdef __cplusplus
}
#endif

#endif
