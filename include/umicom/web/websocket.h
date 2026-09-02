/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/websocket.h
 *
 * PURPOSE:
 *   Encode and decode bounded WebSocket frames.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_WEBSOCKET_H
#define UMICOM_WEB_WEBSOCKET_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named web socket opcode values accepted by this public contract.
 */
typedef enum UmiWebSocketOpcode { UMI_WS_TEXT=1, UMI_WS_BINARY=2, UMI_WS_CLOSE=8, UMI_WS_PING=9, UMI_WS_PONG=10 } UmiWebSocketOpcode;
/**
 * Write websocket in its stable representation and report capacity or input failures to
 * the caller.
 */
UmiStatus umi_websocket_encode(UmiWebSocketOpcode opcode,const uint8_t *payload,size_t length,uint8_t *out_frame,size_t capacity,size_t *out_length);
/**
 * Read websocket into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_websocket_decode(const uint8_t *frame,size_t length,UmiWebSocketOpcode *out_opcode,uint8_t *out_payload,size_t capacity,size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
