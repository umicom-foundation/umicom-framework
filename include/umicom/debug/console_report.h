/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/console_report.h
 *
 * PURPOSE:
 *   Own a bounded debugger-output observation for stable page review and plain-text copying.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_CONSOLE_REPORT_H
#define UMICOM_DEBUG_CONSOLE_REPORT_H
#include "umicom/debug/console_query.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugConsoleReport UmiDebugConsoleReport;
#define UMI_DEBUG_CONSOLE_REPORT_PAGE_LIMIT 128U
/** Copy a successfully executed console query. Rows, counts and source revision
 * stay fixed after the original registry/query changes or is destroyed. This
 * captures retained output, not a complete process log. UTF-8 and all text bounds
 * are checked before publication. No I/O occurs. Failure sets *out=NULL.
 * Access query on its owner thread without concurrent mutation. */
UmiStatus UmiDebugConsoleReportCreate(const UmiDebugConsoleQuery *query,
                                      UmiDebugConsoleReport **out);
void UmiDebugConsoleReportDestroy(UmiDebugConsoleReport *report);
UmiStatus UmiDebugConsoleReportRead(const UmiDebugConsoleReport *report,
                                    UmiDebugConsoleQuerySnapshot *out);
UmiStatus UmiDebugConsoleReportEntryAt(const UmiDebugConsoleReport *report, size_t index,
                                       UmiDebugConsoleEntrySnapshot *out);
/** Format up to limit rows from one immutable report; limit is 1..128.
 * Offset zero is valid for an empty report. Other offsets must name a retained
 * record. Required bytes exclude NUL. Query using out=NULL,capacity=0.
 * Short buffers change required only, never partially write text. Other failures
 * preserve outputs. Outputs must not overlap each other or the report.
 * Text is plain UTF-8; row bodies retain tabs/newlines while other C0 bytes, DEL
 * and backslashes are escaped. Header fields escape every C0 control byte.
 * No clipboard, terminal escape processing, file output or expression evaluation. */
UmiStatus UmiDebugConsoleReportFormat(const UmiDebugConsoleReport *report, size_t offset,
                                      size_t limit, char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
