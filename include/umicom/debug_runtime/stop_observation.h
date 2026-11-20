/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/stop_observation.h
 *
 * PURPOSE:
 *   Read the first source frame at a native debugger stop within one deadline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_STOP_OBSERVATION_H
#define UMICOM_DEBUG_RUNTIME_STOP_OBSERVATION_H

#include "umicom/debug_runtime/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Wait for the active session to pause and copy its first stack frame.
 * The timeout covers event processing and stack retrieval together.
 * Call from the session's owner thread. The caller keeps control of its
 * session. The output is unchanged on failure. */
UmiStatus UmiDebugRuntimeObserveStop(UmiDebugRuntimePlatform *platform, uint32_t timeoutMs,
    UmiDebugStackFrameSnapshot *outFrame);

#ifdef __cplusplus
}
#endif
#endif
