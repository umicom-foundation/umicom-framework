/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/storyboard_bridge.h
 *
 * PURPOSE:
 *   Expose canonical storyboard validation through a bounded, local JSON binding.
 *
 * EDUCATIONAL OVERVIEW:
 *   Wire decoding is separate from scene rules. The canonical creative workspace
 *   owns scene validation and asset-reference binding; no parallel planner exists.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_STORYBOARD_BRIDGE_H
#define UMICOM_AI_WORKSPACE_STORYBOARD_BRIDGE_H
#include "umicom/ai_workspace/worker_transport.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Availability reflects linkage, not a fabricated fallback implementation. */
bool UmiAiStoryboardBridgeAvailable(void);
/* No filesystem/network/model operations occur; the caller owns all bytes. */
UmiStatus UmiAiStoryboardCompile(const char *input, size_t length,
    UmiAiWorkerWrite sink, void *context);
#ifdef __cplusplus
}
#endif
#endif
