/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/console_report_private.h
 *
 * PURPOSE:
 *   Keep console report rows independent from mutable debugger storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_CONSOLE_REPORT_PRIVATE_H
#define UMICOM_DEBUG_CONSOLE_REPORT_PRIVATE_H
#include "umicom/debug/console_report.h"
struct UmiDebugConsoleReport {
  UmiDebugConsoleQuerySnapshot summary;
  UmiDebugConsoleEntrySnapshot *rows;
};
#endif
