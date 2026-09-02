/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_visibility.h
 *
 * PURPOSE:
 *   Maintain user column-visibility overrides by semantic identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_VISIBILITY_H
#define UMICOM_UI_ENTERPRISE_COLUMN_VISIBILITY_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent column visibility data shared with callers of this public contract.
 */
typedef struct UmiUiEntColumnVisibility { char hidden[UMI_UI_ENT_MAX_COLUMNS][UMI_UI_ENT_ID_CAPACITY]; size_t hidden_count; } UmiUiEntColumnVisibility;
/**
 * Initialise ui ent column visibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_column_visibility_init(UmiUiEntColumnVisibility *v);
/**
 * Provide the ui ent column visibility hide operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_visibility_hide(UmiUiEntColumnVisibility *v,const char *id);
/**
 * Provide the ui ent column visibility show operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_visibility_show(UmiUiEntColumnVisibility *v,const char *id);
/**
 * Provide the ui ent column visibility is visible operation used by this module and its
 * client applications.
 */
int umi_ui_ent_column_visibility_is_visible(const UmiUiEntColumnVisibility *v,const char *id);

#endif
