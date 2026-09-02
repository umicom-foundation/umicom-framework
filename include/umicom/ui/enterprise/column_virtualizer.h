/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_virtualizer.h
 *
 * PURPOSE:
 *   Compute the materialisation range for virtual columns from a viewport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_VIRTUALIZER_H
#define UMICOM_UI_ENTERPRISE_COLUMN_VIRTUALIZER_H

#include "umicom/ui/enterprise/viewport.h"
/**
 * Represent the ui ent column virtualizer data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntColumnVirtualizer { size_t total; UmiUiEntSpan materialised; uint64_t generation; } UmiUiEntColumnVirtualizer;
/**
 * Provide the ui ent column virtualizer update operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_ent_column_virtualizer_update(UmiUiEntColumnVirtualizer *value,const UmiUiEntViewport *viewport,size_t total);

#endif
