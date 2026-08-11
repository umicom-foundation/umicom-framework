/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/bridge.c
 *
 * PURPOSE:
 *   Queue bounded messages between native code and embedded web content.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/bridge.h"
#include "umicom/web/types.h"
#include <stdlib.h>
#include <string.h>
struct UmiBrowserBridge{char messages[UMI_BROWSER_BRIDGE_CAPACITY][UMI_BROWSER_MESSAGE_CAPACITY];size_t head,count;};
UmiStatus umi_browser_bridge_create(UmiBrowserBridge **out){UmiBrowserBridge *b;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;b=(UmiBrowserBridge*)calloc(1U,sizeof(*b));if(b==NULL)return UMI_STATUS_OUT_OF_MEMORY;*out=b;return UMI_STATUS_OK;}
void umi_browser_bridge_destroy(UmiBrowserBridge *b){free(b);}
UmiStatus umi_browser_bridge_push(UmiBrowserBridge *b,const char *message){size_t idx;if(b==NULL||message==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(b->count>=UMI_BROWSER_BRIDGE_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;idx=(b->head+b->count)%UMI_BROWSER_BRIDGE_CAPACITY;if(umi_web_copy_text(b->messages[idx],sizeof(b->messages[idx]),message)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;++b->count;return UMI_STATUS_OK;}
UmiStatus umi_browser_bridge_pop(UmiBrowserBridge *b,char *out,size_t capacity){UmiStatus s;if(b==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(b->count==0U)return UMI_STATUS_NOT_FOUND;s=umi_web_copy_text(out,capacity,b->messages[b->head]);if(s!=UMI_STATUS_OK)return s;b->head=(b->head+1U)%UMI_BROWSER_BRIDGE_CAPACITY;--b->count;return UMI_STATUS_OK;}
size_t umi_browser_bridge_count(const UmiBrowserBridge *b){return b!=NULL?b->count:0U;}
