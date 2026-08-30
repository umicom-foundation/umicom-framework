/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/websocket.c
 *
 * PURPOSE:
 *   Implement bounded WebSocket frame encoding and decoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/websocket.h"
UmiStatus umi_websocket_encode(UmiWebSocketOpcode opcode,const uint8_t *payload,size_t length,uint8_t *out_frame,size_t capacity,size_t *out_length){size_t header;if(out_frame==NULL||(payload==NULL&&length>0U)||length>65535U)return UMI_STATUS_INVALID_ARGUMENT;header=length<126U?2U:4U;if(header+length>capacity)return UMI_STATUS_CAPACITY_EXCEEDED;out_frame[0]=(uint8_t)(0x80U|(uint8_t)opcode);if(length<126U){out_frame[1]=(uint8_t)length;}else{out_frame[1]=126U;out_frame[2]=(uint8_t)((length>>8)&0xffU);out_frame[3]=(uint8_t)(length&0xffU);}for(size_t i=0U;i<length;++i)out_frame[header+i]=payload[i];if(out_length!=NULL)*out_length=header+length;return UMI_STATUS_OK;}
UmiStatus umi_websocket_decode(const uint8_t *frame,size_t length,UmiWebSocketOpcode *out_opcode,uint8_t *out_payload,size_t capacity,size_t *out_length){size_t payload_len,offset=2U,i;int masked;uint8_t mask[4]={0U,0U,0U,0U};if(frame==NULL||out_opcode==NULL||out_payload==NULL||length<2U)return UMI_STATUS_INVALID_ARGUMENT;masked=(frame[1]&0x80U)!=0U;payload_len=(size_t)(frame[1]&0x7fU);if(payload_len==126U){if(length<4U)return UMI_STATUS_PARSE_ERROR;payload_len=((size_t)frame[2]<<8)|(size_t)frame[3];offset=4U;}if(masked){if(length<offset+4U)return UMI_STATUS_PARSE_ERROR;for(i=0U;i<4U;++i)mask[i]=frame[offset+i];offset+=4U;}if(payload_len>capacity||length<offset+payload_len)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=0U;i<payload_len;++i)out_payload[i]=(uint8_t)(frame[offset+i]^(masked?mask[i%4U]:0U));*out_opcode=(UmiWebSocketOpcode)(frame[0]&0x0fU);if(out_length!=NULL)*out_length=payload_len;return UMI_STATUS_OK;}
