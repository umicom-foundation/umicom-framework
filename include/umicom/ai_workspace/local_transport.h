/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/local_transport.h
 *
 * PURPOSE:
 *   Own an explicit native local HTTP runtime before background requests are admitted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_LOCAL_TRANSPORT_H
#define UMICOM_AI_WORKSPACE_LOCAL_TRANSPORT_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiLocalTransport UmiAiLocalTransport;
/* Call during single-threaded application startup, outside DllMain/static DLL
 * initialization. This establishes global HTTP-library lifetime before workers
 * or GUI toolkits start threads. A native build without thread-safe global HTTP
 * support returns UNAVAILABLE. No network request or model is started.
 * Output remains unchanged on failure. All operations on this runtime and its
 * request controllers belong to the creating thread. */
UmiStatus UmiAiLocalTransportCreate(UmiAiLocalTransport **out_transport);
/* Counts retained request owners, including unstarted and completed jobs.
 * The count is not an inference-progress or network-connection measurement. */
UmiStatus UmiAiLocalTransportRequestCount(UmiAiLocalTransport *transport,size_t *out_count);
/* BUSY while any request owner retains this runtime, even if its worker ended.
 * Join and release those jobs first. NULL *transport succeeds. Wrong-thread
 * calls fail without release. This pairs only this owner's global init; other
 * Framework/external HTTP clients retain responsibility for their own lifetime.
 * Success clears the pointer. No external application threads are joined here. */
UmiStatus UmiAiLocalTransportDestroy(UmiAiLocalTransport **in_out_transport);
#ifdef __cplusplus
}
#endif
#endif
