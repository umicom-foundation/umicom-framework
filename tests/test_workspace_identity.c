/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_identity.c
 *
 * PURPOSE:
 *   Verify non-reused workspace identities and atomic publication at counter exhaustion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/workspace_graph.h"
#include "umicom/platform/directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); exit(1); } } while (0)

/* Include the real implementation to reach otherwise impractical uint64_t
 * limits. No public setter exposes these counters. Directory traversal and
 * mutexes remain real; the narrow hook adds commit-time failure or staleness. */
static UmiWorkspaceGraph *live_graph;
static int walk_fault;
static UmiStatus IdentityWalk(const char *,const UmiDirectoryWalkOptions *,
    UmiDirectoryVisitor,void *);
#define umi_directory_walk IdentityWalk
#include "../src/platform/workspace_graph.c"
#undef umi_directory_walk

static UmiStatus IdentityWalk(const char *root,const UmiDirectoryWalkOptions *options,
    UmiDirectoryVisitor visitor,void *data)
{
    UmiStatus status=umi_directory_walk(root,options,visitor,data);
    if(status!=UMI_STATUS_OK) return status;
    if(walk_fault==1) return UMI_STATUS_IO_ERROR;
    if(walk_fault==2) return umi_workspace_graph_set_trusted(live_graph,0);
    return status;
}

static UmiWorkspaceGraph *Capture(UmiWorkspaceGraph *graph)
{
    UmiWorkspaceGraph *copy=malloc(sizeof(*copy));
    CHECK(copy!=NULL);
    memcpy(copy,graph,sizeof(*copy));
    return copy;
}
static void Unchanged(UmiWorkspaceGraph *graph,const UmiWorkspaceGraph *copy)
{
    /* Compare copied object bytes, including unchanged padding and row storage.
     * The mutex is borrowed only as a stable pointer, never copied or operated. */
    CHECK(memcmp(graph,copy,sizeof(*graph))==0);
}
static UmiStatus Add(UmiWorkspaceGraph *graph,const char *leaf,uint64_t *out)
{
    return umi_workspace_graph_add_project(graph,leaf,leaf,leaf,
        UMI_PROJECT_CMAKE,out);
}

int main(int argc,char **argv)
{
    CHECK(argc==3);
    const char *name=argv[1],*root=argv[2];
    UmiWorkspaceGraph *graph=NULL,*before=NULL;
    uint64_t id=0,output=900U;
    CHECK(umi_workspace_graph_create(&graph)==UMI_STATUS_OK);
    CHECK(umi_workspace_graph_open(graph,root,1)==UMI_STATUS_OK);
    CHECK(Add(graph,"manual",&id)==UMI_STATUS_OK);
    live_graph=graph;
    if(strstr(name,"-overflow") || strcmp(name,"id-zero")==0) {
        if(strncmp(name,"discover-",9)==0) {
            if(strcmp(name,"discover-overflow")==0) graph->revision=UINT64_MAX;
            else graph->next_project_id=UINT64_MAX;
        } else if(strncmp(name,"id-",3)==0) {
            graph->next_project_id=strcmp(name,"id-zero")==0 ? 0U : UINT64_MAX;
        } else graph->revision=UINT64_MAX;
        before=Capture(graph);
        UmiStatus status=UMI_STATUS_OK;
        if(strcmp(name,"open-overflow")==0)
            status=umi_workspace_graph_open(graph,root,0);
        else if(strcmp(name,"close-overflow")==0)
            status=umi_workspace_graph_close(graph);
        else if(strcmp(name,"trust-overflow")==0)
            status=umi_workspace_graph_set_trusted(graph,0);
        else if(strcmp(name,"remove-overflow")==0)
            status=umi_workspace_graph_remove_project(graph,id);
        else if(strncmp(name,"discover-",9)==0)
            status=umi_workspace_graph_discover(graph);
        else status=Add(graph,"next",&output);
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(output==900U);
        Unchanged(graph,before);
    } else if(strcmp(name,"id-last")==0) {
        graph->next_project_id=UINT64_MAX-1U;
        CHECK(Add(graph,"next",&output)==UMI_STATUS_OK);
        CHECK(output==UINT64_MAX-1U && graph->next_project_id==UINT64_MAX);
        before=Capture(graph);
        output=900U;
        CHECK(Add(graph,"later",&output)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(output==900U);
        Unchanged(graph,before);
    } else if(strcmp(name,"discover-multi-last")==0) {
        graph->revision=UINT64_MAX-1U;
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_OK);
        CHECK(graph->project_count==3U && graph->revision==UINT64_MAX);
        CHECK(graph->projects[0].project_id==id);
        CHECK(graph->projects[1].project_id!=graph->projects[2].project_id);
        CHECK(graph->next_project_id==4U);
        before=Capture(graph);
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_OK);
        Unchanged(graph,before);
    } else if(strstr(name,"-last")!=NULL) {
        graph->revision=UINT64_MAX-1U;
        UmiStatus status=UMI_STATUS_INVALID_ARGUMENT;
        if(strcmp(name,"open-last")==0) {
            status=umi_workspace_graph_open(graph,root,0);
            CHECK(graph->open && !graph->trusted && graph->project_count==0U);
        } else if(strcmp(name,"close-last")==0) {
            status=umi_workspace_graph_close(graph);
            CHECK(!graph->open && graph->root[0]=='\0' && graph->project_count==0U);
        } else if(strcmp(name,"trust-last")==0) {
            status=umi_workspace_graph_set_trusted(graph,0);
            CHECK(!graph->trusted && graph->project_count==1U);
        } else if(strcmp(name,"add-last")==0) {
            status=Add(graph,"next",&output);
            CHECK(output==2U && graph->project_count==2U);
        } else if(strcmp(name,"remove-last")==0) {
            status=umi_workspace_graph_remove_project(graph,id);
            CHECK(graph->project_count==0U);
        }
        CHECK(status==UMI_STATUS_OK && graph->revision==UINT64_MAX);
    } else if(strcmp(name,"duplicate-at-limit")==0) {
        graph->revision=UINT64_MAX;
        graph->next_project_id=UINT64_MAX;
        before=Capture(graph);
        CHECK(Add(graph,"manual",&output)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(output==id);
        Unchanged(graph,before);
    } else if(strcmp(name,"missing-at-limit")==0) {
        graph->revision=UINT64_MAX;
        before=Capture(graph);
        CHECK(umi_workspace_graph_remove_project(graph,99U)==UMI_STATUS_NOT_FOUND);
        Unchanged(graph,before);
    } else if(strcmp(name,"discover-id-partial")==0) {
        graph->next_project_id=UINT64_MAX-1U;
        before=Capture(graph);
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_CAPACITY_EXCEEDED);
        Unchanged(graph,before);
    } else if(strncmp(name,"discover-nochange",17)==0) {
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_OK);
        CHECK(graph->project_count==3U);
        graph->revision=UINT64_MAX;
        if(strcmp(name,"discover-nochange-id-limit")==0)
            graph->next_project_id=UINT64_MAX;
        before=Capture(graph);
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_OK);
        Unchanged(graph,before);
    } else if(strcmp(name,"discover-failure")==0) {
        before=Capture(graph);
        walk_fault=1;
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_IO_ERROR);
        Unchanged(graph,before);
    } else if(strcmp(name,"discover-stale")==0) {
        before=Capture(graph);
        walk_fault=2;
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_INVALID_STATE);
        /* The injected newer trust change survives; unpublished discovered
         * rows and candidate IDs must not replace it. */
        before->trusted=0;
        before->revision++;
        Unchanged(graph,before);
    } else if(strcmp(name,"session-ids")==0) {
        uint64_t previous_revision=graph->revision;
        CHECK(umi_workspace_graph_close(graph)==UMI_STATUS_OK);
        CHECK(umi_workspace_graph_open(graph,root,1)==UMI_STATUS_OK);
        CHECK(Add(graph,"manual",&output)==UMI_STATUS_OK);
        CHECK(output>id && graph->revision==previous_revision+3U);
    } else if(strcmp(name,"closed")==0) {
        CHECK(umi_workspace_graph_close(graph)==UMI_STATUS_OK);
        graph->revision=UINT64_MAX;
        before=Capture(graph);
        CHECK(umi_workspace_graph_set_trusted(graph,1)==UMI_STATUS_INVALID_STATE);
        CHECK(Add(graph,"next",&output)==UMI_STATUS_INVALID_STATE);
        CHECK(umi_workspace_graph_discover(graph)==UMI_STATUS_INVALID_STATE);
        CHECK(output==900U);
        Unchanged(graph,before);
    } else if(strcmp(name,"invalid")==0) {
        graph->revision=UINT64_MAX;
        before=Capture(graph);
        CHECK(umi_workspace_graph_open(graph,NULL,1)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_workspace_graph_open(graph,"",1)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_workspace_graph_remove_project(graph,0U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_workspace_graph_add_project(graph,"x",NULL,"x",
            UMI_PROJECT_GENERIC,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(output==900U);
        Unchanged(graph,before);
    } else CHECK(0);
    free(before);
    /* Destruction must not need another identity even after exhaustion. */
    umi_workspace_graph_destroy(graph);
    return 0;
}
