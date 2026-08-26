/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_visibility.h
 *
 * PURPOSE:
 *   Maintain user column-visibility overrides by semantic identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_VISIBILITY_H
#define UMICOM_UI_ENTERPRISE_COLUMN_VISIBILITY_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntColumnVisibility { char hidden[UMI_UI_ENT_MAX_COLUMNS][UMI_UI_ENT_ID_CAPACITY]; size_t hidden_count; } UmiUiEntColumnVisibility;
void umi_ui_ent_column_visibility_init(UmiUiEntColumnVisibility *v);
UmiStatus umi_ui_ent_column_visibility_hide(UmiUiEntColumnVisibility *v,const char *id);
UmiStatus umi_ui_ent_column_visibility_show(UmiUiEntColumnVisibility *v,const char *id);
int umi_ui_ent_column_visibility_is_visible(const UmiUiEntColumnVisibility *v,const char *id);

#endif
