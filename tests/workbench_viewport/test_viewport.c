/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "../../examples/workbench_viewport/practice_layout.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
#define NONE UMI_WORKBENCH_LAYOUT_INDEX_NONE
static UmiWorkbenchLayoutDocument d;
static UmiWorkbenchViewportPlan p;
static UmiWorkbenchViewportDiagnostic error;
static UmiWorkbenchLayoutRect box={0,0,1000,600};
static UmiStatus Build(void){return UmiWorkbenchViewportBuild(&d,box,NULL,&p,&error);}
static int Zero(void){const unsigned char *b=(const unsigned char*)&p;for(size_t i=0;i<sizeof p;++i)if(b[i])return 0;return 1;}
static void MinimumZero(void){for(size_t i=0;i<d.node_count;++i)d.nodes[i].minimum_size=(UmiWorkbenchLayoutSize){0,0};}
static int Invariants(void)
{
    CHECK(p.nodeCount==d.node_count);
    for(size_t i=0;i<d.node_count;++i){UmiWorkbenchLayoutRect r=p.slots[i].bounds;
        CHECK(r.width>=0 && r.height>=0);
        if(p.slots[i].visible){CHECK(r.x>=box.x && r.y>=box.y);CHECK((int64_t)r.x+r.width<=(int64_t)box.x+box.width);CHECK((int64_t)r.y+r.height<=(int64_t)box.y+box.height);}
        if(d.nodes[i].kind==UMI_WORKBENCH_LAYOUT_NODE_SPLIT && p.slots[i].visible){
            size_t a=d.nodes[i].child_indices[0],b=d.nodes[i].child_indices[1];
            if(p.slots[a].visible && p.slots[b].visible){UmiWorkbenchLayoutRect x=p.slots[a].bounds,y=p.slots[b].bounds,g=p.slots[i].decoration;
                if(d.nodes[i].orientation==UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL){CHECK((int64_t)x.width+g.width+y.width==r.width);CHECK((int64_t)x.x+x.width==g.x && (int64_t)g.x+g.width==y.x);}
                else{CHECK((int64_t)x.height+g.height+y.height==r.height);CHECK((int64_t)x.y+x.height==g.y && (int64_t)g.y+g.height==y.y);}
            }
        }
    }
    for(size_t i=0;i<p.focusCount;++i){size_t n=p.focusOrder[i];CHECK(n<d.node_count && p.slots[n].visible && p.slots[n].bounds.width>0 && p.slots[n].bounds.height>0);}
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];UmiViewportLessonCreate(&d);
    if(!strcmp(name,"example")){CHECK(Build()==UMI_STATUS_OK);CHECK(p.focusCount==3 && p.focusOrder[0]==2 && p.focusOrder[1]==5 && p.focusOrder[2]==7);CHECK(p.slots[2].bounds.width==248);CHECK(p.slots[5].bounds.x==256 && p.slots[5].bounds.y==32);CHECK(p.slots[5].bounds.width==744 && p.slots[5].bounds.height==412);CHECK(!p.minimumViolations);}
    else if(!strcmp(name,"tab_position")){d.nodes[4].active_child_index=1;CHECK(Build()==UMI_STATUS_OK);CHECK(p.slots[6].visible && !p.slots[5].visible && p.slots[4].activeChild==1);}
    else if(!strcmp(name,"hidden_tab")){d.nodes[5].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;CHECK(Build()==UMI_STATUS_OK);CHECK(p.slots[4].activeChild==1 && d.nodes[4].active_child_index==0);}
    else if(!strcmp(name,"no_active_tab")){d.nodes[4].active_child_index=NONE;CHECK(Build()==UMI_STATUS_OK);CHECK(p.slots[4].activeChild==0);}
    else if(!strcmp(name,"all_tabs_hidden")){d.nodes[5].visibility=d.nodes[6].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;CHECK(Build()==UMI_STATUS_OK);CHECK(!p.slots[4].decoration.height && p.slots[4].activeChild==NONE && p.focusCount==2);}
    else if(!strcmp(name,"hidden_sidebar")){d.nodes[2].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;CHECK(Build()==UMI_STATUS_OK);CHECK(p.slots[3].bounds.width==box.width && !p.slots[1].decoration.width);}
    else if(!strcmp(name,"hidden_container")){d.nodes[3].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;CHECK(Build()==UMI_STATUS_OK);CHECK(!p.slots[5].visible && p.focusCount==1 && p.slots[2].bounds.width==1000);}
    else if(!strcmp(name,"hidden_root")){d.nodes[0].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;CHECK(Build()==UMI_STATUS_OK);CHECK(!p.focusCount);}
    else if(!strcmp(name,"auto_visibility")){d.nodes[2].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO;CHECK(Build()==UMI_STATUS_OK && p.slots[2].visible);}
    else if(!strcmp(name,"minimum_clamp")){d.nodes[2].minimum_size.width=400;d.nodes[1].split_ratio=0.05;CHECK(Build()==UMI_STATUS_OK);CHECK(p.slots[2].bounds.width==400);}
    else if(!strcmp(name,"right_minimum")){d.nodes[1].split_ratio=0.95;CHECK(Build()==UMI_STATUS_OK);CHECK(p.slots[3].bounds.width==360);}
    else if(!strcmp(name,"insufficient")){box.width=100;box.height=20;CHECK(Build()==UMI_STATUS_OK);CHECK(p.minimumViolations>0);}
    else if(!strcmp(name,"zero_viewport")){box.width=box.height=0;CHECK(Build()==UMI_STATUS_OK);CHECK(!p.focusCount);}
    else if(!strcmp(name,"negative_origin")){box.x=-1200;box.y=-700;CHECK(Build()==UMI_STATUS_OK);}
    else if(!strcmp(name,"edge_coordinates")){box.x=INT32_MAX-1000;box.y=INT32_MAX-600;CHECK(Build()==UMI_STATUS_OK);}
    else if(!strcmp(name,"focus")){CHECK(Build()==UMI_STATUS_OK);CHECK(UmiWorkbenchViewportNextFocus(&p,NONE,false)==2);CHECK(UmiWorkbenchViewportNextFocus(&p,2,true)==7);CHECK(UmiWorkbenchViewportNextFocus(&p,7,false)==2);CHECK(UmiWorkbenchViewportNextFocus(&p,5,true)==2);CHECK(UmiWorkbenchViewportNextFocus(NULL,1,false)==NONE);}
    else if(!strcmp(name,"utf8")){strcpy(d.nodes[5].title,"R\xc3\xa9sum\xc3\xa9 \xf0\x9f\x93\x9d");CHECK(Build()==UMI_STATUS_OK);}
    else if(!strcmp(name,"immutable")){UmiWorkbenchLayoutDocument *copy=malloc(sizeof d);CHECK(copy);memcpy(copy,&d,sizeof d);CHECK(Build()==UMI_STATUS_OK);CHECK(!memcmp(copy,&d,sizeof d));free(copy);}
    else if(!strcmp(name,"sweep")){MinimumZero();for(int width=0;width<=511;++width)for(int ratio=5;ratio<=95;ratio+=5){box.width=width;box.height=width+37;d.nodes[1].split_ratio=(double)ratio/100;CHECK(Build()==UMI_STATUS_OK);CHECK(!Invariants());}return 0;}
    else if(!strcmp(name,"large")){MinimumZero();box.width=box.height=INT32_MAX;CHECK(Build()==UMI_STATUS_OK);}
    else if(!strcmp(name,"depth") || !strcmp(name,"depth_exceeded")) {
        size_t depth = !strcmp(name,"depth") ? 64U : 65U;
        memset(&d,0,sizeof d);
        d.structure_size=sizeof d;
        d.version.schema_version=UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;
        d.node_count=depth;
        for(size_t i=0;i<depth;++i) {
            UmiWorkbenchLayoutNode *n=&d.nodes[i];
            n->structure_size=sizeof *n;
            (void)snprintf(n->node_id,sizeof n->node_id,"node-%zu",i);
            n->kind=i+1==depth?UMI_WORKBENCH_LAYOUT_NODE_EMPTY:UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP;
            n->visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
            n->dock_region=UMI_WORKBENCH_LAYOUT_DOCK_CANVAS;
            n->active_child_index=NONE;
            n->parent_index=i?i-1:NONE;
            if(i+1<depth){n->child_count=1;n->child_indices[0]=i+1;}
        }
        if(depth==65){CHECK(Build()==UMI_STATUS_CAPACITY_EXCEEDED && Zero());return 0;}
        CHECK(Build()==UMI_STATUS_OK);
    }
    else if(!strcmp(name,"zero_decoration")){UmiWorkbenchViewportOptions o={0,0};CHECK(UmiWorkbenchViewportBuild(&d,box,&o,&p,&error)==UMI_STATUS_OK);CHECK(p.slots[2].bounds.width==250 && p.slots[5].bounds.y==0);}
    else if(!strcmp(name,"invalid_options")){UmiWorkbenchViewportOptions o={-1,20};CHECK(UmiWorkbenchViewportBuild(&d,box,&o,&p,&error)==UMI_STATUS_INVALID_ARGUMENT && Zero());return 0;}
    else if(!strcmp(name,"invalid_output")){CHECK(UmiWorkbenchViewportBuild(&d,box,NULL,NULL,&error)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    else if(!strcmp(name,"invalid_document")){CHECK(UmiWorkbenchViewportBuild(NULL,box,NULL,&p,&error)==UMI_STATUS_INVALID_ARGUMENT && Zero());return 0;}
    else {
        if(!strcmp(name,"nan"))d.nodes[1].split_ratio=NAN;
        else if(!strcmp(name,"infinite"))d.nodes[1].split_ratio=INFINITY;
        else if(!strcmp(name,"ratio"))d.nodes[1].split_ratio=0;
        else if(!strcmp(name,"child_range"))d.nodes[1].child_indices[0]=999;
        else if(!strcmp(name,"child_count"))d.nodes[1].child_count=17;
        else if(!strcmp(name,"duplicate_child"))d.nodes[1].child_indices[1]=2;
        else if(!strcmp(name,"duplicate_id"))strcpy(d.nodes[6].node_id,d.nodes[5].node_id);
        else if(!strcmp(name,"parent"))d.nodes[5].parent_index=0;
        else if(!strcmp(name,"unreachable"))d.nodes[4].child_count=1;
        else if(!strcmp(name,"root"))d.root_index=200;
        else if(!strcmp(name,"root_parent"))d.nodes[0].parent_index=1;
        else if(!strcmp(name,"self_cycle"))d.nodes[1].child_indices[0]=1;
        else if(!strcmp(name,"disconnected_cycle")) {
            d.node_count=10;
            d.nodes[8]=d.nodes[4];d.nodes[9]=d.nodes[4];
            strcpy(d.nodes[8].node_id,"cycle-a");strcpy(d.nodes[9].node_id,"cycle-b");
            d.nodes[8].parent_index=9;d.nodes[9].parent_index=8;
            d.nodes[8].child_count=d.nodes[9].child_count=1;
            d.nodes[8].child_indices[0]=9;d.nodes[9].child_indices[0]=8;
            d.nodes[8].active_child_index=d.nodes[9].active_child_index=NONE;
        }
        else if(!strcmp(name,"hidden_invalid")) {
            d.nodes[3].visibility=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
            d.nodes[4].child_indices[0]=999;
        }
        else if(!strcmp(name,"unterminated"))memset(d.nodes[5].title,'x',sizeof d.nodes[5].title);
        else if(!strcmp(name,"overlong_utf8"))strcpy(d.nodes[5].title,"\xc0\x80");
        else if(!strcmp(name,"surrogate"))strcpy(d.nodes[5].title,"\xed\xa0\x80");
        else if(!strcmp(name,"control_label"))strcpy(d.nodes[5].title,"Notes\n");
        else if(!strcmp(name,"negative_size"))d.nodes[5].minimum_size.width=-1;
        else if(!strcmp(name,"sum_overflow")){d.nodes[2].minimum_size.width=INT32_MAX;d.nodes[5].minimum_size.width=INT32_MAX;}
        else if(!strcmp(name,"coordinate_overflow"))box.x=INT32_MAX;
        else if(!strcmp(name,"negative_viewport"))box.width=-1;
        else if(!strcmp(name,"bad_schema"))d.version.schema_version=42;
        else if(!strcmp(name,"bad_structure"))d.structure_size=0;
        else if(!strcmp(name,"bad_node_structure"))d.nodes[5].structure_size=0;
        else if(!strcmp(name,"tab_range"))d.nodes[4].active_child_index=6;
        else if(!strcmp(name,"panel_children")){d.nodes[5].child_count=1;d.nodes[5].child_indices[0]=6;}
        else if(!strcmp(name,"nested_window"))d.nodes[3].kind=UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW;
        else if(!strcmp(name,"missing_component"))d.nodes[5].component_id[0]=0;
        else if(!strcmp(name,"too_many_nodes"))d.node_count=257;
        else if(!strcmp(name,"empty_document"))d.node_count=0;
        else {fprintf(stderr,"Unknown case: %s\n",name);return 2;}
        CHECK(Build()!=UMI_STATUS_OK && Zero());CHECK(error.status!=UMI_STATUS_OK && error.message[0]);return 0;
    }
    CHECK(!Invariants());return 0;
}
