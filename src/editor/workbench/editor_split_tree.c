/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_split_tree.c
 *
 * PURPOSE:
 *   Maintain an acyclic parent-linked split tree for editor groups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_split_tree.h"

#include <string.h>
/* Provide the index of operation used by this module and its client applications. */
static int index_of(const UmiEditorWbEditorSplitTree *t,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<t->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(t->nodes[i].node_id,id)==0)return (int)i;return -1;}
/*
 * Initialise editor wb editor split tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_editor_split_tree_init(UmiEditorWbEditorSplitTree *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t!=NULL)memset(t,0,sizeof *t);}
/*
 * Provide the editor wb editor split tree acyclic operation used by this module and its
 * client applications.
 */
int umi_editor_wb_editor_split_tree_acyclic(const UmiEditorWbEditorSplitTree *t){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<t->count;i++){char current[UMI_EDITOR_WB_ID_CAPACITY];size_t steps=0U;(void)umi_editor_wb_copy_text(current,sizeof current,t->nodes[i].parent_id);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(current[0]!='\0'){int p=index_of(t,current);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p<0)break;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(t->nodes[i].node_id,current)==0)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(++steps>t->count)return 0;(void)umi_editor_wb_copy_text(current,sizeof current,t->nodes[(size_t)p].parent_id);}}return 1;}
/*
 * Add editor wb editor split tree only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_editor_split_tree_add(UmiEditorWbEditorSplitTree *t,const char *id,const char *parent){UmiEditorWbSplitNode backup;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||!umi_editor_wb_id_valid(id)||parent==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index_of(t,id)>=0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(parent[0]!='\0'&&index_of(t,parent)<0)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->count>=UMI_EDITOR_WB_MAX_GROUPS)return UMI_STATUS_CAPACITY_EXCEEDED;memset(&backup,0,sizeof backup);(void)umi_editor_wb_copy_text(backup.node_id,sizeof backup.node_id,id);(void)umi_editor_wb_copy_text(backup.parent_id,sizeof backup.parent_id,parent);t->nodes[t->count++]=backup;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_editor_wb_editor_split_tree_acyclic(t)){t->count--;return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
