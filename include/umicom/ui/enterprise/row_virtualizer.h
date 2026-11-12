/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_virtualizer.h
 *
 * PURPOSE:
 *   Compute the materialisation range for virtual rows from a viewport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_VIRTUALIZER_H
#define UMICOM_UI_ENTERPRISE_ROW_VIRTUALIZER_H

#include "umicom/ui/enterprise/viewport.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent row virtualizer data shared with callers of this public contract.
 */
typedef struct UmiUiEntRowVirtualizer { size_t total; UmiUiEntSpan materialised; uint64_t generation; } UmiUiEntRowVirtualizer;
/**
 * Provide the ui ent row virtualizer update operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_row_virtualizer_update(UmiUiEntRowVirtualizer *value,const UmiUiEntViewport *viewport,size_t total);


#ifdef __cplusplus
}
#endif
#endif
