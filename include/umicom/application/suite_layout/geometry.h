/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/geometry.h
 *
 * PURPOSE:
 *   Map canonical UI placement values to deterministic normalized workspace geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GEOMETRY_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GEOMETRY_H

#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSuiteLayoutRect {
    double x;
    double y;
    double width;
    double height;
} UmiApplicationSuiteLayoutRect;

UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement);
int umi_application_suite_layout_region_supported(UmiUiPlacement placement);

#ifdef __cplusplus
}
#endif
#endif
