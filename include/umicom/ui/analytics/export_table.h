/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/export_table.h
 *
 * PURPOSE:
 *   Build bounded analytical tables and emit standards-compliant CSV text.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_EXPORT_TABLE_H
#define UMICOM_UI_ANALYTICS_EXPORT_TABLE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ANALYTICS_EXPORT_ROWS 32U
#define UMI_ANALYTICS_EXPORT_COLUMNS 16U
#define UMI_ANALYTICS_EXPORT_CELL 128U
#define UMI_ANALYTICS_EXPORT_BUFFER 16384U
/**
 * Represent the analytics export table data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsExportTable { char cells[UMI_ANALYTICS_EXPORT_ROWS][UMI_ANALYTICS_EXPORT_COLUMNS][UMI_ANALYTICS_EXPORT_CELL]; size_t rows; size_t columns; } UmiAnalyticsExportTable;
/**
 * Initialise analytics export table from caller-provided values so later operations
 * receive a known state.
 */
void umi_analytics_export_table_init(UmiAnalyticsExportTable *t,size_t columns);
/**
 * Copy analytics export table into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_analytics_export_table_set(UmiAnalyticsExportTable *t,size_t row,size_t column,const char *value);
/**
 * Provide the analytics export table csv operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_export_table_csv(const UmiAnalyticsExportTable *t,char *buffer,size_t capacity,size_t *out_length);

#ifdef __cplusplus
}
#endif

#endif
