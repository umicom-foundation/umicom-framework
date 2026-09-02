/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/change_tracker.c
 *
 * PURPOSE:
 *   Track ORM entity mutations and prevent invalid persistence-state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/change_tracker.h"
#include <string.h>

/* Tracker reset keeps transaction boundaries explicit. */ void umi_data_change_tracker_init(UmiDataChangeTracker *tracker){if(tracker!=NULL)(void)memset(tracker,0,sizeof(*tracker));}
/* Insert+delete cancels, while delete followed by update is rejected as an invalid lifecycle. */
UmiStatus umi_data_change_tracker_track(UmiDataChangeTracker *tracker,const char *change_id,const char *entity_id,const char *key,UmiDataChangeKind kind){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tracker==NULL||change_id==NULL||entity_id==NULL||key==NULL||kind==UMI_DATA_CHANGE_NONE)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<tracker->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(tracker->items[i].entity_id,entity_id)==0&&strcmp(tracker->items[i].key,key)==0){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tracker->items[i].kind==UMI_DATA_CHANGE_DELETE&&kind!=UMI_DATA_CHANGE_INSERT)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tracker->items[i].kind==UMI_DATA_CHANGE_INSERT&&kind==UMI_DATA_CHANGE_DELETE){tracker->items[i]=tracker->items[tracker->count-1U];tracker->count--;tracker->revision++;return UMI_STATUS_OK;}tracker->items[i].kind=kind;tracker->items[i].revision=++tracker->revision;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tracker->count>=UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memset(&tracker->items[tracker->count],0,sizeof(tracker->items[tracker->count]));s=umi_data_enterprise_copy_text(tracker->items[tracker->count].change_id,sizeof(tracker->items[tracker->count].change_id),change_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(tracker->items[tracker->count].entity_id,sizeof(tracker->items[tracker->count].entity_id),entity_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(tracker->items[tracker->count].key,sizeof(tracker->items[tracker->count].key),key);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;tracker->items[tracker->count].kind=kind;tracker->items[tracker->count].revision=++tracker->revision;tracker->count++;return UMI_STATUS_OK;}
/* Counting by kind supports deterministic persistence-plan generation. */ size_t umi_data_change_tracker_count_kind(const UmiDataChangeTracker *tracker,UmiDataChangeKind kind){size_t i,n=0U;if(tracker==NULL)return 0U;for(i=0;i<tracker->count;++i)if(tracker->items[i].kind==kind)n++;return n;}
