/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_websocket.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <string.h>
int main(void){const uint8_t text[]={'h','i'};uint8_t frame[32],payload[32];size_t fl=0U,pl=0U;UmiWebSocketOpcode op;assert(umi_websocket_encode(UMI_WS_TEXT,text,2U,frame,sizeof(frame),&fl)==UMI_STATUS_OK);assert(umi_websocket_decode(frame,fl,&op,payload,sizeof(payload),&pl)==UMI_STATUS_OK);assert(op==UMI_WS_TEXT&&pl==2U&&memcmp(payload,text,2U)==0);return 0;}
