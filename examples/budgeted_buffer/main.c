/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/budgeted_buffer/main.c
 *
 * PURPOSE:
 *   Demonstrate refusal-safe C buffer growth through the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/base/budget_allocator.h"
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv)
{
    if(argc>2 || (argc==2 && strcmp(argv[1],"--reject-only")!=0)) return 2;
    UmiBudgetAllocator *owner=NULL;
    void *buffer=NULL,*replacement=NULL;
    UmiBudgetAllocatorSnapshot state;
    int result=1;
    if(umi_budget_allocator_create(NULL,32U,2U,&owner)!=UMI_STATUS_OK) return 3;
    if(umi_budget_allocator_allocate(owner,16U,&buffer)!=UMI_STATUS_OK) goto cleanup;
    memcpy(buffer,"Umicom",7U);
    /* A 24-byte replacement would coexist with the current 16-byte buffer.
     * Refuse that 40-byte peak while preserving the six letters and their terminating zero byte. */
    if(umi_budget_allocator_resize(owner,buffer,24U,&replacement)!=UMI_STATUS_CAPACITY_EXCEEDED ||
       replacement!=NULL || memcmp(buffer,"Umicom",7U)!=0) goto cleanup;
    if(argc==1) {
        /* Shrinking still needs temporary space: 16+8 fits, then 8+24 fits.
         * Assign the new pointer only after each successful resize. */
        if(umi_budget_allocator_resize(owner,buffer,8U,&replacement)!=UMI_STATUS_OK) goto cleanup;
        buffer=replacement;replacement=NULL;
        if(umi_budget_allocator_resize(owner,buffer,24U,&replacement)!=UMI_STATUS_OK) goto cleanup;
        buffer=replacement;replacement=NULL;
    }
    if(umi_budget_allocator_snapshot(owner,&state)!=UMI_STATUS_OK ||
       state.live_bytes!=(argc==1?24U:16U) || state.peak_bytes!=(argc==1?32U:16U) ||
       state.capacity_refusals!=1U || memcmp(buffer,"Umicom",7U)!=0) goto cleanup;
    if(umi_budget_allocator_destroy(&owner)!=UMI_STATUS_BUSY) goto cleanup;
    if(printf("Umicom bounded buffer: live=%zu peak=%zu refusals=%llu\n",
        state.live_bytes,state.peak_bytes,(unsigned long long)state.capacity_refusals)<0 || fflush(stdout)!=0) goto cleanup;
    result=0;
cleanup:
    if(buffer!=NULL && umi_budget_allocator_release(owner,buffer)!=UMI_STATUS_OK) result=1;
    if(umi_budget_allocator_destroy(&owner)!=UMI_STATUS_OK) result=1;
    return result;
}
