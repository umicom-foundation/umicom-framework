/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/navigation_service.h
 *
 * PURPOSE:
 *   Compose history, bookmarks, symbols and references into one reusable
 *   navigation service for editor/tool windows and AI workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_NAVIGATION_SERVICE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_NAVIGATION_SERVICE_H

#include "umicom/developer_productivity/bookmarks.h"
#include "umicom/developer_productivity/navigation_history.h"
#include "umicom/developer_productivity/references.h"
#include "umicom/developer_productivity/symbols.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer navigation service data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperNavigationService {
    UmiDeveloperNavigationHistory history;
    UmiDeveloperBookmarkStore bookmarks;
    UmiDeveloperReferenceSet references;
    UmiDeveloperSymbolIndex *symbols;
    uint64_t revision;
} UmiDeveloperNavigationService;

/**
 * Initialise developer navigation service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_navigation_service_create(
    UmiDeveloperNavigationService **out_service);

/**
 * Release or reset state held by developer navigation service so the same storage can be
 * reused safely.
 */
void umi_developer_navigation_service_destroy(
    UmiDeveloperNavigationService *service);

/**
 * Provide the developer navigation service visit operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_service_visit(
    UmiDeveloperNavigationService *service,
    const UmiDeveloperProductivityLocation *location);

#ifdef __cplusplus
}
#endif

#endif
