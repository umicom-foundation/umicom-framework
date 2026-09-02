/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/transport.c
 *
 * PURPOSE:
 *   Validate runtime transports and adapt persistent process streams.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/transport.h"
#include <string.h>
/* Provide the w operation used by this module and its client applications. */
static UmiStatus w(void*i,const void*b,size_t n){return umi_language_runtime_process_stream_write(i,b,n);}static UmiStatus r(void*i,void*b,size_t c,uint32_t t,size_t*n){return umi_language_runtime_process_stream_read(i,b,c,t,n);}static UmiStatus s(void*i,uint32_t t){return umi_language_runtime_process_stream_stop(i,t);}static int q(void*i){return umi_language_runtime_process_stream_is_running(i);}static void d(void*i){umi_language_runtime_process_stream_destroy(i);}
/*
 * Check that language runtime transport satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_language_runtime_transport_validate(const UmiLanguageRuntimeTransport*t){return t&&t->instance&&t->write&&t->read&&t->stop&&t->is_running&&t->destroy?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the language runtime transport from process operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_transport_from_process(UmiLanguageRuntimeProcessStream*p,UmiLanguageRuntimeTransport*out){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));out->instance=p;out->write=w;out->read=r;out->stop=s;out->is_running=q;out->destroy=d;return UMI_STATUS_OK;}
