/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/document_bridge.h
 *
 * PURPOSE:
 *   Open a normalized IDE location through the authoritative DocumentCoordinator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_DOCUMENT_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_DOCUMENT_BRIDGE_H
#include "umicom/document/coordinator.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ide document open location operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_document_open_location(
    UmiDocumentCoordinator *documents,
    const UmiIdeLocation *location,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

#ifdef __cplusplus
}
#endif
#endif
