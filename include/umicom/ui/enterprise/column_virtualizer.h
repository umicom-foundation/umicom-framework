/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_virtualizer.h
 *
 * PURPOSE:
 *   Compute the materialisation range for virtual columns from a viewport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_VIRTUALIZER_H
#define UMICOM_UI_ENTERPRISE_COLUMN_VIRTUALIZER_H

#include "umicom/ui/enterprise/viewport.h"
typedef struct UmiUiEntColumnVirtualizer { size_t total; UmiUiEntSpan materialised; uint64_t generation; } UmiUiEntColumnVirtualizer;
UmiStatus umi_ui_ent_column_virtualizer_update(UmiUiEntColumnVirtualizer *value,const UmiUiEntViewport *viewport,size_t total);

#endif
