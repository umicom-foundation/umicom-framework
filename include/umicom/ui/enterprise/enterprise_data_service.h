/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/enterprise_data_service.h
 *
 * PURPOSE:
 *   Aggregate virtual grid, tree and inspector readiness for Framework consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ENTERPRISE_DATA_SERVICE_H
#define UMICOM_UI_ENTERPRISE_ENTERPRISE_DATA_SERVICE_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent enterprise data service data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntEnterpriseDataService { size_t active_grids; size_t active_trees; size_t active_inspectors; size_t materialised_rows; size_t cached_cells; uint64_t revision; } UmiUiEntEnterpriseDataService;
/**
 * Initialise ui ent enterprise data service from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_ent_enterprise_data_service_init(UmiUiEntEnterpriseDataService *s);
/**
 * Provide the ui ent enterprise data service update operation used by this module and its
 * client applications.
 */
void umi_ui_ent_enterprise_data_service_update(UmiUiEntEnterpriseDataService *s,size_t grids,size_t trees,size_t inspectors,size_t rows,size_t cells);
/**
 * Provide the ui ent enterprise data service ready operation used by this module and its
 * client applications.
 */
int umi_ui_ent_enterprise_data_service_ready(const UmiUiEntEnterpriseDataService *s);

#endif
