/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/coalescer.c
 *
 * PURPOSE:
 *   Coalesce duplicate interaction events by source, kind or subject within a configurable window.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/coalescer.h"
#include <string.h>
static bool same_key(
    const UmiWorkbenchContextEventCoalescer *c,
    const UmiWorkbenchContextEvent *a,
    const UmiWorkbenchContextEvent *b)
{
    switch(c->mode){
    case UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SOURCE:
        return strcmp(a->source_id,b->source_id)==0&&a->kind==b->kind;
    case UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_KIND:
        return a->kind==b->kind&&a->content_hash==b->content_hash;
    case UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SUBJECT:
        return strcmp(a->subject_id,b->subject_id)==0&&a->kind==b->kind&&a->content_hash==b->content_hash;
    default:return false;
    }
}
void umi_workbench_context_event_coalescer_init(
    UmiWorkbenchContextEventCoalescer *c,
    UmiWorkbenchContextEventCoalescingMode mode,
    uint64_t window_ms)
{
    if (c == NULL) return;
    memset(c, 0, sizeof(*c));
    c->mode = mode;
    c->window_ms = window_ms;
    c->revision = 1U;
}
bool umi_workbench_context_event_coalescer_accept(
    UmiWorkbenchContextEventCoalescer *c,const UmiWorkbenchContextEvent *event)
{
    if(!c||!event)return false;
    if(c->has_last&&c->mode!=UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_NONE&&
       event->timestamp_ms>=c->last.timestamp_ms&&event->timestamp_ms-c->last.timestamp_ms<=c->window_ms&&
       same_key(c,&c->last,event)){
        ++c->coalesced_count;++c->revision;return false;
    }
    c->last=*event;c->has_last=true;++c->accepted_count;++c->revision;return true;
}
