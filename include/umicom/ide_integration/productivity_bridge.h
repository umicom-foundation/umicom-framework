/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/productivity_bridge.h
 *
 * PURPOSE:
 *   Project normalized IDE locations into the existing Developer Productivity
 *   navigation history service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_PRODUCTIVITY_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_PRODUCTIVITY_BRIDGE_H
#include "umicom/developer_productivity/navigation_service.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide location to productivity operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_location_to_productivity(
    const UmiIdeLocation *location,
    UmiDeveloperProductivityLocation *out_location);

/**
 * Provide the ide productivity record visit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_productivity_record_visit(
    UmiDeveloperNavigationService *navigation,
    const UmiIdeLocation *location);

#ifdef __cplusplus
}
#endif
#endif
