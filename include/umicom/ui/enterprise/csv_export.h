/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/csv_export.h
 *
 * PURPOSE:
 *   Escape one enterprise grid field into RFC-style CSV text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CSV_EXPORT_H
#define UMICOM_UI_ENTERPRISE_CSV_EXPORT_H

#include "umicom/ui/enterprise/types.h"
UmiStatus umi_ui_ent_csv_export_field(const char *input,char *output,size_t capacity);

#endif
