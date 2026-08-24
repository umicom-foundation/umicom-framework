/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/productivity_bridge.h
 *
 * PURPOSE:
 *   Project normalized IDE locations into the existing Developer Productivity
 *   navigation history service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_PRODUCTIVITY_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_PRODUCTIVITY_BRIDGE_H
#include "umicom/developer_productivity/navigation_service.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ide_location_to_productivity(
    const UmiIdeLocation *location,
    UmiDeveloperProductivityLocation *out_location);

UmiStatus umi_ide_productivity_record_visit(
    UmiDeveloperNavigationService *navigation,
    const UmiIdeLocation *location);

#ifdef __cplusplus
}
#endif
#endif
