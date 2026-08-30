/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/keyboard_navigation.c
 *
 * PURPOSE:
 *   Implement deterministic keyboard traversal for enterprise grids.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/keyboard_navigation.h"
UmiUiEntNavigationPosition umi_ui_ent_keyboard_navigation_apply(UmiUiEntNavigationPosition p,UmiUiEntNavigationAction a,size_t rows,size_t cols,size_t page){if(rows==0U||cols==0U)return (UmiUiEntNavigationPosition){0U,0U};if(p.row>=rows)p.row=rows-1U;if(p.column>=cols)p.column=cols-1U;switch(a){case UMI_UI_ENT_NAV_LEFT:if(p.column>0U)p.column--;break;case UMI_UI_ENT_NAV_RIGHT:if(p.column+1U<cols)p.column++;break;case UMI_UI_ENT_NAV_UP:if(p.row>0U)p.row--;break;case UMI_UI_ENT_NAV_DOWN:if(p.row+1U<rows)p.row++;break;case UMI_UI_ENT_NAV_HOME:p.column=0U;break;case UMI_UI_ENT_NAV_END:p.column=cols-1U;break;case UMI_UI_ENT_NAV_PAGE_UP:p.row=p.row>page?p.row-page:0U;break;case UMI_UI_ENT_NAV_PAGE_DOWN:p.row=(page>rows-1U-p.row)?rows-1U:p.row+page;break;default:break;}return p;}
