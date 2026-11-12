/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/csv_export.h
 *
 * PURPOSE:
 *   Escape one enterprise grid field into RFC-style CSV text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CSV_EXPORT_H
#define UMICOM_UI_ENTERPRISE_CSV_EXPORT_H

#include "umicom/ui/enterprise/types.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ui ent csv export field operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_csv_export_field(const char *input,char *output,size_t capacity);


#ifdef __cplusplus
}
#endif
#endif
