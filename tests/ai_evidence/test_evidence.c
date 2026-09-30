/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_evidence/test_evidence.c
 * PURPOSE:
 *   Native evidence, selection, lifetime and persistence regression cases.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native evidence, selection, lifetime and persistence regression cases.
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include <math.h>
#include <stdint.h>

static int Selection(const char *name)
{
    Fixture f; CHECK(OpenFixture(&f,NULL)==0);
    UmiStatus expected=UMI_STATUS_INVALID_ARGUMENT;
    size_t count=2U;
    if(strcmp(name,"selection_order")==0) {
        UmiAiWorkspaceEvidence swap=f.evidence[0];f.evidence[0]=f.evidence[1];f.evidence[1]=swap;
        expected=UMI_STATUS_OK;
    } else if(strcmp(name,"selection_duplicates")==0) f.evidence[1]=f.evidence[0];
    else if(strcmp(name,"selection_zero")==0) count=0U;
    else if(strcmp(name,"selection_capacity")==0) count=UMI_AI_WORKSPACE_MAX_EVIDENCE+1U;
    else if(strcmp(name,"selection_nan")==0) f.evidence[0].score=NAN;
    else if(strcmp(name,"selection_infinite")==0) f.evidence[0].score=INFINITY;
    else if(strcmp(name,"selection_negative")==0) f.evidence[0].score=-0.1;
    else if(strcmp(name,"selection_zero_score")==0) {f.evidence[0].score=0.0;expected=UMI_STATUS_OK;}
    else if(strcmp(name,"selection_forged_text")==0) {strcpy(f.evidence[0].source.text,"Forged");expected=UMI_STATUS_BUSY;}
    else if(strcmp(name,"selection_forged_title")==0) {strcpy(f.evidence[0].source.title,"Forged");expected=UMI_STATUS_BUSY;}
    else if(strcmp(name,"selection_forged_lines")==0) {++f.evidence[0].source.lastLine;expected=UMI_STATUS_BUSY;}
    else if(strcmp(name,"selection_forged_revision")==0) {++f.evidence[0].source.revision;expected=UMI_STATUS_BUSY;}
    else if(strcmp(name,"selection_wrong_collection")==0) strcpy(f.evidence[0].source.collectionId,"other");
    else if(strcmp(name,"selection_unterminated")==0) memset(f.evidence[0].source.title,'x',sizeof f.evidence[0].source.title);
    else if(strcmp(name,"selection_stale")==0) {
        OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","Saving","Changed",10U)); expected=UMI_STATUS_BUSY;
    } else if(strcmp(name,"selection_removed")==0) {
        OK(UmiAiWorkspaceRemoveSource(f.workspace,"save"));
        OK(UmiAiWorkspaceSnapshotRead(f.workspace,&f.snapshot)); expected=UMI_STATUS_BUSY;
    } else return 1;
    UmiAiWorkspaceSnapshot before,after;OK(UmiAiWorkspaceSnapshotRead(f.workspace,&before));
    CHECK(PrepareFixture(&f,"job",f.evidence,count)==expected);
    OK(UmiAiWorkspaceSnapshotRead(f.workspace,&after));
    if(expected==UMI_STATUS_OK) {
        CHECK(after.jobCount==before.jobCount+1U);
        UmiAiWorkspaceJob job;OK(UmiAiWorkspaceJobFind(f.workspace,"job",&job));
        CHECK(job.state==UMI_AI_WORKSPACE_REVIEW && job.response.text[0]=='\0');
        CHECK(strcmp(job.evidence[0].source.id,f.evidence[0].source.id)==0);
    } else CHECK(after.revision==before.revision && after.jobCount==before.jobCount);
    CloseFixture(&f);return 0;
}
static int Duplicate(void)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);
    OK(PrepareFixture(&f,"job",f.evidence,2U));
    UmiAiWorkspaceSnapshot before,after;OK(UmiAiWorkspaceSnapshotRead(f.workspace,&before));
    f.evidence[0].score=0.125; /* Transient score is NOT durable request identity. */
    OK(PrepareFixture(&f,"job",f.evidence,2U));
    OK(UmiAiWorkspaceSnapshotRead(f.workspace,&after));CHECK(before.revision==after.revision);
    UmiAiWorkspaceEvidence swap=f.evidence[0];f.evidence[0]=f.evidence[1];f.evidence[1]=swap;
    CHECK(PrepareFixture(&f,"job",f.evidence,2U)==UMI_STATUS_ALREADY_EXISTS);
    CloseFixture(&f);return 0;
}
static int DefaultAndHybrid(void)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);
    UmiAiEmbedding save={0},check={0},query={0};
    save.dimension=2U;save.values[0]=1.0;check.dimension=2U;check.values[1]=1.0;
    query=check;
    OK(UmiAiWorkspaceSetEmbedding(f.workspace,"save",f.evidence[0].source.revision,"synthetic-test",&save));
    OK(UmiAiWorkspaceSetEmbedding(f.workspace,"check",f.evidence[1].source.revision,"synthetic-test",&check));
    UmiAiWorkspaceEvidence selected[4];size_t count=0U;
    OK(UmiAiWorkspaceSearch(f.workspace,"notes","absentterm","synthetic-test",&query,NULL,selected,4U,&count));
    CHECK(count==1U && strcmp(selected[0].source.id,"check")==0);
    OK(PrepareFixture(&f,"hybrid",selected,count));
    UmiAiWorkspaceJob job;OK(UmiAiWorkspaceJobFind(f.workspace,"hybrid",&job));
    CHECK(job.evidenceCount==1U && strcmp(job.evidence[0].source.id,"check")==0);
    OK(UmiAiWorkspacePrepare(f.workspace,"lexical",UMI_AI_WORKSPACE_GROUNDED_DRAFT,
        "umicom.extractive-preview","extractive-preview","notes","Save checkpoint","writer",256U));
    OK(UmiAiWorkspaceJobFind(f.workspace,"lexical",&job));
    CHECK(job.evidenceCount==1U && strcmp(job.evidence[0].source.id,"save")==0);
    CloseFixture(&f);return 0;
}
static int Capture(const char *name)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);
    if(strcmp(name,"capture_pending")==0) OK(PrepareFixture(&f,"job",f.evidence,2U));
    else CHECK(RunFixture(&f)==0);
    bool change=strcmp(name,"capture_changed")==0 || strcmp(name,"capture_snapshot")==0;
    if(change) OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","New title","new text",50U));
    if(strcmp(name,"capture_removed")==0) OK(UmiAiWorkspaceRemoveSource(f.workspace,"save"));
    if(strcmp(name,"capture_unrelated")==0) OK(UmiAiWorkspacePutSource(f.workspace,"extra","notes","Extra","unrelated",1U));
    UmiAiEvidenceReview *review=NULL;OK(UmiAiEvidenceCapture(f.workspace,"job",&review));
    UmiAiEvidenceSummary summary;OK(UmiAiEvidenceSummaryRead(review,&summary));
    CHECK(summary.sourceCount==2U);
    CHECK(summary.hasResponse==(strcmp(name,"capture_pending")!=0));
    CHECK(summary.changedSources==(change?1U:0U));
    CHECK(summary.removedSources==(strcmp(name,"capture_removed")==0?1U:0U));
    if(strcmp(name,"capture_unrelated")==0) CHECK(summary.corpusChanged && summary.changedSources==0U);
    if(summary.hasResponse) CHECK(summary.referenceBindingsValid && summary.validReferences==2U);
    if(strcmp(name,"capture_snapshot")==0) {
        OK(UmiAiWorkspaceRemoveSource(f.workspace,"check"));
        UmiAiEvidenceSummary after;OK(UmiAiEvidenceSummaryRead(review,&after));
        CHECK(memcmp(&summary,&after,sizeof after)==0);
    }
    CloseFixture(&f);
    UmiAiEvidenceSourceCheck source;OK(UmiAiEvidenceSourceAt(review,0U,&source));
    CHECK(strcmp(source.frozen.text,"Save checkpoint.\nKeep the draft.\n")==0);
    if(change) CHECK(strcmp(source.current.text,"new text")==0);
    UmiAiWorkspaceJob job;OK(UmiAiEvidenceJobRead(review,&job));CHECK(strcmp(job.id,"job")==0);
    char line[100];OK(UmiAiEvidenceCopyLines(review,0U,10U,10U,line,sizeof line,NULL));CHECK(strcmp(line,"Save checkpoint.\n")==0);
    UmiAiEvidenceDestroy(review);return 0;
}
static int Lines(const char *name)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);
    const char *text=strcmp(name,"lines_unicode")==0?"أهلاً\n世界\nlast":"one\n\ntwo\n";
    uint32_t start=strcmp(name,"lines_high_number")==0?UINT32_MAX-2U:10U;
    OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","Saving",text,start));
    OK(UmiAiWorkspaceSourceAt(f.workspace,0U,&f.evidence[0].source));
    OK(UmiAiWorkspaceSnapshotRead(f.workspace,&f.snapshot));
    OK(PrepareFixture(&f,"job",f.evidence,1U));
    UmiAiEvidenceReview *review=NULL;OK(UmiAiEvidenceCapture(f.workspace,"job",&review));
    char buffer[200];size_t bytes=999U;
    memset(buffer,'Z',sizeof buffer);
    CHECK(UmiAiEvidenceCopyLines(review,0U,start,start,buffer,1U,&bytes)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(buffer[0]=='Z' && bytes==999U);
    CHECK(UmiAiEvidenceCopyLines(review,0U,start-1U,start,buffer,sizeof buffer,&bytes)==UMI_STATUS_NOT_FOUND);
    CHECK(buffer[0]=='Z' && bytes==999U);
    OK(UmiAiEvidenceCopyLines(review,0U,start,start+2U,buffer,sizeof buffer,&bytes));
    CHECK(bytes==strlen(text) && strcmp(buffer,text)==0);
    OK(UmiAiEvidenceCopyLines(review,0U,start+1U,start+1U,buffer,sizeof buffer,NULL));
    CHECK(strcmp(buffer,strcmp(name,"lines_unicode")==0?"世界\n":"\n")==0);
    OK(UmiAiEvidenceCopyLines(review,0U,start+2U,start+2U,buffer,sizeof buffer,NULL));
    CHECK(strcmp(buffer,strcmp(name,"lines_unicode")==0?"last":"two\n")==0);
    UmiAiEvidenceDestroy(review);CloseFixture(&f);return 0;
}
static int Scan(void)
{
    struct {const char *text;size_t count,valid,bad,unknown;} cases[]={
        {"",0,0,0,0},{"no reference",0,0,0,0},{"[S1]",1,1,0,0},
        {"[S1][S2]",2,2,0,0},{"[S01]",1,1,0,0},{"[S0]",1,0,1,0},
        {"[S99]",1,0,0,1},{"[S123]",1,0,1,0},{"[S]",1,0,1,0},
        {"[S1",1,0,1,0},{"[S",1,0,1,0},{"[",0,0,0,0},
        {"[S[S1]",2,1,1,0},{"`[S1]`",1,1,0,0},{"[s1]",0,0,0,0},
        {"[S-1] [S 1]",2,0,2,0},{"أهلاً [S1]",1,1,0,0}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i){
        UmiAiEvidenceReferences refs;OK(UmiAiEvidenceScanReferences(cases[i].text,2U,&refs));
        CHECK(refs.count==cases[i].count&&refs.validCount==cases[i].valid&&refs.malformedCount==cases[i].bad&&refs.unknownCount==cases[i].unknown);
        for(size_t j=0;j<refs.count;++j) CHECK(refs.items[j].byteOffset+refs.items[j].byteLength<=strlen(cases[i].text));
    }
    UmiAiEvidenceReferences refs;OK(UmiAiEvidenceScanReferences("[S01]",1U,&refs));CHECK(!refs.items[0].canonicalSpelling);
    OK(UmiAiEvidenceScanReferences("[S1]",0U,&refs));CHECK(refs.unknownCount==1U);
    return 0;
}
static int ScanBounds(void)
{
    char text[UMI_AI_TEXT_CAPACITY];UmiAiEvidenceReferences refs,original;
    memset(&original,0x5A,sizeof original);refs=original;
    memset(text,'x',sizeof text);
    CHECK(UmiAiEvidenceScanReferences(text,1U,&refs)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&refs,&original,sizeof refs)==0);
    text[0]=(char)0xC0;text[1]=(char)0x80;text[2]='\0';
    CHECK(UmiAiEvidenceScanReferences(text,1U,&refs)==UMI_STATUS_INVALID_ARGUMENT);
    text[0]='\0';for(size_t i=0;i<UMI_AI_EVIDENCE_MAX_REFERENCES+1U;++i)strcat(text,"[S1]");
    CHECK(UmiAiEvidenceScanReferences(text,1U,&refs)==UMI_STATUS_CAPACITY_EXCEEDED && memcmp(&refs,&original,sizeof refs)==0);
    text[UMI_AI_EVIDENCE_MAX_REFERENCES*4U]='\0';
    OK(UmiAiEvidenceScanReferences(text,1U,&refs));CHECK(refs.validCount==UMI_AI_EVIDENCE_MAX_REFERENCES);
    return 0;
}
static int Mutation(void)
{
    uint32_t random=91U;const char alphabet[]="[S012349] x\n";
    for(size_t run=0;run<5000U;++run) {
        char text[600];size_t length=run%(sizeof text-1U);
        for(size_t j=0;j<length;++j){random=random*1664525U+1013904223U;text[j]=alphabet[random%(sizeof alphabet-1U)];}
        text[length]='\0';UmiAiEvidenceReferences refs;UmiStatus status=UmiAiEvidenceScanReferences(text,4U,&refs);
        CHECK(status==UMI_STATUS_OK||status==UMI_STATUS_CAPACITY_EXCEEDED);
        if(status==UMI_STATUS_OK){
            CHECK(refs.count==refs.validCount+refs.malformedCount+refs.unknownCount);
            for(size_t j=0;j<refs.count;++j){CHECK(refs.items[j].byteOffset+refs.items[j].byteLength<=length);
                if(j)CHECK(refs.items[j].byteOffset>refs.items[j-1U].byteOffset);}
        }
    }return 0;
}
static int AtomicRequest(void)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);OK(PrepareFixture(&f,"job",f.evidence,2U));
    UmiAiWorkspaceJob *job=calloc(1U,sizeof *job);UmiAiRequest *request=malloc(sizeof *request),*before=malloc(sizeof *before);
    CHECK(job&&request&&before);OK(UmiAiWorkspaceJobFind(f.workspace,"job",job));
    memset(request,0x5A,sizeof *request);memcpy(before,request,sizeof *request);
    job->evidence[1].source.firstLine=0U;
    CHECK(UmiAiWorkspaceBuildRequest(job,request)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before,request,sizeof *request)==0);
    job->evidence[1].source.firstLine=30U;OK(UmiAiWorkspaceBuildRequest(job,request));
    CHECK(request->message_count==4U && request->allow_tools==0);
    free(job);free(request);free(before);CloseFixture(&f);return 0;
}
static int Format(void)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);CHECK(RunFixture(&f)==0);
    UmiAiEvidenceReview *review=NULL;OK(UmiAiEvidenceCapture(f.workspace,"job",&review));
    char tiny[10];memset(tiny,'Z',sizeof tiny);size_t bytes=111U;
    CHECK(UmiAiEvidenceFormat(review,tiny,sizeof tiny,&bytes)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(tiny[0]=='Z'&&bytes==111U);
    char *report=malloc(UMI_AI_EVIDENCE_REPORT_CAPACITY);CHECK(report);
    OK(UmiAiEvidenceFormat(review,report,UMI_AI_EVIDENCE_REPORT_CAPACITY,&bytes));
    CHECK(bytes==strlen(report)&&strstr(report,"not proof of truth")&&strstr(report,"[S1] save")!=NULL);
    free(report);UmiAiEvidenceDestroy(review);CloseFixture(&f);return 0;
}
static int Storage(const char *path)
{
    (void)remove(path);Fixture f;int opened=OpenFixture(&f,path);if(opened)return opened;
    CHECK(RunFixture(&f)==0);
    OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","Saving","Changed since generation",10U));
    UmiAiWorkspaceDestroy(f.workspace);f.workspace=NULL;
    OK(UmiAiWorkspaceCreate(f.server,&f.runtime,"evidence-test",&f.workspace));
    UmiAiEvidenceReview *review=NULL;OK(UmiAiEvidenceCapture(f.workspace,"job",&review));
    UmiAiEvidenceSummary summary;OK(UmiAiEvidenceSummaryRead(review,&summary));CHECK(summary.changedSources==1U&&summary.hasResponse);
    char line[64];OK(UmiAiEvidenceCopyLines(review,0U,10U,10U,line,sizeof line,NULL));CHECK(strcmp(line,"Save checkpoint.\n")==0);
    UmiAiEvidenceDestroy(review);CloseFixture(&f);CHECK(remove(path)==0);return 0;
}
static int ReviewGate(void)
{
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);OK(PrepareFixture(&f,"job",f.evidence,2U));
    CHECK(UmiAiWorkspaceRun(f.workspace,"job",NULL)==UMI_STATUS_INVALID_STATE);
    OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","Saving","Changed",10U));
    CHECK(UmiAiWorkspaceReview(f.workspace,"job","reviewer",true)==UMI_STATUS_BUSY);
    CloseFixture(&f);return 0;
}
int main(int argc,char **argv)
{
    if(argc<2)return 2;
    const char *name=argv[1];
    if(strncmp(name,"selection_",10U)==0)return Selection(name);
    if(strncmp(name,"capture_",8U)==0)return Capture(name);
    if(strncmp(name,"lines_",6U)==0)return Lines(name);
    if(strcmp(name,"duplicate_identity")==0)return Duplicate();
    if(strcmp(name,"hybrid_and_default")==0)return DefaultAndHybrid();
    if(strcmp(name,"reference_grammar")==0)return Scan();
    if(strcmp(name,"reference_bounds")==0)return ScanBounds();
    if(strcmp(name,"reference_mutations")==0)return Mutation();
    if(strcmp(name,"atomic_request")==0)return AtomicRequest();
    if(strcmp(name,"format")==0)return Format();
    if(strcmp(name,"sqlite_reload")==0 && argc==3)return Storage(argv[2]);
    if(strcmp(name,"review_gate")==0)return ReviewGate();
    return 2;
}
