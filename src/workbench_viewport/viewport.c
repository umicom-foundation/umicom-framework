/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * All geometry is derived from the canonical layout. Toolkit adapters consume
 * this plan instead of reimplementing visibility, split and minimum-size rules.
 */
#include "umicom/workbench_layout/viewport.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define LIMIT UMI_WORKBENCH_LAYOUT_MAX_NODES
#define NONE UMI_WORKBENCH_LAYOUT_INDEX_NONE
static UmiStatus Fail(UmiWorkbenchViewportDiagnostic *d, size_t i,
    UmiStatus status, const char *message)
{
    if (d) {
        d->nodeIndex=i; d->status=status;
        (void)snprintf(d->message,sizeof d->message,"%s",message);
    }
    return status;
}
/* Bounded UTF-8 validation rejects overlong sequences, surrogate code points
 * and embedded controls in single-line labels. No strlen on external arrays. */
static bool Text(const char *s,size_t capacity,bool required)
{
    const char *end=memchr(s,0,capacity);
    if (!end || (required && end==s)) return false;
    size_t n=(size_t)(end-s),i=0;
    while (i<n) {
        unsigned c=(unsigned char)s[i++],count; uint32_t value,minimum;
        if(c<128U) { if(c<32U || c==127U)return false; continue; }
        if(c>=194U && c<=223U){count=1;value=c&31U;minimum=128;}
        else if(c>=224U && c<=239U){count=2;value=c&15U;minimum=2048;}
        else if(c>=240U && c<=244U){count=3;value=c&7U;minimum=65536;}
        else return false;
        if(count>n-i)return false;
        for(unsigned j=0;j<count;++j){unsigned b=(unsigned char)s[i++];if((b&192U)!=128U)return false;value=(value<<6)|(b&63U);}
        if(value<minimum || value>0x10ffffU || (value>=0xd800U && value<=0xdfffU))return false;
    }
    return true;
}
static bool Rect(UmiWorkbenchLayoutRect r)
{
    return r.width>=0 && r.height>=0 && (int64_t)r.x+r.width<=INT32_MAX &&
        (int64_t)r.y+r.height<=INT32_MAX;
}
static int32_t Max(int32_t a,int32_t b){return a>b?a:b;}
static int32_t Min(int32_t a,int32_t b){return a<b?a:b;}
static bool Leaf(UmiWorkbenchLayoutNodeKind kind)
{
    return kind==UMI_WORKBENCH_LAYOUT_NODE_PANEL || kind==UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP ||
        kind==UMI_WORKBENCH_LAYOUT_NODE_EMPTY;
}
UmiWorkbenchViewportOptions UmiWorkbenchViewportDefaults(void)
{ return (UmiWorkbenchViewportOptions){8,32}; }

static UmiStatus Check(const UmiWorkbenchLayoutDocument *d,size_t *order,
    UmiWorkbenchViewportDiagnostic *error)
{
    size_t incoming[LIMIT]={0},depth[LIMIT]={0};
    bool seen[LIMIT]={false};
    if(!d || d->structure_size<sizeof *d || d->version.schema_version!=UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION)
        return Fail(error,NONE,UMI_STATUS_INVALID_ARGUMENT,"The canonical layout structure or schema is unsupported.");
    if(!d->node_count || d->node_count>LIMIT || d->root_index>=d->node_count)
        return Fail(error,NONE,UMI_STATUS_CAPACITY_EXCEEDED,"The node count or root index is outside the layout bounds.");
    for(size_t i=0;i<d->node_count;++i){
        const UmiWorkbenchLayoutNode *n=&d->nodes[i];
        if(n->structure_size<sizeof *n || !Text(n->node_id,sizeof n->node_id,true) ||
            !Text(n->title,sizeof n->title,false) || !Text(n->component_id,sizeof n->component_id,false) ||
            !Text(n->owner_application_id,sizeof n->owner_application_id,false) ||
            !Text(n->context_group_id,sizeof n->context_group_id,false) || !Text(n->monitor_id,sizeof n->monitor_id,false))
            return Fail(error,i,UMI_STATUS_PARSE_ERROR,"A node structure, identifier or UTF-8 label is invalid.");
        for(size_t j=0;j<i;++j)if(!strcmp(n->node_id,d->nodes[j].node_id))
            return Fail(error,i,UMI_STATUS_ALREADY_EXISTS,"Node identifiers must be unique.");
        if(n->kind<UMI_WORKBENCH_LAYOUT_NODE_EMPTY || n->kind>UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW ||
            n->visibility<UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE || n->visibility>UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO ||
            n->child_count>UMI_WORKBENCH_LAYOUT_MAX_CHILDREN || n->minimum_size.width<0 || n->minimum_size.height<0 ||
            n->preferred_size.width<0 || n->preferred_size.height<0 || !Rect(n->bounds) ||
            n->orientation<UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE || n->orientation>UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL ||
            n->dock_region<UMI_WORKBENCH_LAYOUT_DOCK_CANVAS || n->dock_region>UMI_WORKBENCH_LAYOUT_DOCK_FLOATING)
            return Fail(error,i,UMI_STATUS_INVALID_ARGUMENT,"A node kind, size, visibility or child count is invalid.");
        if(n->active_child_index!=NONE && n->active_child_index>=n->child_count)
            return Fail(error,i,UMI_STATUS_INVALID_ARGUMENT,"Active tab is a child position and is outside the child list.");
        if(Leaf(n->kind) && n->child_count)
            return Fail(error,i,UMI_STATUS_INVALID_STATE,"A leaf cannot contain child nodes.");
        if((n->kind==UMI_WORKBENCH_LAYOUT_NODE_PANEL || n->kind==UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) &&
            (!n->component_id[0] || !n->owner_application_id[0]))
            return Fail(error,i,UMI_STATUS_INVALID_ARGUMENT,"A component leaf must identify its component and owning application.");
        if(n->kind==UMI_WORKBENCH_LAYOUT_NODE_SPLIT && (n->child_count!=2 ||
            n->orientation==UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE || !isfinite(n->split_ratio) ||
            n->split_ratio<0.05 || n->split_ratio>0.95))
            return Fail(error,i,UMI_STATUS_INVALID_ARGUMENT,"A split needs two children and a finite ratio from 0.05 to 0.95.");
        if(n->kind==UMI_WORKBENCH_LAYOUT_NODE_WINDOW || n->kind==UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW){
            if(i!=d->root_index)return Fail(error,i,UMI_STATUS_UNAVAILABLE,"Project each top-level window separately.");
            if(n->child_count>1)return Fail(error,i,UMI_STATUS_UNAVAILABLE,"A window needs one child container to arrange its content.");
        }
        for(size_t j=0;j<n->child_count;++j){
            size_t c=n->child_indices[j];
            if(c>=d->node_count || c==i)return Fail(error,i,UMI_STATUS_INVALID_STATE,"A child reference is invalid or self-referential.");
            if(++incoming[c]!=1 || d->nodes[c].parent_index!=i)
                return Fail(error,c,UMI_STATUS_INVALID_STATE,"Parent and child references disagree or a child is shared.");
        }
    }
    if(incoming[d->root_index] || d->nodes[d->root_index].parent_index!=NONE)
        return Fail(error,d->root_index,UMI_STATUS_INVALID_STATE,"The root must have no parent.");
    /* Iterative pre-order traversal. Every child follows its parent in order;
     * reversing the order then gives a bounded bottom-up minimum-size pass. */
    size_t stack[LIMIT],used=1,count=0;stack[0]=d->root_index;depth[d->root_index]=1;
    while(used){size_t i=stack[--used];if(seen[i])return Fail(error,i,UMI_STATUS_INVALID_STATE,"The layout contains a cycle.");
        seen[i]=true;order[count++]=i;
        const UmiWorkbenchLayoutNode *n=&d->nodes[i];
        if(depth[i]>UMI_WORKBENCH_VIEWPORT_MAX_DEPTH)return Fail(error,i,UMI_STATUS_CAPACITY_EXCEEDED,"The preview depth limit is exceeded.");
        for(size_t j=n->child_count;j>0;--j){size_t c=n->child_indices[j-1];
            if(used==LIMIT)return Fail(error,c,UMI_STATUS_CAPACITY_EXCEEDED,"Traversal capacity exceeded.");
            depth[c]=depth[i]+1;stack[used++]=c;
        }
    }
    if(count!=d->node_count)return Fail(error,NONE,UMI_STATUS_INVALID_STATE,"The layout contains unreachable nodes or a disconnected cycle.");
    return UMI_STATUS_OK;
}
UmiStatus UmiWorkbenchViewportBuild(const UmiWorkbenchLayoutDocument *d,
    UmiWorkbenchLayoutRect viewport,const UmiWorkbenchViewportOptions *options,
    UmiWorkbenchViewportPlan *out,UmiWorkbenchViewportDiagnostic *error)
{
    if(error){memset(error,0,sizeof *error);error->nodeIndex=NONE;}
    if(!out)return Fail(error,NONE,UMI_STATUS_INVALID_ARGUMENT,"An output plan is required.");
    memset(out,0,sizeof *out);
    UmiWorkbenchViewportOptions o=options?*options:UmiWorkbenchViewportDefaults();
    if(!Rect(viewport) || o.splitGap<0 || o.splitGap>1024 || o.tabHeight<0 || o.tabHeight>4096)
        return Fail(error,NONE,UMI_STATUS_INVALID_ARGUMENT,"Invalid viewport, split gap or tab height.");
    size_t order[LIMIT];UmiStatus status=Check(d,order,error);if(status!=UMI_STATUS_OK)return status;
    UmiWorkbenchViewportPlan p;memset(&p,0,sizeof p);p.nodeCount=d->node_count;
    for(size_t k=d->node_count;k>0;--k){size_t i=order[k-1];const UmiWorkbenchLayoutNode *n=&d->nodes[i];
        UmiWorkbenchViewportSlot *s=&p.slots[i];s->activeChild=NONE;
        if(n->visibility==UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN)continue;
        int64_t w=0,h=0;size_t visible=0;
        for(size_t j=0;j<n->child_count;++j){size_t c=n->child_indices[j];
            if(d->nodes[c].visibility==UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN)continue;
            ++visible;
            if(n->kind==UMI_WORKBENCH_LAYOUT_NODE_SPLIT && n->orientation==UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL){w+=p.slots[c].minimum.width;h=Max((int32_t)h,p.slots[c].minimum.height);}
            else if(n->kind==UMI_WORKBENCH_LAYOUT_NODE_SPLIT){h+=p.slots[c].minimum.height;w=Max((int32_t)w,p.slots[c].minimum.width);}
            else {w=Max((int32_t)w,p.slots[c].minimum.width);h=Max((int32_t)h,p.slots[c].minimum.height);}
        }
        if(visible==2 && n->kind==UMI_WORKBENCH_LAYOUT_NODE_SPLIT){if(n->orientation==UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL)w+=o.splitGap;else h+=o.splitGap;}
        if(visible && n->kind==UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP)h+=o.tabHeight;
        if(w>INT32_MAX || h>INT32_MAX)return Fail(error,i,UMI_STATUS_CAPACITY_EXCEEDED,"Combined minimum sizes exceed the logical-pixel range.");
        s->minimum=(UmiWorkbenchLayoutSize){Max((int32_t)w,n->minimum_size.width),Max((int32_t)h,n->minimum_size.height)};
    }
    p.minimum=p.slots[d->root_index].minimum;
    p.slots[d->root_index].visible=d->nodes[d->root_index].visibility!=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    p.slots[d->root_index].bounds=viewport;
    for(size_t k=0;k<d->node_count;++k){size_t i=order[k];const UmiWorkbenchLayoutNode *n=&d->nodes[i];UmiWorkbenchViewportSlot *s=&p.slots[i];
        if(!s->visible){s->bounds=(UmiWorkbenchLayoutRect){0};continue;}
        UmiWorkbenchLayoutRect r=s->bounds;
        s->minimumViolated=r.width<s->minimum.width || r.height<s->minimum.height;
        if(s->minimumViolated)++p.minimumViolations;
        if(Leaf(n->kind)){
            if(n->kind!=UMI_WORKBENCH_LAYOUT_NODE_EMPTY && r.width && r.height)p.focusOrder[p.focusCount++]=i;
            continue;
        }
        size_t children[UMI_WORKBENCH_LAYOUT_MAX_CHILDREN],positions[UMI_WORKBENCH_LAYOUT_MAX_CHILDREN],count=0;
        for(size_t j=0;j<n->child_count;++j)if(d->nodes[n->child_indices[j]].visibility!=UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN){children[count]=n->child_indices[j];positions[count++]=j;}
        if(!count)continue;
        if(n->kind==UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP){
            size_t chosen=0;for(size_t j=0;j<count;++j)if(positions[j]==n->active_child_index)chosen=j;
            s->activeChild=positions[chosen];int32_t bar=Min(r.height,o.tabHeight);
            s->decoration=(UmiWorkbenchLayoutRect){r.x,r.y,r.width,bar};r.y+=bar;r.height-=bar;
            p.slots[children[chosen]].visible=true;p.slots[children[chosen]].bounds=r;
        }else if(n->kind==UMI_WORKBENCH_LAYOUT_NODE_SPLIT && count==2){
            bool horizontal=n->orientation==UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL;
            int32_t length=horizontal?r.width:r.height,gap=Min(length,o.splitGap),available=length-gap;
            int32_t a=(int32_t)((long double)available*(long double)n->split_ratio);
            int32_t minA=horizontal?p.slots[children[0]].minimum.width:p.slots[children[0]].minimum.height;
            int32_t minB=horizontal?p.slots[children[1]].minimum.width:p.slots[children[1]].minimum.height;
            if((int64_t)minA+minB<=available){a=Max(a,minA);a=Min(a,available-minB);}
            UmiWorkbenchLayoutRect first=r,second=r,dec=r;
            if(horizontal){first.width=a;dec.x=r.x+a;dec.width=gap;second.x=dec.x+gap;second.width=available-a;}
            else{first.height=a;dec.y=r.y+a;dec.height=gap;second.y=dec.y+gap;second.height=available-a;}
            s->decoration=dec;p.slots[children[0]].bounds=first;p.slots[children[1]].bounds=second;
            p.slots[children[0]].visible=true;p.slots[children[1]].visible=true;
        }else {p.slots[children[0]].bounds=r;p.slots[children[0]].visible=true;}
    }
    *out=p;return UMI_STATUS_OK;
}
size_t UmiWorkbenchViewportNextFocus(const UmiWorkbenchViewportPlan *p,size_t current,bool reverse)
{
    if(!p || !p->focusCount || p->focusCount>LIMIT || p->nodeCount>LIMIT)return NONE;
    for(size_t i=0;i<p->focusCount;++i)if(p->focusOrder[i]>=p->nodeCount)return NONE;
    for(size_t i=0;i<p->focusCount;++i)if(p->focusOrder[i]==current)
        return p->focusOrder[reverse?(i?i-1:p->focusCount-1):(i+1)%p->focusCount];
    return p->focusOrder[reverse?p->focusCount-1:0];
}
