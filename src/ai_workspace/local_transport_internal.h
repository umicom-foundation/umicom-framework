/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_transport_internal.h
 *
 * PURPOSE:
 *   Share owner-thread runtime retention between native local request controllers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_LOCAL_TRANSPORT_INTERNAL_H
#define UMICOM_AI_WORKSPACE_LOCAL_TRANSPORT_INTERNAL_H
#include "umicom/ai_workspace/local_transport.h"
UmiStatus AwLocalTransportRetain(UmiAiLocalTransport *transport);
UmiStatus AwLocalTransportRelease(UmiAiLocalTransport *transport);
#endif
