/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/client_runtime.c
 *
 * PURPOSE:
 *   Emit the small Framework-controlled JavaScript runtime that applies DOM patches and forwards UI events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/client_runtime.h"

#include <string.h>
/*
 * Provide the native web client runtime emit operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_client_runtime_emit(const char *event_endpoint,UmiNativeWebMarkupBuffer *out_javascript)
{
    UmiStatus s;
    static const char *prefix="(()=>{const U={apply(p){for(const x of p){const e=document.getElementById(x.t);if(!e)continue;if(x.k===1)e.textContent=x.v||'';else if(x.k===2)e.setAttribute(x.n,x.v||'');else if(x.k===3)e.removeAttribute(x.n);else if(x.k===5)e.remove();else if(x.k===7)e.focus();}},send(k,t,v){fetch('";
    static const char *suffix="',{method:'POST',headers:{'content-type':'application/json'},body:JSON.stringify({k:k,t:t,v:v||''}),credentials:'same-origin'});}};document.addEventListener('click',e=>{const t=e.target.closest('[data-umi-command]');if(t)U.send('command',t.id,t.dataset.umiCommand);});document.addEventListener('input',e=>{if(e.target.id)U.send('input',e.target.id,e.target.value);});document.addEventListener('keydown',e=>{if(e.target.id)U.send('key',e.target.id,[e.ctrlKey?'Ctrl':'',e.altKey?'Alt':'',e.shiftKey?'Shift':'',e.key].filter(Boolean).join('+'));});window.UmicomWeb=U;})();";
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(event_endpoint==NULL||out_javascript==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    umi_native_web_markup_buffer_init(out_javascript);
    s=umi_native_web_markup_buffer_append(out_javascript,prefix);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_native_web_markup_buffer_append(out_javascript,event_endpoint);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    return umi_native_web_markup_buffer_append(out_javascript,suffix);
}
/*
 * Provide the native web client runtime uses webassembly operation used by this module and
 * its client applications.
 */
bool umi_native_web_client_runtime_uses_webassembly(const UmiNativeWebMarkupBuffer *runtime){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(runtime==NULL)return false;return strstr(runtime->data,"WebAssembly")!=NULL||strstr(runtime->data,".wasm")!=NULL;}

