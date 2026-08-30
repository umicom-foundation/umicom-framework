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
UmiUiEntSpan umi_ui_ent_virtual_range_intersection(UmiUiEntSpan a,UmiUiEntSpan b);
UmiUiEntSpan umi_ui_ent_virtual_range_union(UmiUiEntSpan a,UmiUiEntSpan b);
UmiUiEntSpan umi_ui_ent_virtual_range_clamp(UmiUiEntSpan value,size_t total);

#endif
