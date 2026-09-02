/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_resize.h
 *
 * PURPOSE:
 *   Apply pointer/keyboard resize deltas while respecting semantic column bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_RESIZE_H
#define UMICOM_UI_ENTERPRISE_COLUMN_RESIZE_H

#include "umicom/ui/enterprise/column_descriptor.h"
/**
 * Perform ui ent column resize through the module contract so client applications do not
 * duplicate its policy.
 */
int32_t umi_ui_ent_column_resize_apply(const UmiUiEntColumnDescriptor *column,int32_t current_width,int32_t delta);

#endif
