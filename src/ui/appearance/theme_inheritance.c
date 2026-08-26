/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_inheritance.c
 *
 * PURPOSE:
 *   Validate acyclic theme-pack inheritance before layered token resolution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_inheritance.h"
#include <string.h>
static int path_to(const UmiAppearanceThemeInheritance *g,const char *from,const char *target,size_t depth){ size_t i; if(depth>g->count) return 0; if(strcmp(from,target)==0) return 1; for(i=0;i<g->count;i++) if(strcmp(g->edges[i].child_id,from)==0 && path_to(g,g->edges[i].parent_id,target,depth+1U)) return 1; return 0; }
UmiStatus umi_appearance_theme_inheritance_init(UmiAppearanceThemeInheritance *graph){ if(graph==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(graph,0,sizeof *graph); return UMI_STATUS_OK; }
UmiStatus umi_appearance_theme_inheritance_add(UmiAppearanceThemeInheritance *graph,const char *child_id,const char *parent_id){ UmiAppearanceThemeInheritanceEdge *e; if(graph==NULL||!umi_appearance_id_valid(child_id)||!umi_appearance_id_valid(parent_id)||strcmp(child_id,parent_id)==0) return UMI_STATUS_INVALID_ARGUMENT; if(graph->count>=UMI_APPEARANCE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; if(path_to(graph,parent_id,child_id,0U)) return UMI_STATUS_INVALID_STATE; e=&graph->edges[graph->count]; if(umi_appearance_copy_text(e->child_id,sizeof e->child_id,child_id)!=UMI_STATUS_OK||umi_appearance_copy_text(e->parent_id,sizeof e->parent_id,parent_id)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; graph->count++; return UMI_STATUS_OK; }
int umi_appearance_theme_inheritance_has_cycle(const UmiAppearanceThemeInheritance *graph){ size_t i; if(graph==NULL) return 0; for(i=0;i<graph->count;i++) if(path_to(graph,graph->edges[i].parent_id,graph->edges[i].child_id,0U)) return 1; return 0; }
