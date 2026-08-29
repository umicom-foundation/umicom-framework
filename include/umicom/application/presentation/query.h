/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/query.h
 *
 * PURPOSE:
 *   Query projected presentation plans by region without exposing frontend
 *   widget details or duplicating traversal code in applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_QUERY_H
#define UMICOM_APPLICATION_PRESENTATION_QUERY_H

#include "umicom/application/presentation/projection.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_presentation_plan_region_count(
    const UmiApplicationPresentationPlan *plan,
    UmiApplicationComponentRegion region);
const UmiApplicationPresentationPanelPlacement *
umi_application_presentation_plan_region_at(
    const UmiApplicationPresentationPlan *plan,
    UmiApplicationComponentRegion region,
    size_t region_index);

#ifdef __cplusplus
}
#endif

#endif
