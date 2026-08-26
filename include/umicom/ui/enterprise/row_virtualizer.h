/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_virtualizer.h
 *
 * PURPOSE:
 *   Compute the materialisation range for virtual rows from a viewport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_VIRTUALIZER_H
#define UMICOM_UI_ENTERPRISE_ROW_VIRTUALIZER_H

#include "umicom/ui/enterprise/viewport.h"
typedef struct UmiUiEntRowVirtualizer { size_t total; UmiUiEntSpan materialised; uint64_t generation; } UmiUiEntRowVirtualizer;
UmiStatus umi_ui_ent_row_virtualizer_update(UmiUiEntRowVirtualizer *value,const UmiUiEntViewport *viewport,size_t total);

#endif
