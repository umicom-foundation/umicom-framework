/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/timeline.c
 *
 * PURPOSE:
 *   Implement the timeline behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework debugger timeline. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/debug/timeline.h"
#include <stdlib.h>
#include <string.h>
struct UmiDebugTimeline{UmiDebugEventSnapshot items[UMI_DEBUG_TIMELINE_CAPACITY];size_t count;uint64_t revision;};
/*
 * Initialise debug timeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_timeline_create(UmiDebugTimeline**out){UmiDebugTimeline*t;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;t=calloc(1U,sizeof(*t));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_OUT_OF_MEMORY;t->revision=1U;*out=t;return UMI_STATUS_OK;}
/* Release or reset state held by debug timeline so the same storage can be reused safely. */
void umi_debug_timeline_destroy(UmiDebugTimeline*t){free(t);}
/* Add debug timeline only after its inputs and available capacity have been checked. */
UmiStatus umi_debug_timeline_append(UmiDebugTimeline*t,const UmiDebugEventSnapshot*e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||e==NULL||e->kind[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->count>=UMI_DEBUG_TIMELINE_CAPACITY){(void)memmove(&t->items[0],&t->items[1],(UMI_DEBUG_TIMELINE_CAPACITY-1U)*sizeof(t->items[0]));t->count-=1U;}t->items[t->count]=*e;t->items[t->count].id[127U]='\0';t->items[t->count].kind[127U]='\0';t->items[t->count].detail[1023U]='\0';t->revision+=1U;t->items[t->count].revision=t->revision;t->count+=1U;return UMI_STATUS_OK;}
/*
 * Find debug timeline while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_timeline_at(const UmiDebugTimeline*t,size_t i,UmiDebugEventSnapshot*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=t->count)return UMI_STATUS_NOT_FOUND;*out=t->items[i];return UMI_STATUS_OK;}
/* Return the number of records represented by debug timeline without changing their state. */
size_t umi_debug_timeline_count(const UmiDebugTimeline*t){return t!=NULL?t->count:0U;}
/* Release or reset state held by debug timeline so the same storage can be reused safely. */
void umi_debug_timeline_clear(UmiDebugTimeline*t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t!=NULL){(void)memset(t->items,0,sizeof(t->items));t->count=0U;t->revision+=1U;}}
