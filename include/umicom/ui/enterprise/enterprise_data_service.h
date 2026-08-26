/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/enterprise_data_service.h
 *
 * PURPOSE:
 *   Aggregate virtual grid, tree and inspector readiness for Framework consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ENTERPRISE_DATA_SERVICE_H
#define UMICOM_UI_ENTERPRISE_ENTERPRISE_DATA_SERVICE_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntEnterpriseDataService { size_t active_grids; size_t active_trees; size_t active_inspectors; size_t materialised_rows; size_t cached_cells; uint64_t revision; } UmiUiEntEnterpriseDataService;
void umi_ui_ent_enterprise_data_service_init(UmiUiEntEnterpriseDataService *s);
void umi_ui_ent_enterprise_data_service_update(UmiUiEntEnterpriseDataService *s,size_t grids,size_t trees,size_t inspectors,size_t rows,size_t cells);
int umi_ui_ent_enterprise_data_service_ready(const UmiUiEntEnterpriseDataService *s);

#endif
