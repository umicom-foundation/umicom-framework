/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/backtrace.h
 *
 * PURPOSE:
 *   Capture and format an owned stopped-thread call stack without debugger execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_BACKTRACE_H
#define UMICOM_DEBUG_BACKTRACE_H
#include "umicom/debug/selection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDebugBacktrace UmiDebugBacktrace;
typedef struct UmiDebugBacktraceSummary {
  UmiDebugThreadSnapshot thread;
  size_t frame_count;
} UmiDebugBacktraceSummary;
typedef enum UmiDebugBacktracePaths {
  UMI_DEBUG_BACKTRACE_OMIT_PATHS = 0,
  UMI_DEBUG_BACKTRACE_FILENAMES = 1,
  UMI_DEBUG_BACKTRACE_FULL_PATHS = 2
} UmiDebugBacktracePaths;
/** Copy the selected stopped thread and its currently retained frames, in workspace
 * order. No DAP request, file access or expression evaluation occurs. The stack may
 * be incomplete if the adapter has not supplied every frame; no completeness claim
 * is made. Access the workspace on its owner thread without concurrent mutation.
 * No selected thread returns NOT_FOUND; a running thread returns INVALID_STATE.
 * Failure sets *out to NULL. The capture can outlive the workspace. */
UmiStatus UmiDebugBacktraceCapture(UmiDebugWorkspace *workspace, UmiDebugBacktrace **out);
UmiStatus UmiDebugBacktraceClone(const UmiDebugBacktrace *trace, UmiDebugBacktrace **out);
void UmiDebugBacktraceDestroy(UmiDebugBacktrace *trace);
UmiStatus UmiDebugBacktraceRead(const UmiDebugBacktrace *trace, UmiDebugBacktraceSummary *out);
UmiStatus UmiDebugBacktraceFrameAt(const UmiDebugBacktrace *trace, size_t index,
                                   UmiDebugStackFrameSnapshot *out);
/** Compare the captured workspace identity, selection and registry generations.
 * BUSY means the observation is older; the owned trace remains readable. This is
 * a conservative freshness check, not proof of live target state. */
UmiStatus UmiDebugBacktraceCheck(UmiDebugWorkspace *workspace, const UmiDebugBacktrace *trace);
/** Format plain UTF-8 text. Omit paths by default in public-facing hosts. Filename
 * mode keeps only the last slash/backslash component; full mode retains source
 * text exactly apart from escaped controls. These options are not a redaction
 * guarantee: function/thread names can themselves contain private data.
 * C0, DEL and backslashes are escaped; no markup is interpreted.
 * Query with out=NULL, capacity=0. Required bytes exclude the NUL terminator.
 * A short buffer reports the required length and writes nothing. Other failures
 * preserve both outputs. Output, required and trace storage must not overlap.
 * The formatter owns no output allocation and sends nothing to the clipboard. */
UmiStatus UmiDebugBacktraceFormat(const UmiDebugBacktrace *trace, UmiDebugBacktracePaths paths,
                                  char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
