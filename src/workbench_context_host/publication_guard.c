/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/publication_guard.c
 *
 * PURPOSE:
 *   Reject immediate duplicate publications from the same source/group while preserving later revisits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/publication_guard.h"
#include <string.h>
/*
 * Initialise workbench context host publication guard from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_publication_guard_init(UmiWorkbenchContextHostPublicationGuard *guard)
{/* Apply this branch only when its contract condition is satisfied. */ if(guard){memset(guard,0,sizeof(*guard));guard->revision=1U;}}
/*
 * Provide the workbench context host publication guard accept operation used by this
 * module and its client applications.
 */
bool umi_workbench_context_host_publication_guard_accept(
    UmiWorkbenchContextHostPublicationGuard *guard,const char *source_panel_id,const char *group_id,
    const UmiContextPayload *payload,uint64_t now_ms)
{
    const char *source=source_panel_id?source_panel_id:"";
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!guard||!group_id||!payload)return false;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(guard->content_hash==payload->content_hash&&strcmp(guard->source_panel_id,source)==0&&
       strcmp(guard->group_id,group_id)==0&&now_ms>=guard->last_published_ms&&
       now_ms-guard->last_published_ms<50U){++guard->duplicate_count;++guard->revision;return false;}
    (void)umi_workbench_context_host_copy_text(guard->source_panel_id,sizeof(guard->source_panel_id),source);
    (void)umi_workbench_context_host_copy_text(guard->group_id,sizeof(guard->group_id),group_id);
    guard->content_hash=payload->content_hash;guard->last_published_ms=now_ms;++guard->accepted_count;++guard->revision;return true;
}
