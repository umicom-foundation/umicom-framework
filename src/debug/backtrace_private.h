/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/backtrace_private.h
 *
 * PURPOSE:
 *   Keep immutable backtrace ownership separate from mutable debugger registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_BACKTRACE_PRIVATE_H
#define UMICOM_DEBUG_BACKTRACE_PRIVATE_H
#include "umicom/debug/backtrace.h"
struct UmiDebugBacktrace {
  UmiDebugViewStamp stamp;
  UmiDebugBacktraceSummary summary;
  UmiDebugStackFrameSnapshot *frames;
};
#endif
