/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_resize.h
 *
 * PURPOSE:
 *   Apply pointer/keyboard resize deltas while respecting semantic column bounds.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_RESIZE_H
#define UMICOM_UI_ENTERPRISE_COLUMN_RESIZE_H

#include "umicom/ui/enterprise/column_descriptor.h"
int32_t umi_ui_ent_column_resize_apply(const UmiUiEntColumnDescriptor *column,int32_t current_width,int32_t delta);

#endif
