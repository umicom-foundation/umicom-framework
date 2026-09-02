/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/data_provider.c
 *
 * PURPOSE:
 *   Implement validation and bounded dispatch for data providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_provider.h"
/*
 * Check that ui ent data provider satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_data_provider_validate(const UmiUiEntDataProvider *p){return p&&p->row_count&&p->cell_at;}
/*
 * Provide the ui ent data provider cell operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_data_provider_cell(const UmiUiEntDataProvider *p,size_t r,size_t c,UmiUiEntCellValue *out){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_ui_ent_data_provider_validate(p)||!out)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r>=p->row_count(p->context))return UMI_STATUS_NOT_FOUND;return p->cell_at(p->context,r,c,out);}
