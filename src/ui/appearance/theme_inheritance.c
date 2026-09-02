/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_inheritance.c
 *
 * PURPOSE:
 *   Validate acyclic theme-pack inheritance before layered token resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_inheritance.h"
#include <string.h>
/* Provide the path to operation used by this module and its client applications. */
static int path_to(const UmiAppearanceThemeInheritance *g,const char *from,const char *target,size_t depth){ size_t i; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(depth>g->count) return 0; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(from,target)==0) return 1; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<g->count;i++) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(g->edges[i].child_id,from)==0 && path_to(g,g->edges[i].parent_id,target,depth+1U)) return 1; return 0; }
/*
 * Initialise appearance theme inheritance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_appearance_theme_inheritance_init(UmiAppearanceThemeInheritance *graph){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(graph,0,sizeof *graph); return UMI_STATUS_OK; }
/*
 * Add appearance theme inheritance only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_appearance_theme_inheritance_add(UmiAppearanceThemeInheritance *graph,const char *child_id,const char *parent_id){ UmiAppearanceThemeInheritanceEdge *e; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||!umi_appearance_id_valid(child_id)||!umi_appearance_id_valid(parent_id)||strcmp(child_id,parent_id)==0) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph->count>=UMI_APPEARANCE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(path_to(graph,parent_id,child_id,0U)) return UMI_STATUS_INVALID_STATE; e=&graph->edges[graph->count]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(e->child_id,sizeof e->child_id,child_id)!=UMI_STATUS_OK||umi_appearance_copy_text(e->parent_id,sizeof e->parent_id,parent_id)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; graph->count++; return UMI_STATUS_OK; }
/*
 * Provide the appearance theme inheritance has cycle operation used by this module and its
 * client applications.
 */
int umi_appearance_theme_inheritance_has_cycle(const UmiAppearanceThemeInheritance *graph){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL) return 0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<graph->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(path_to(graph,graph->edges[i].parent_id,graph->edges[i].child_id,0U)) return 1; return 0; }
