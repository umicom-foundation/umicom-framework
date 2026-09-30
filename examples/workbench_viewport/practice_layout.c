/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/workbench_viewport/practice_layout.c
 * PURPOSE:
 *   One example builds a window, a sidebar, two tabs and a lower status panel. The values
 *   describe presentation only: no banking or filesystem operations.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One example builds a window, a sidebar, two tabs and a lower status panel.
 * The values describe presentation only: no banking or filesystem operations.
 */
#include "practice_layout.h"
#include <string.h>
static void Node(UmiWorkbenchLayoutDocument *d,size_t index,const char *id,
    const char *title,UmiWorkbenchLayoutNodeKind kind,size_t parent,const char *component)
{
    UmiWorkbenchLayoutNode *n=&d->nodes[index];
    n->structure_size=sizeof *n;n->kind=kind;n->parent_index=parent;
    n->active_child_index=UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    n->visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
    n->dock_region=UMI_WORKBENCH_LAYOUT_DOCK_CANVAS;n->split_ratio=0.5;n->revision=1;
    n->flags=UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE|UMI_WORKBENCH_LAYOUT_NODE_MOVABLE;
    /* These are fixed, reviewed literals below the canonical field capacities. */
    strcpy(n->node_id,id);strcpy(n->title,title);
    if(component){strcpy(n->component_id,component);strcpy(n->owner_application_id,"umicom.gallery");}
    for(size_t i=0;i<UMI_WORKBENCH_LAYOUT_MAX_CHILDREN;++i)n->child_indices[i]=UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}
void UmiViewportLessonCreate(UmiWorkbenchLayoutDocument *d)
{
    if(!d)return;
    memset(d,0,sizeof *d);d->structure_size=sizeof *d;
    d->version.schema_version=UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;d->version.revision=1;
    strcpy(d->identity.layout_id,"umicom.gallery.workspace");strcpy(d->name,"Notes and account browser");
    d->node_count=8;d->root_index=0;
    Node(d,0,"window","Practice workspace",UMI_WORKBENCH_LAYOUT_NODE_WINDOW,UMI_WORKBENCH_LAYOUT_INDEX_NONE,NULL);
    Node(d,1,"columns","Workspace columns",UMI_WORKBENCH_LAYOUT_NODE_SPLIT,0,NULL);
    Node(d,2,"navigation","Notebook",UMI_WORKBENCH_LAYOUT_NODE_PANEL,1,"gallery.navigation");
    Node(d,3,"rows","Editor and status",UMI_WORKBENCH_LAYOUT_NODE_SPLIT,1,NULL);
    Node(d,4,"documents","Documents",UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP,3,NULL);
    Node(d,5,"notes","Notes",UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP,4,"gallery.notes");
    Node(d,6,"accounts","Account browser",UMI_WORKBENCH_LAYOUT_NODE_PANEL,4,"gallery.accounts");
    Node(d,7,"status","Controls and status",UMI_WORKBENCH_LAYOUT_NODE_PANEL,3,"gallery.status");
    d->nodes[0].child_count=1;d->nodes[0].child_indices[0]=1;
    d->nodes[1].child_count=2;d->nodes[1].child_indices[0]=2;d->nodes[1].child_indices[1]=3;
    d->nodes[1].orientation=UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL;d->nodes[1].split_ratio=0.25;
    d->nodes[3].child_count=2;d->nodes[3].child_indices[0]=4;d->nodes[3].child_indices[1]=7;
    d->nodes[3].orientation=UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL;d->nodes[3].split_ratio=0.75;
    d->nodes[4].child_count=2;d->nodes[4].child_indices[0]=5;d->nodes[4].child_indices[1]=6;d->nodes[4].active_child_index=0;
    d->nodes[2].minimum_size=(UmiWorkbenchLayoutSize){180,120};
    d->nodes[5].minimum_size=(UmiWorkbenchLayoutSize){320,180};
    d->nodes[6].minimum_size=(UmiWorkbenchLayoutSize){360,160};
    d->nodes[7].minimum_size=(UmiWorkbenchLayoutSize){320,100};
}
