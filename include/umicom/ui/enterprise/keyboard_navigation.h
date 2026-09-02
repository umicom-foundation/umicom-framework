/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/keyboard_navigation.h
 *
 * PURPOSE:
 *   Translate keyboard navigation intents into clamped logical grid positions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_KEYBOARD_NAVIGATION_H
#define UMICOM_UI_ENTERPRISE_KEYBOARD_NAVIGATION_H

#include "umicom/ui/enterprise/types.h"
/**
 * List the named ui ent navigation action values accepted by this public contract.
 */
typedef enum UmiUiEntNavigationAction { UMI_UI_ENT_NAV_LEFT=1,UMI_UI_ENT_NAV_RIGHT=2,UMI_UI_ENT_NAV_UP=3,UMI_UI_ENT_NAV_DOWN=4,UMI_UI_ENT_NAV_HOME=5,UMI_UI_ENT_NAV_END=6,UMI_UI_ENT_NAV_PAGE_UP=7,UMI_UI_ENT_NAV_PAGE_DOWN=8 } UmiUiEntNavigationAction;
/**
 * Represent the ui ent navigation position data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntNavigationPosition { size_t row,column; } UmiUiEntNavigationPosition;
/**
 * Perform ui ent keyboard navigation through the module contract so client applications do
 * not duplicate its policy.
 */
UmiUiEntNavigationPosition umi_ui_ent_keyboard_navigation_apply(UmiUiEntNavigationPosition p,UmiUiEntNavigationAction a,size_t rows,size_t columns,size_t page_rows);

#endif
