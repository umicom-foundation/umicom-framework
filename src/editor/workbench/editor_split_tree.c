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
static int index_of(const UmiEditorWbEditorSplitTree *t,const char *id){size_t i;for(i=0U;i<t->count;i++)if(strcmp(t->nodes[i].node_id,id)==0)return (int)i;return -1;}
void umi_editor_wb_editor_split_tree_init(UmiEditorWbEditorSplitTree *t){if(t!=NULL)memset(t,0,sizeof *t);}
int umi_editor_wb_editor_split_tree_acyclic(const UmiEditorWbEditorSplitTree *t){size_t i;if(t==NULL)return 0;for(i=0U;i<t->count;i++){char current[UMI_EDITOR_WB_ID_CAPACITY];size_t steps=0U;(void)umi_editor_wb_copy_text(current,sizeof current,t->nodes[i].parent_id);while(current[0]!='\0'){int p=index_of(t,current);if(p<0)break;if(strcmp(t->nodes[i].node_id,current)==0)return 0;if(++steps>t->count)return 0;(void)umi_editor_wb_copy_text(current,sizeof current,t->nodes[(size_t)p].parent_id);}}return 1;}
UmiStatus umi_editor_wb_editor_split_tree_add(UmiEditorWbEditorSplitTree *t,const char *id,const char *parent){UmiEditorWbSplitNode backup;if(t==NULL||!umi_editor_wb_id_valid(id)||parent==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(index_of(t,id)>=0)return UMI_STATUS_ALREADY_EXISTS;if(parent[0]!='\0'&&index_of(t,parent)<0)return UMI_STATUS_NOT_FOUND;if(t->count>=UMI_EDITOR_WB_MAX_GROUPS)return UMI_STATUS_CAPACITY_EXCEEDED;memset(&backup,0,sizeof backup);(void)umi_editor_wb_copy_text(backup.node_id,sizeof backup.node_id,id);(void)umi_editor_wb_copy_text(backup.parent_id,sizeof backup.parent_id,parent);t->nodes[t->count++]=backup;if(!umi_editor_wb_editor_split_tree_acyclic(t)){t->count--;return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
