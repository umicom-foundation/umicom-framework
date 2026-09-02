/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/focus_traversal.c
 *
 * PURPOSE:
 *   Maintain deterministic keyboard focus order independently of toolkit widget hierarchy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_traversal.h"
#include <string.h>
/*
 * Initialise appearance focus traversal from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_appearance_focus_traversal_init(UmiAppearanceFocusTraversal *order){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(order==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(order,0,sizeof *order);return UMI_STATUS_OK;}
/*
 * Add appearance focus traversal only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_appearance_focus_traversal_add(UmiAppearanceFocusTraversal *order,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(order==NULL||!umi_appearance_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<order->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(order->ids[i],id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(order->count>=UMI_APPEARANCE_MAX_FOCUS_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(order->ids[order->count],sizeof order->ids[order->count],id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;order->count++;return UMI_STATUS_OK;}
/*
 * Provide the appearance focus traversal next operation used by this module and its client
 * applications.
 */
const char *umi_appearance_focus_traversal_next(const UmiAppearanceFocusTraversal *order,const char *current_id,int wrap){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(order==NULL||order->count==0U)return NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(current_id==NULL||current_id[0]=='\0')return order->ids[0];/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<order->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(order->ids[i],current_id)==0){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<order->count)return order->ids[i+1U];return wrap?order->ids[0]:NULL;}return NULL;}
