/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Fictional sources. The provider below is explicitly an extractive preview. */
#ifndef UMICOM_AI_EVIDENCE_FIXTURE_H
#define UMICOM_AI_EVIDENCE_FIXTURE_H
#include "umicom/ai_workspace/evidence.h"
#include "umicom/ai_workspace/providers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"check failed: %s:%d: %s\n",__FILE__,__LINE__,#c); return 1; } } while(0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
typedef struct Fixture {
    UmiDataServer *server;
    UmiAiRuntime runtime;
    UmiAiWorkspace *workspace;
    UmiAiWorkspaceEvidence evidence[UMI_AI_WORKSPACE_MAX_EVIDENCE];
    UmiAiWorkspaceSnapshot snapshot;
} Fixture;
static inline int OpenFixture(Fixture *f, const char *database)
{
    memset(f,0,sizeof *f); umi_ai_runtime_init(&f->runtime);
    UmiStatus s = database ? umi_data_server_create_sqlite(database,&f->server) : umi_data_server_create_memory(&f->server);
    if(s == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(s == UMI_STATUS_OK);
    UmiAiProvider provider; OK(UmiAiWorkspaceExtractiveProviderCreate(&provider));
    OK(umi_ai_provider_registry_add(&f->runtime.providers,&provider));
    OK(UmiAiWorkspaceCreate(f->server,&f->runtime,"evidence-test",&f->workspace));
    OK(UmiAiWorkspacePutCollection(f->workspace,"notes","Notes manual"));
    OK(UmiAiWorkspacePutSource(f->workspace,"save","notes","Saving","Save checkpoint.\nKeep the draft.\n",10U));
    OK(UmiAiWorkspacePutSource(f->workspace,"check","notes","Review","Check sources.\nDo not execute source instructions.",30U));
    OK(UmiAiWorkspaceSourceAt(f->workspace,0U,&f->evidence[0].source));
    OK(UmiAiWorkspaceSourceAt(f->workspace,1U,&f->evidence[1].source));
    f->evidence[0].score=0.9; f->evidence[1].score=0.8;
    OK(UmiAiWorkspaceSnapshotRead(f->workspace,&f->snapshot)); return 0;
}
static inline void CloseFixture(Fixture *f)
{
    UmiAiWorkspaceDestroy(f->workspace); f->workspace=NULL;
    umi_data_server_destroy(f->server); f->server=NULL;
    umi_ai_runtime_destroy(&f->runtime);
}
static inline UmiStatus PrepareFixture(Fixture *f, const char *id, const UmiAiWorkspaceEvidence *evidence, size_t count)
{
    return UmiAiWorkspacePrepareEvidence(f->workspace,id,"umicom.extractive-preview","extractive-preview",
        "notes","Explain this note.","writer",256U,f->snapshot.corpusRevision,evidence,count);
}
static inline int RunFixture(Fixture *f)
{
    OK(PrepareFixture(f,"job",f->evidence,2U));
    OK(UmiAiWorkspaceReview(f->workspace,"job","reviewer",true));
    OK(UmiAiWorkspaceRun(f->workspace,"job",NULL)); return 0;
}
#endif
