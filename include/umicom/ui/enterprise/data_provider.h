/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/data_provider.h
 *
 * PURPOSE:
 *   Define a pull-based provider contract for virtual grids without owning backend data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_DATA_PROVIDER_H
#define UMICOM_UI_ENTERPRISE_DATA_PROVIDER_H

#include "umicom/ui/enterprise/cell_value.h"
typedef size_t (*UmiUiEntRowCountFn)(void *context);
typedef UmiStatus (*UmiUiEntCellFn)(void *context,size_t row,size_t column,UmiUiEntCellValue *out_value);
typedef struct UmiUiEntDataProvider { void *context; UmiUiEntRowCountFn row_count; UmiUiEntCellFn cell_at; uint64_t revision; } UmiUiEntDataProvider;
int umi_ui_ent_data_provider_validate(const UmiUiEntDataProvider *provider);
UmiStatus umi_ui_ent_data_provider_cell(const UmiUiEntDataProvider *provider,size_t row,size_t column,UmiUiEntCellValue *out_value);

#endif
