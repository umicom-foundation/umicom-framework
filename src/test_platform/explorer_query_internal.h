/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/explorer_query_internal.h
 * PURPOSE: Stage owned Explorer projections for owner-thread composition without exposing a partial query.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EXPLORER_QUERY_INTERNAL_H
#define UMICOM_EXPLORER_QUERY_INTERNAL_H
#include "umicom/test_platform/explorer_model.h"
#include "umicom/test_platform/workspace.h"
/* Borrowed owner identity used to reject composition across unrelated catalogues. */
const UmiTestPlatformService *UmiExplorerWorkspaceService(const UmiTestWorkspace *workspace);
/* Private composition contract: the prepared model owns independent storage.
 * Serialize preparation and publication with registry/workspace access. No
 * callbacks, I/O or allocations may be inserted after the workspace commits. */
UmiStatus UmiExplorerPrepareQuery(const UmiTestExplorerModel *model,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter, UmiTestExplorerSortMode sort,
    UmiTestExplorerGroupMode group, UmiTestExplorerModel **out_prepared);
/* Consumes prepared. Both arguments must be distinct, valid models; no failure
 * remains after preparation. The address borrowed by frontends stays stable. */
void UmiExplorerPublishQuery(UmiTestExplorerModel *model,
    UmiTestExplorerModel *prepared);
#endif
