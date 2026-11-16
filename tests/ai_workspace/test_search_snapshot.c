/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_search_snapshot.c
 *
 * PURPOSE:
 *   Exercise canonical retrieval ownership, selection atomicity and stale-source preparation through actual in-memory Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workspace/search_snapshot.h"
#include "umicom/ai_workspace/evidence.h"
#include "umicom/ai_workspace/providers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct Fixture {
    UmiDataServer *data;
    UmiAiRuntime runtime;
    UmiAiWorkspace *workspace;
    int rerankMode;
    size_t calls;
    UmiStatus nested;
} Fixture;
static Fixture fixture;
static int Open(void)
{
    umi_ai_runtime_init(&fixture.runtime);
    CHECK(umi_data_server_create_memory(&fixture.data)==UMI_STATUS_OK);
    UmiAiProvider provider;
    CHECK(UmiAiWorkspaceExtractiveProviderCreate(&provider)==UMI_STATUS_OK);
    CHECK(umi_ai_provider_registry_add(&fixture.runtime.providers,&provider)==UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceCreate(fixture.data,&fixture.runtime,"search.lesson",&fixture.workspace)==UMI_STATUS_OK);
    CHECK(UmiAiWorkspacePutCollection(fixture.workspace,"main","Workshop 東京")==UMI_STATUS_OK);
    CHECK(UmiAiWorkspacePutCollection(fixture.workspace,"other","Separate collection")==UMI_STATUS_OK);
    const char *ids[]={"b","a","e","d","c"};
    for(size_t i=0U;i<5U;++i)
        CHECK(UmiAiWorkspacePutSource(fixture.workspace,ids[i],"main",ids[i],"workshop 東京 opens at 10:00",7U)==UMI_STATUS_OK);
    CHECK(UmiAiWorkspacePutSource(fixture.workspace,"secret","other","Separate source","workshop confidential",2U)==UMI_STATUS_OK);
    return 0;
}
static void Close(void)
{
    UmiAiWorkspaceDestroy(fixture.workspace);fixture.workspace=NULL;
    umi_data_server_destroy(fixture.data);fixture.data=NULL;
    umi_ai_runtime_destroy(&fixture.runtime);
}
static double Rerank(const char *query,const UmiAiChunk *chunk,double base,void *context)
{
    Fixture *f=context;(void)query;++f->calls;
    if(f->rerankMode==1)return -1.0;
    if(f->rerankMode==2)return NAN;
    if(f->rerankMode==3)return INFINITY;
    if(f->rerankMode==4){
        f->nested=UmiAiWorkspacePutCollection(f->workspace,"nested","Must be refused");
        return base;
    }
    return !strcmp(chunk->chunk_id,"e")?2.0:base;
}
static int SeedVector(const char *model)
{
    UmiAiWorkspaceSource source;UmiAiEmbedding value={0};value.dimension=2U;value.values[0]=1.0F;
    CHECK(UmiAiWorkspaceSourceAt(fixture.workspace,1U,&source)==UMI_STATUS_OK && !strcmp(source.id,"a"));
    CHECK(UmiAiWorkspaceSetEmbedding(fixture.workspace,"a",source.revision,model,&value)==UMI_STATUS_OK);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    static const char *const modes[]={"lexical","limit","empty","collection-scope","unicode","unknown-collection","invalid-collection","null-query","invalid-utf8","long-query","stopwords","zero-limit","large-limit","null-workspace","occupied","null-output","vector","missing-vector","model-mismatch","invalid-vector","null-model","rerank","negative-rerank","nan-rerank","infinite-rerank","invalid-reranker","reentrant","ownership","input-copy","changed-source","source-copy","summary-refusal","row-refusal","select-order","select-maximum","select-duplicate","select-late-invalid","select-zero","select-overflow","select-null","select-alias","prepare","stale-selected-source","stale-other-source","capacity"};
    int recognized=0;for(size_t i=0U;i<sizeof modes/sizeof modes[0];++i)if(!strcmp(mode,modes[i]))recognized=1;
    CHECK(recognized);CHECK(Open()==0);
    const char *collection="main",*query="workshop",*model="synthetic.embedding";
    char queryStorage[UMI_AI_WORKSPACE_PASSAGE_CAPACITY+1U];
    UmiAiEmbedding embedding={0};embedding.dimension=2U;embedding.values[0]=1.0F;
    const UmiAiEmbedding *vector=NULL;UmiAiReranker ranker={Rerank,&fixture};const UmiAiReranker *reranker=NULL;
    size_t limit=UMI_AI_MAX_RETRIEVAL_RESULTS;UmiStatus expected=UMI_STATUS_OK;
    UmiAiSearchSnapshot *snapshot=NULL;
    if(!strcmp(mode,"limit"))limit=2U;
    else if(!strcmp(mode,"empty"))query="unmatched";
    else if(!strcmp(mode,"unicode"))query="東京";
    else if(!strcmp(mode,"unknown-collection")){collection="missing";expected=UMI_STATUS_NOT_FOUND;}
    else if(!strcmp(mode,"invalid-collection")){collection="bad id";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-query")){query=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-utf8")){queryStorage[0]=(char)0xff;queryStorage[1]='\0';query=queryStorage;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"long-query")){memset(queryStorage,'x',sizeof queryStorage);queryStorage[sizeof queryStorage-1U]='\0';query=queryStorage;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"stopwords")){query="the and";expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"zero-limit")){limit=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"large-limit")){limit=UMI_AI_MAX_RETRIEVAL_RESULTS+1U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"null-workspace") || !strcmp(mode,"null-output"))expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(!strcmp(mode,"occupied")){
        CHECK(UmiAiWorkspaceSearchCapture(fixture.workspace,collection,query,NULL,NULL,NULL,limit,&snapshot)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_ARGUMENT;
    }else if(!strcmp(mode,"vector") || !strcmp(mode,"missing-vector") || !strcmp(mode,"model-mismatch") ||
        !strcmp(mode,"invalid-vector") || !strcmp(mode,"null-model")){
        vector=&embedding;query="unmatched";
        if(!strcmp(mode,"vector"))CHECK(SeedVector(model)==0);
        if(!strcmp(mode,"model-mismatch"))CHECK(SeedVector("other.embedding")==0);
        if(!strcmp(mode,"missing-vector") || !strcmp(mode,"model-mismatch"))expected=UMI_STATUS_UNAVAILABLE;
        if(!strcmp(mode,"invalid-vector")){embedding.dimension=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
        if(!strcmp(mode,"null-model")){model=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    }else if(!strcmp(mode,"rerank") || !strcmp(mode,"negative-rerank") || !strcmp(mode,"nan-rerank") ||
        !strcmp(mode,"infinite-rerank") || !strcmp(mode,"invalid-reranker") || !strcmp(mode,"reentrant")){
        reranker=&ranker;
        if(!strcmp(mode,"negative-rerank"))fixture.rerankMode=1;
        if(!strcmp(mode,"nan-rerank"))fixture.rerankMode=2;
        if(!strcmp(mode,"infinite-rerank"))fixture.rerankMode=3;
        if(!strcmp(mode,"reentrant"))fixture.rerankMode=4;
        if(fixture.rerankMode>=1 && fixture.rerankMode<=3)expected=UMI_STATUS_INVALID_STATE;
        if(!strcmp(mode,"invalid-reranker")){ranker.score=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    }else if(!strcmp(mode,"input-copy")){strcpy(queryStorage,"workshop");query=queryStorage;}
    else if(!strcmp(mode,"capacity")){
        for(unsigned i=0U;i<20U;++i){
            char id[32];int n=snprintf(id,sizeof id,"row-%02u",i);CHECK(n>0 && (size_t)n<sizeof id);
            CHECK(UmiAiWorkspacePutSource(fixture.workspace,id,"main",id,"workshop",1U)==UMI_STATUS_OK);
        }
    }
    UmiAiWorkspaceSnapshot before,after;
    CHECK(UmiAiWorkspaceSnapshotRead(fixture.workspace,&before)==UMI_STATUS_OK);
    UmiAiSearchSnapshot *old=snapshot;
    CHECK(UmiAiWorkspaceSearchCapture(!strcmp(mode,"null-workspace")?NULL:fixture.workspace,
        collection,query,model,vector,reranker,limit,!strcmp(mode,"null-output")?NULL:&snapshot)==expected);
    CHECK(UmiAiWorkspaceSnapshotRead(fixture.workspace,&after)==UMI_STATUS_OK);
    CHECK(before.revision==after.revision && before.corpusRevision==after.corpusRevision &&
        after.jobCount==0U && before.sourceCount==after.sourceCount);
    if(expected!=UMI_STATUS_OK){CHECK(snapshot==old);UmiAiSearchSnapshotDestroy(snapshot);Close();return 0;}
    if(!strcmp(mode,"ownership"))Close();
    if(!strcmp(mode,"input-copy"))memset(queryStorage,'x',sizeof queryStorage);
    UmiAiSearchSummary summary;CHECK(UmiAiSearchSnapshotInspect(snapshot,&summary)==UMI_STATUS_OK);
    size_t expectedCount=!strcmp(mode,"empty")?0U:(!strcmp(mode,"vector")?1U:(!strcmp(mode,"limit")?2U:(!strcmp(mode,"capacity")?16U:5U)));
    CHECK(summary.count==expectedCount && summary.requestedLimit==limit && summary.corpusRevision==before.corpusRevision);
    CHECK(!strcmp(summary.collection.id,"main") && !strcmp(summary.collection.title,"Workshop 東京"));
    CHECK(summary.vectorRequested==(vector!=NULL));
    if(vector!=NULL)CHECK(!strcmp(summary.embeddingModelId,"synthetic.embedding"));
    else CHECK(summary.embeddingModelId[0]=='\0');
    if(!strcmp(mode,"input-copy"))CHECK(!strcmp(summary.query,"workshop"));
    UmiAiWorkspaceEvidence row;
    for(size_t i=0U;i<summary.count;++i){
        CHECK(UmiAiSearchSnapshotAt(snapshot,i,&row)==UMI_STATUS_OK);
        CHECK(!strcmp(row.source.collectionId,"main") && strcmp(row.source.id,"secret") && row.score>0.0);
        if(!strcmp(mode,"lexical") || !strcmp(mode,"collection-scope") || !strcmp(mode,"unicode")){
            CHECK(row.source.id[0]==(char)('a'+(int)i) && row.source.id[1]=='\0');
        }
    }

    if(!strcmp(mode,"empty")){
        UmiAiSearchSelection emptySelection;unsigned char saved[sizeof emptySelection];size_t index=0U;
        memset(&emptySelection,0xa5,sizeof emptySelection);memcpy(saved,&emptySelection,sizeof emptySelection);
        CHECK(UmiAiSearchSnapshotSelect(snapshot,&index,1U,&emptySelection)==UMI_STATUS_NOT_FOUND);
        CHECK(!memcmp(saved,&emptySelection,sizeof emptySelection));
    }
    if(!strcmp(mode,"rerank"))CHECK(UmiAiSearchSnapshotAt(snapshot,0U,&row)==UMI_STATUS_OK && !strcmp(row.source.id,"e"));
    if(!strcmp(mode,"reentrant"))CHECK(fixture.calls==5U && fixture.nested==UMI_STATUS_BUSY);
    if(!strcmp(mode,"changed-source")){
        CHECK(UmiAiWorkspacePutSource(fixture.workspace,"a","main","Changed","different passage",20U)==UMI_STATUS_OK);
        CHECK(UmiAiSearchSnapshotAt(snapshot,0U,&row)==UMI_STATUS_OK && !strcmp(row.source.title,"a") && row.source.firstLine==7U);
    }
    if(!strcmp(mode,"source-copy")){
        CHECK(UmiAiSearchSnapshotAt(snapshot,0U,&row)==UMI_STATUS_OK);strcpy(row.source.text,"modified copy");
        CHECK(UmiAiSearchSnapshotAt(snapshot,0U,&row)==UMI_STATUS_OK && strstr(row.source.text,"workshop"));
    }
    if(!strcmp(mode,"summary-refusal")){
        UmiAiSearchSummary target;unsigned char saved[sizeof target];memset(&target,0xa5,sizeof target);memcpy(saved,&target,sizeof target);
        CHECK(UmiAiSearchSnapshotInspect(NULL,&target)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(saved,&target,sizeof target));
        CHECK(UmiAiSearchSnapshotInspect(snapshot,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    if(!strcmp(mode,"row-refusal")){
        unsigned char saved[sizeof row];memset(&row,0xa5,sizeof row);memcpy(saved,&row,sizeof row);
        CHECK(UmiAiSearchSnapshotAt(snapshot,summary.count,&row)==UMI_STATUS_NOT_FOUND && !memcmp(saved,&row,sizeof row));
        CHECK(UmiAiSearchSnapshotAt(NULL,0U,&row)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(saved,&row,sizeof row));
        CHECK(UmiAiSearchSnapshotAt(snapshot,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    if(!strncmp(mode,"select-",7U) || !strcmp(mode,"prepare") || !strncmp(mode,"stale-",6U)){
        UmiAiSearchSelection selection;unsigned char saved[sizeof selection];
        memset(&selection,0xa5,sizeof selection);memcpy(saved,&selection,sizeof selection);
        size_t indices[]={2U,0U,3U,1U,4U};size_t count=2U;UmiStatus selectionStatus=UMI_STATUS_OK;
        if(!strcmp(mode,"select-maximum"))count=4U;
        if(!strcmp(mode,"select-duplicate")){indices[1]=2U;selectionStatus=UMI_STATUS_ALREADY_EXISTS;}
        if(!strcmp(mode,"select-late-invalid")){indices[1]=summary.count;selectionStatus=UMI_STATUS_NOT_FOUND;}
        if(!strcmp(mode,"select-zero")){count=0U;selectionStatus=UMI_STATUS_INVALID_ARGUMENT;}
        if(!strcmp(mode,"select-overflow")){count=5U;selectionStatus=UMI_STATUS_INVALID_ARGUMENT;}
        if(!strcmp(mode,"select-null")){
            CHECK(UmiAiSearchSnapshotSelect(NULL,indices,2U,&selection)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiSearchSnapshotSelect(snapshot,NULL,2U,&selection)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiSearchSnapshotSelect(snapshot,indices,2U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(saved,&selection,sizeof selection));
        }else{
            const size_t *input=indices;
            if(!strcmp(mode,"select-alias")){selection.count=2U;input=&selection.count;count=1U;}
            CHECK(UmiAiSearchSnapshotSelect(snapshot,input,count,&selection)==selectionStatus);
            if(selectionStatus!=UMI_STATUS_OK)CHECK(!memcmp(saved,&selection,sizeof selection));
            else{
                CHECK(selection.count==count && selection.corpusRevision==summary.corpusRevision);
                CHECK(!strcmp(selection.collection.id,"main") && !strcmp(selection.query,summary.query));
                CHECK(!strcmp(selection.evidence[0].source.id,"c"));
                if(count>1U)CHECK(!strcmp(selection.evidence[1].source.id,"a"));
                if(count==4U)CHECK(!strcmp(selection.evidence[2].source.id,"d") && !strcmp(selection.evidence[3].source.id,"b"));
                if(!strcmp(mode,"prepare") || !strncmp(mode,"stale-",6U)){
                    UmiStatus prepareStatus=UMI_STATUS_OK;
                    if(!strcmp(mode,"stale-selected-source")){
                        CHECK(UmiAiWorkspacePutSource(fixture.workspace,"c","main","Changed","new passage",1U)==UMI_STATUS_OK);
                        prepareStatus=UMI_STATUS_BUSY;
                    }
                    if(!strcmp(mode,"stale-other-source")){
                        CHECK(UmiAiWorkspacePutSource(fixture.workspace,"secret","other","Changed","new passage",1U)==UMI_STATUS_OK);
                        prepareStatus=UMI_STATUS_BUSY;
                    }
                    CHECK(UmiAiWorkspacePrepareEvidence(fixture.workspace,"draft","umicom.extractive-preview","extractive-preview",
                        selection.collection.id,selection.query,"learner",128U,selection.corpusRevision,
                        selection.evidence,selection.count)==prepareStatus);
                    CHECK(UmiAiWorkspaceSnapshotRead(fixture.workspace,&after)==UMI_STATUS_OK);
                    CHECK(after.jobCount==(prepareStatus==UMI_STATUS_OK?1U:0U));
                    if(prepareStatus==UMI_STATUS_OK){
                        UmiAiWorkspaceJob *job=malloc(sizeof *job);CHECK(job!=NULL);
                        CHECK(UmiAiWorkspaceJobFind(fixture.workspace,"draft",job)==UMI_STATUS_OK &&
                            job->state==UMI_AI_WORKSPACE_REVIEW && job->evidenceCount==2U &&
                            !strcmp(job->evidence[0].source.id,"c") && !strcmp(job->evidence[1].source.id,"a"));
                        free(job);
                    }
                }
            }
        }
    }
    UmiAiSearchSnapshotDestroy(snapshot);UmiAiSearchSnapshotDestroy(NULL);
    if(fixture.workspace!=NULL)Close();
    return 0;
}
