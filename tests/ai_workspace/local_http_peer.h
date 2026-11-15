/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/local_http_peer.h
 *
 * PURPOSE:
 *   Own a bounded test-only ephemeral loopback peer with Framework request parsing and joined thread lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_LOCAL_HTTP_PEER_H
#define UMICOM_TEST_LOCAL_HTTP_PEER_H
#include "umicom/ai_workspace/workspace.h"
#include "umicom/web/request.h"
typedef struct LocalHttpPeer LocalHttpPeer;
typedef struct LocalHttpPeerOptions {
    const char *response;size_t response_bytes,fragment_bytes;
    uint32_t delay_millis;
    bool allow_disconnect;
    UmiAiWorkspaceCancellation *cancel_after_request;
} LocalHttpPeerOptions;
/* The response/token are borrowed until Finish succeeds in releasing the peer.
 * The peer accepts one request on an ephemeral 127.0.0.1 port. All waits have
 * a five-second deadline; deliberate delay is at most one second. */
UmiStatus LocalHttpPeerStart(const LocalHttpPeerOptions *options,LocalHttpPeer **out,uint16_t *out_port);
/* Join and release before copying the captured request. A native join/release
 * failure leaves *peer owned for retry. A completed peer error frees ownership,
 * returns IO_ERROR and preserves out_request. NULL *peer is invalid. */
UmiStatus LocalHttpPeerFinish(LocalHttpPeer **peer,UmiWebRequest *out_request);
/* Concurrent observation only; captured request storage is still copied by
 * Finish after joining. The peer must remain alive while this is queried. */
bool LocalHttpPeerHasRequest(const LocalHttpPeer *peer);
#endif
