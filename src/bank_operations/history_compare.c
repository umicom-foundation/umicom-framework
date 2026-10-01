/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/history_compare.c
 * PURPOSE: Compare canonical event values for retained reviews without reading structure padding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>
UmiStatus BankEventSame(const UmiBankAuditEvent *a,const UmiBankAuditEvent *b,bool *outSame)
{
    if(outSame==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outSame=false;
    if(a==NULL||b==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    char left[BANK_RECORD_TEXT_CAPACITY],right[BANK_RECORD_TEXT_CAPACITY];
    UmiStatus status=BankEncode(a,left,sizeof(left));
    if(status==UMI_STATUS_OK)status=BankEncode(b,right,sizeof(right));
    if(status==UMI_STATUS_OK)*outSame=strcmp(left,right)==0;
    return status;
}
/* Revision is only a fast rejection; a different local database may contain
 * different events at that revision. Compare every canonical encoded event. */
UmiStatus BankHistoryMatches(const BankState *state,uint64_t revision,
    const UmiBankAuditEvent *events,size_t count)
{
    if(state==NULL||(count!=0&&events==NULL)||count>UMI_BANK_EVENT_CAPACITY)return UMI_STATUS_INVALID_ARGUMENT;
    if(state->counts.revision!=revision||state->counts.events!=count)return UMI_STATUS_BUSY;
    for(size_t i=0;i<count;++i){
        bool same=false;UmiStatus status=BankEventSame(&events[i],&state->events[i],&same);
        if(status!=UMI_STATUS_OK)return status;
        if(!same)return UMI_STATUS_BUSY;
    }
    return UMI_STATUS_OK;
}
