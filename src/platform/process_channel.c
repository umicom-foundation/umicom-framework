/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_channel.c
 * PURPOSE:
 *   Shared validation and diagnostic-tail ownership for interactive processes.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Shared validation and diagnostic-tail ownership for interactive processes. */
#include "process_channel_internal.h"
#include <limits.h>
static int Absolute(const char *s){
#ifdef _WIN32
    return s&&strlen(s)>3U&&((s[0]>='A'&&s[0]<='Z')||(s[0]>='a'&&s[0]<='z'))&&s[1]==':'&&(s[2]=='/'||s[2]=='\\');
#else
    return s&&s[0]=='/';
#endif
}
UmiStatus PcValidate(const UmiProcessChannelRequest *r){
    if(!r||!Absolute(r->program)||!Absolute(r->workingDirectory)||        r->argumentCount>UMI_CHANNEL_MAX_ARGUMENTS||(r->argumentCount&&!r->arguments))return UMI_STATUS_INVALID_ARGUMENT;
    size_t total=strlen(r->program)+strlen(r->workingDirectory);
    if(total>16000U)return UMI_STATUS_CAPACITY_EXCEEDED;
    for(size_t i=0;i<r->argumentCount;++i){
        if(!r->arguments[i])return UMI_STATUS_INVALID_ARGUMENT;
        size_t n=strlen(r->arguments[i]);
        if(n>16000U-total)return UMI_STATUS_CAPACITY_EXCEEDED;
        total+=n;
    }
    return UMI_STATUS_OK;
}
void PcDiagnostic(UmiProcessChannel *c,const char *p,size_t n){
    size_t cap=sizeof c->snapshot.diagnostics-1U,old=strlen(c->snapshot.diagnostics);
    if(n>=cap){
        p+=n-cap;
        n=cap;
        old=0;
        c->snapshot.diagnosticsTruncated=1;
    }
    if(old+n>cap){
        size_t drop=old+n-cap;
        memmove(c->snapshot.diagnostics,c->snapshot.diagnostics+drop,old-drop);
        old-=drop;
        c->snapshot.diagnosticsTruncated=1;
    }
    /* Diagnostic bytes are not a protocol and may contain NULs. Keep their
                             * existence visible without allowing a NUL to hide following errors. */
    for(size_t i=0;i<n;++i)c->snapshot.diagnostics[old+i]=p[i]?p[i]:'?';
    c->snapshot.diagnostics[old+n]=0;
}
