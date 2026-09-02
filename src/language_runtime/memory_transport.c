/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/memory_transport.c
 *
 * PURPOSE:
 *   Implement bounded in-memory bidirectional transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/memory_transport.h"
#include <stdlib.h>
#include <string.h>
struct UmiLanguageRuntimeMemoryTransport{char rb[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t rn,ro;char wb[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t wn;int run;};
/* Provide the mw operation used by this module and its client applications. */
static UmiStatus mw(void*i,const void*b,size_t n){UmiLanguageRuntimeMemoryTransport*m=i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!m||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->wn+n>=sizeof(m->wb))return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(m->wb+m->wn,b,n);m->wn+=n;m->wb[m->wn]=0;return UMI_STATUS_OK;}static UmiStatus mr(void*i,void*out,size_t cap,uint32_t t,size_t*n){UmiLanguageRuntimeMemoryTransport*m=i;size_t av,c;(void)t;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!m||!out||!cap||!n)return UMI_STATUS_INVALID_ARGUMENT;av=m->rn>m->ro?m->rn-m->ro:0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!av){*n=0;return UMI_STATUS_NOT_FOUND;}c=av<cap?av:cap;memcpy(out,m->rb+m->ro,c);m->ro+=c;*n=c;return UMI_STATUS_OK;}static UmiStatus ms(void*i,uint32_t t){(void)t;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!i)return UMI_STATUS_INVALID_ARGUMENT;((UmiLanguageRuntimeMemoryTransport*)i)->run=0;return UMI_STATUS_OK;}static int mq(void*i){return i&&((UmiLanguageRuntimeMemoryTransport*)i)->run;}static void md(void*i){free(i);}
/*
 * Initialise language runtime memory transport from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_language_runtime_memory_transport_create(UmiLanguageRuntimeMemoryTransport**m,UmiLanguageRuntimeTransport*out){UmiLanguageRuntimeMemoryTransport*x;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m||!out)return UMI_STATUS_INVALID_ARGUMENT;*m=NULL;memset(out,0,sizeof(*out));x=calloc(1,sizeof(*x));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!x)return UMI_STATUS_OUT_OF_MEMORY;x->run=1;out->instance=x;out->write=mw;out->read=mr;out->stop=ms;out->is_running=mq;out->destroy=md;*m=x;return UMI_STATUS_OK;}
/*
 * Read language runtime memory transport push into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_language_runtime_memory_transport_push_read(UmiLanguageRuntimeMemoryTransport*m,const void*b,size_t n){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!m||(b==NULL&&n))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->rn+n>=sizeof(m->rb))return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(m->rb+m->rn,b,n);m->rn+=n;m->rb[m->rn]=0;return UMI_STATUS_OK;}
/*
 * Provide the language runtime memory transport written operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_memory_transport_written(const UmiLanguageRuntimeMemoryTransport*m,char*out,size_t cap,size_t*n){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!m||!out||!cap||!n)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(m->wn+1>cap)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out,m->wb,m->wn);out[m->wn]=0;*n=m->wn;return UMI_STATUS_OK;}
