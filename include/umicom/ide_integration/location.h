/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/location.h
 *
 * PURPOSE:
 *   Normalize path/URI/line information from independent Framework services into
 *   one cross-navigation location contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_LOCATION_H
#define UMICOM_IDE_INTEGRATION_LOCATION_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise ide location from caller-provided values so later operations receive a known
 * state.
 */
void umi_ide_location_init(UmiIdeLocation *location);

/**
 * Provide the ide location set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_location_set_path(
    UmiIdeLocation *location,
    const char *path,
    uint32_t line,
    uint32_t column);

/**
 * Provide the ide location set uri operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_location_set_uri(
    UmiIdeLocation *location,
    const char *uri,
    uint32_t line,
    uint32_t column);

/**
 * Provide the ide location has target operation used by this module and its client
 * applications.
 */
int umi_ide_location_has_target(const UmiIdeLocation *location);

#ifdef __cplusplus
}
#endif
#endif
