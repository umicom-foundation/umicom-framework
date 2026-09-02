/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/virtual_range.h
 *
 * PURPOSE:
 *   Provide intersection and union operations for virtual materialisation ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_VIRTUAL_RANGE_H
#define UMICOM_UI_ENTERPRISE_VIRTUAL_RANGE_H

#include "umicom/ui/enterprise/types.h"
/**
 * Provide the ui ent virtual range intersection operation used by this module and its
 * client applications.
 */
UmiUiEntSpan umi_ui_ent_virtual_range_intersection(UmiUiEntSpan a,UmiUiEntSpan b);
/**
 * Provide the ui ent virtual range union operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_virtual_range_union(UmiUiEntSpan a,UmiUiEntSpan b);
/**
 * Provide the ui ent virtual range clamp operation used by this module and its client
 * applications.
 */
UmiUiEntSpan umi_ui_ent_virtual_range_clamp(UmiUiEntSpan value,size_t total);

#endif
