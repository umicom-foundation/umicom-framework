/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_workspace/test_workspace.c
 * PURPOSE: Exercise production CSV, review, policy and transactional storage without starting applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/enterprise_workspace/practice.h"
#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static struct Fixture {
    UmiDataServer *data;
    UmiEnterprisePracticeAccess *access;
    UmiEnterpriseWorkspace *workspace, *other;
    UmiDataServer *second;
} f;
static const char stock[] = "item_id,label,quantity\nnotebooks,Notebooks,12\npencils,Pencils,8\n";
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) REQUIRE((x) == UMI_STATUS_OK)
#define STATUS(x,s) REQUIRE((x) == (s))
#define ACT(i) UmiEnterprisePracticeActor(i)
static int Setup(const char *path)
{
    OK(UmiEnterprisePracticeAccessCreate(&f.access));
    if (path != NULL) { (void)remove(path); OK(umi_data_server_create_sqlite(path, &f.data)); }
    else OK(umi_data_server_create_memory(&f.data));
    OK(UmiEnterpriseWorkspaceOpen(f.data, UmiEnterprisePracticeAuthorisation(f.access), &f.workspace));
    return 0;
}
static int Seed(void)
{
    OK(UmiEnterpriseWorkspaceSetRecipe(f.workspace, ACT(4), "stock.csv", true));
    OK(UmiEnterpriseWorkspaceCreateDataset(f.workspace, ACT(1), "supplies", "Workshop supplies"));
    return 0;
}
static int Prepare(const char *id, const char *csv)
{
    OK(UmiEnterpriseWorkspacePrepare(f.workspace, ACT(1), id, "supplies", "stock.csv", csv, strlen(csv), NULL));
    return 0;
}
static int Approve(const char *id)
{
    OK(UmiEnterpriseWorkspaceReview(f.workspace, ACT(2), id, true, "Checked against the stock sheet"));
    return 0;
}
static int Apply(const char *id)
{
    OK(UmiEnterpriseWorkspaceExecute(f.workspace, ACT(3), id)); return 0;
}
static uint64_t Revision(void)
{
    UmiEnterpriseSnapshot snapshot;
    if (UmiEnterpriseWorkspaceSnapshot(f.workspace, &snapshot) != UMI_STATUS_OK) return UINT64_MAX;
    return snapshot.revision;
}
static int Parser(const char *name)
{
    static const struct { const char *name, *csv; bool good; } cases[] = {
        {"csv_lf", "item_id,label,quantity\na,Alpha,3\n", true},
        {"csv_crlf", "item_id,label,quantity\r\na,Alpha,3\r\n", true},
        {"csv_bom", "\xef\xbb\xbfitem_id,label,quantity\na,Alpha,3", true},
        {"csv_quoted", "item_id,label,quantity\na,\"A, \"\"quoted\"\" label\",3\n", true},
        {"csv_multiline", "item_id,label,quantity\na,\"Line one\nLine two\",3", true},
        {"csv_unicode", "item_id,label,quantity\na,Caf\xc3\xa9 \xf0\x9f\x93\x9a,3", true},
        {"csv_zero", "item_id,label,quantity\na,Alpha,0", true},
        {"csv_maximum", "item_id,label,quantity\na,Alpha,9223372036854775807", true},
        {"csv_overflow", "item_id,label,quantity\na,Alpha,9223372036854775808", false},
        {"csv_negative", "item_id,label,quantity\na,Alpha,-1", false},
        {"csv_plus", "item_id,label,quantity\na,Alpha,+1", false},
        {"csv_decimal", "item_id,label,quantity\na,Alpha,1.2", false},
        {"csv_space", "item_id,label,quantity\na,Alpha, 3", false},
        {"csv_header", "id,label,quantity\na,Alpha,3", false},
        {"csv_extra", "item_id,label,quantity\na,Alpha,3,4", false},
        {"csv_missing", "item_id,label,quantity\na,Alpha", false},
        {"csv_duplicate", "item_id,label,quantity\na,Alpha,3\na,Beta,4", false},
        {"csv_bad_id", "item_id,label,quantity\n../x,Alpha,3", false},
        {"csv_empty_label", "item_id,label,quantity\na,,3", false},
        {"csv_unclosed", "item_id,label,quantity\na,\"Alpha,3", false},
        {"csv_quote_tail", "item_id,label,quantity\na,\"Alpha\"x,3", false},
        {"csv_bare_quote", "item_id,label,quantity\na,Al\"pha,3", false},
        {"csv_bare_cr", "item_id,label,quantity\ra,Alpha,3", false},
        {"csv_empty", "item_id,label,quantity\n", false},
        {"csv_bad_utf8", "item_id,label,quantity\na,\xc0\xaf,3", false},
        {"csv_surrogate", "item_id,label,quantity\na,\xed\xa0\x80,3", false},
        {"csv_control", "item_id,label,quantity\na,A\x01,3", false},
        {"csv_blank_record", "item_id,label,quantity\n\na,Alpha,3", false}
    };
    UmiEnterpriseRow rows[UMI_ENTERPRISE_MAX_IMPORT_ROWS]; size_t count = 0U;
    UmiEnterpriseIssue issue;
    for (size_t i = 0U; i < sizeof(cases)/sizeof(cases[0]); ++i) if (strcmp(name,cases[i].name)==0) {
        UmiStatus s = EwsParse(cases[i].csv, strlen(cases[i].csv), 0U, rows, &count, &issue);
        REQUIRE((s == UMI_STATUS_OK) == cases[i].good);
        if (cases[i].good) REQUIRE(count == 1U);
        else REQUIRE(issue.message[0] != '\0');
        return 0;
    }
    if (strcmp(name,"csv_embedded_nul")==0) {
        const char value[]="item_id,label,quantity\na,A\0B,3";
        REQUIRE(EwsParse(value,sizeof(value)-1U,0U,rows,&count,&issue)!=UMI_STATUS_OK);
    } else if (strcmp(name,"csv_label_boundary")==0) {
        char csv[400]; char label[194]; memset(label,'X',191);label[191]='\0';
        (void)snprintf(csv,sizeof(csv),"item_id,label,quantity\na,%s,1",label);
        OK(EwsParse(csv,strlen(csv),0U,rows,&count,&issue));
        label[191]='X';label[192]='\0';(void)snprintf(csv,sizeof(csv),"item_id,label,quantity\na,%s,1",label);
        REQUIRE(EwsParse(csv,strlen(csv),0U,rows,&count,&issue)!=UMI_STATUS_OK);
    } else if (strcmp(name,"csv_row_boundary")==0) {
        char csv[2000]="item_id,label,quantity\n"; size_t used=strlen(csv);
        for(unsigned i=0U;i<32U;++i) used+=(size_t)snprintf(csv+used,sizeof(csv)-used,"r%u,Row,1\n",i);
        OK(EwsParse(csv,used,0U,rows,&count,&issue));REQUIRE(count==32U);
        (void)strcat(csv,"extra,Row,1\n");REQUIRE(EwsParse(csv,strlen(csv),0U,rows,&count,&issue)!=UMI_STATUS_OK);
    } else if (strcmp(name,"csv_uppercase_recipe")==0) {
        const char *csv="item_id,label,quantity\na,Caf\xc3\xa9 notes,2";
        OK(EwsParse(csv,strlen(csv),1U,rows,&count,&issue));REQUIRE(strcmp(rows[0].label,"CAF\xc3\xa9 NOTES")==0);
    } else return 1;
    return 0;
}
static int Workflow(const char *name)
{
    UmiEnterpriseSnapshot snapshot;
    UmiEnterpriseJob job;
    UmiEnterpriseDataset dataset;
    UmiEnterpriseRow row;
    REQUIRE(Setup(NULL)==0);
    if(strcmp(name,"empty_open")==0) {
        OK(UmiEnterpriseWorkspaceSnapshot(f.workspace,&snapshot));REQUIRE(snapshot.revision==0U && snapshot.datasetCount==0U);
        REQUIRE(umi_data_server_count(f.data)==0U);return 0;
    }
    REQUIRE(Seed()==0);
    if(strcmp(name,"viewer_denied")==0) {STATUS(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(0),"private","Private"),UMI_STATUS_PERMISSION_DENIED);return 0;}
    if(strcmp(name,"dataset_idempotent")==0) {uint64_t v=Revision();OK(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),"supplies","Workshop supplies"));REQUIRE(Revision()==v);STATUS(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),"supplies","Different"),UMI_STATUS_ALREADY_EXISTS);return 0;}
    if(strcmp(name,"dataset_capacity")==0) {for(unsigned i=1U;i<8U;++i){char id[24];snprintf(id,sizeof(id),"d%u",i);OK(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),id,id));} STATUS(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),"overflow","Overflow"),UMI_STATUS_CAPACITY_EXCEEDED);return 0;}

    if(strcmp(name,"row_capacity")==0) {
        for(unsigned batch=0U;batch<2U;++batch){char csv[3000]="item_id,label,quantity\n";size_t used=strlen(csv);char id[24];snprintf(id,sizeof(id),"batch%u",batch);
            for(unsigned i=0U;i<32U;++i)used+=(size_t)snprintf(csv+used,sizeof(csv)-used,"r%u,Row,1\n",batch*32U+i);
            REQUIRE(Prepare(id,csv)==0);REQUIRE(Approve(id)==0);REQUIRE(Apply(id)==0);}
        const char *csv="item_id,label,quantity\none_more,Row,1\n";
        STATUS(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"overflow","supplies","stock.csv",csv,strlen(csv),NULL),UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiEnterpriseWorkspaceDatasetAt(f.workspace,0U,&dataset));REQUIRE(dataset.rowCount==64U);return 0;
    }
    if(strcmp(name,"frozen_input_copy")==0) {
        char csv[sizeof(stock)];memcpy(csv,stock,sizeof(stock));REQUIRE(Prepare("copy",csv)==0);memset(csv,'X',sizeof(csv));
        REQUIRE(Approve("copy")==0);REQUIRE(Apply("copy")==0);OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",0U,&row));REQUIRE(row.quantity==12U);return 0;
    }
    if(strcmp(name,"invalid_arguments")==0){STATUS(UmiEnterpriseWorkspaceOpen(NULL,NULL,NULL),UMI_STATUS_INVALID_ARGUMENT);STATUS(UmiEnterpriseWorkspaceSnapshot(NULL,&snapshot),UMI_STATUS_INVALID_ARGUMENT);
        STATUS(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),"../invalid","Name"),UMI_STATUS_INVALID_ARGUMENT);
        STATUS(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),"valid",""),UMI_STATUS_INVALID_ARGUMENT);
        STATUS(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"bad","supplies","stock.csv",NULL,0U,NULL),UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(Revision()==2U);return 0;}
    if(strcmp(name,"nested_open")==0){OK(umi_data_server_begin(f.data));STATUS(UmiEnterpriseWorkspaceOpen(f.data,UmiEnterprisePracticeAuthorisation(f.access),&f.other),UMI_STATUS_BUSY);REQUIRE(f.other==NULL);REQUIRE(umi_data_server_in_transaction(f.data));OK(umi_data_server_rollback(f.data));return 0;}
    if(strcmp(name,"recipe_disabled")==0) {OK(UmiEnterpriseWorkspaceSetRecipe(f.workspace,ACT(4),"stock.csv",false));REQUIRE(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"job","supplies","stock.csv",stock,strlen(stock),NULL)!=UMI_STATUS_OK);return 0;}
    if(strcmp(name,"preview_read_only")==0) {UmiEnterprisePreview p;uint64_t v=Revision();OK(UmiEnterpriseWorkspacePreview(f.workspace,"supplies","stock.csv",stock,strlen(stock),&p,NULL));REQUIRE(p.insertCount==2U&&Revision()==v);return 0;}
    REQUIRE(Prepare("first",stock)==0);
    if(strcmp(name,"prepare_idempotent")==0) {uint64_t v=Revision();REQUIRE(Prepare("first",stock)==0);REQUIRE(Revision()==v);STATUS(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"first","supplies","stock.csv","x",1U,NULL),UMI_STATUS_ALREADY_EXISTS);return 0;}
    if(strcmp(name,"job_capacity")==0) {for(unsigned i=1U;i<16U;++i){char id[24];snprintf(id,sizeof(id),"job%u",i);REQUIRE(Prepare(id,stock)==0);}STATUS(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"overflow","supplies","stock.csv",stock,strlen(stock),NULL),UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
    if(strcmp(name,"unapproved_denied")==0) {STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first"),UMI_STATUS_INVALID_STATE);return 0;}
    if(strcmp(name,"self_review_denied")==0) {UmiEnterpriseActor self={"author","administrator"};STATUS(UmiEnterpriseWorkspaceReview(f.workspace,self,"first",true,"Checked"),UMI_STATUS_PERMISSION_DENIED);return 0;}
    if(strcmp(name,"reason_required")==0) {STATUS(UmiEnterpriseWorkspaceReview(f.workspace,ACT(2),"first",true,""),UMI_STATUS_INVALID_ARGUMENT);return 0;}
    if(strcmp(name,"reject_final")==0) {OK(UmiEnterpriseWorkspaceReview(f.workspace,ACT(2),"first",false,"Wrong quantities"));STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first"),UMI_STATUS_INVALID_STATE);return 0;}
    if(strcmp(name,"cancel_author")==0) {OK(UmiEnterpriseWorkspaceCancel(f.workspace,ACT(1),"first","New source available"));STATUS(UmiEnterpriseWorkspaceReview(f.workspace,ACT(2),"first",true,"Checked"),UMI_STATUS_INVALID_STATE);return 0;}
    if(strcmp(name,"cancel_other_denied")==0) {UmiEnterpriseActor other={"other","editor"};STATUS(UmiEnterpriseWorkspaceCancel(f.workspace,other,"first","Cancel"),UMI_STATUS_PERMISSION_DENIED);return 0;}
    if(strcmp(name,"stale_review")==0) {REQUIRE(Prepare("second",stock)==0);REQUIRE(Approve("first")==0);REQUIRE(Apply("first")==0);STATUS(UmiEnterpriseWorkspaceReview(f.workspace,ACT(2),"second",true,"Checked"),UMI_STATUS_BUSY);return 0;}
    REQUIRE(Approve("first")==0);
    if(strcmp(name,"stale_target")==0) {REQUIRE(Prepare("second",stock)==0);REQUIRE(Approve("second")==0);REQUIRE(Apply("first")==0);STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"second"),UMI_STATUS_BUSY);return 0;}
    if(strcmp(name,"paused_execution")==0) {OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),true));STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first"),UMI_STATUS_PERMISSION_DENIED);OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),false));REQUIRE(Apply("first")==0);return 0;}
    if(strcmp(name,"recipe_revoked")==0) {OK(UmiEnterpriseWorkspaceSetRecipe(f.workspace,ACT(4),"stock.csv",false));STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first"),UMI_STATUS_PERMISSION_DENIED);return 0;}
    if(strcmp(name,"cancel_approved")==0) {OK(UmiEnterpriseWorkspaceCancel(f.workspace,ACT(4),"first","Stopped before import"));STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first"),UMI_STATUS_INVALID_STATE);return 0;}
    if(strcmp(name,"nested_transaction")==0) {OK(umi_data_server_begin(f.data));STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first"),UMI_STATUS_BUSY);REQUIRE(umi_data_server_in_transaction(f.data));OK(umi_data_server_rollback(f.data));return 0;}
    REQUIRE(Apply("first")==0);
    OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",0U,&row));REQUIRE(row.quantity==12U&&strcmp(row.sourceJob,"first")==0);
    OK(UmiEnterpriseWorkspaceJobFind(f.workspace,"first",&job));REQUIRE(job.state==UMI_ENTERPRISE_JOB_APPLIED && strcmp(job.reviewer,"reviewer")==0 && strcmp(job.executor,"operator")==0);
    if(strcmp(name,"apply_workflow")==0) {OK(UmiEnterpriseWorkspaceSnapshot(f.workspace,&snapshot));REQUIRE(snapshot.rowCount==2U&&snapshot.appliedCount==1U&&snapshot.pendingCount==0U&&snapshot.auditCount==5U);return 0;}
    if(strcmp(name,"execute_idempotent")==0) {uint64_t v=Revision();REQUIRE(Apply("first")==0);REQUIRE(Revision()==v);return 0;}
    if(strcmp(name,"reload_applied")==0) {uint64_t v=Revision();OK(UmiEnterpriseWorkspaceReload(f.workspace));REQUIRE(Revision()==v);REQUIRE(Apply("first")==0);REQUIRE(Revision()==v);return 0;}
    if(strcmp(name,"update_provenance")==0) {const char *csv="item_id,label,quantity\nnotebooks,Notebooks,20\n";REQUIRE(Prepare("update",csv)==0);REQUIRE(Approve("update")==0);REQUIRE(Apply("update")==0);OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",0U,&row));REQUIRE(row.quantity==20U&&strcmp(row.sourceJob,"update")==0);OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",1U,&row));REQUIRE(strcmp(row.sourceJob,"first")==0);OK(UmiEnterpriseWorkspaceReload(f.workspace));return 0;}
    if(strcmp(name,"unchanged_preserves_generation")==0) {OK(UmiEnterpriseWorkspaceDatasetAt(f.workspace,0U,&dataset));uint64_t gen=dataset.generation;REQUIRE(Prepare("same",stock)==0);REQUIRE(Approve("same")==0);REQUIRE(Apply("same")==0);OK(UmiEnterpriseWorkspaceDatasetAt(f.workspace,0U,&dataset));REQUIRE(dataset.generation==gen);OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",0U,&row));REQUIRE(strcmp(row.sourceJob,"first")==0);return 0;}
    if(strcmp(name,"copied_projection")==0) {row.quantity=999U;OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",0U,&row));REQUIRE(row.quantity==12U);return 0;}
    if(strcmp(name,"stale_workspace")==0 || strcmp(name,"stale_noop")==0) {OK(UmiEnterpriseWorkspaceOpen(f.data,UmiEnterprisePracticeAuthorisation(f.access),&f.other));OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),true));if(strcmp(name,"stale_noop")==0) STATUS(UmiEnterpriseWorkspaceSetPaused(f.other,ACT(4),false),UMI_STATUS_BUSY);else STATUS(UmiEnterpriseWorkspaceCreateDataset(f.other,ACT(1),"new","New"),UMI_STATUS_BUSY);OK(UmiEnterpriseWorkspaceReload(f.other));return 0;}
    if(strcmp(name,"audit_capacity")==0) {while(Revision()<128U) OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),(Revision()%2U)!=0U));STATUS(UmiEnterpriseWorkspaceCreateDataset(f.workspace,ACT(1),"overflow","Overflow"),UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
    if(strcmp(name,"namespace_isolation")==0) {char value[80];OK(umi_data_server_set(f.data,"bank/keep","Untouched"));OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),true));OK(UmiEnterpriseWorkspaceReload(f.workspace));OK(umi_data_server_get(f.data,"bank/keep",value,sizeof(value)));REQUIRE(strcmp(value,"Untouched")==0);return 0;}
    if(strcmp(name,"wire_roundtrip")==0 || strcmp(name,"wire_invalid")==0) {
        char *text=NULL;size_t size=0U;EwsState *decoded=calloc(1U,sizeof(*decoded));REQUIRE(decoded!=NULL);
        UmiStatus s=EwsEncode(f.workspace->state,&text,&size);if(s!=UMI_STATUS_OK){free(decoded);return 1;}
        s=EwsDecode(text,size,decoded);bool same=s==UMI_STATUS_OK&&decoded->revision==Revision();
        if(strcmp(name,"wire_invalid")==0){text[0]='9';s=EwsDecode(text,size,decoded);same=s!=UMI_STATUS_OK;}
        free(text);free(decoded);REQUIRE(same);return 0;
    }
    if(strcmp(name,"memory_capacity_rollback")==0){char key[64];for(unsigned i=0U;i<3000U;++i){snprintf(key,sizeof(key),"filler/%u",i);UmiStatus s=umi_data_server_set(f.data,key,"x");if(s==UMI_STATUS_CAPACITY_EXCEEDED)break;OK(s);}uint64_t v=Revision();char csv[6000]="item_id,label,quantity\n";size_t used=strlen(csv);for(unsigned i=0U;i<32U;++i)used+=(size_t)snprintf(csv+used,sizeof(csv)-used,"r%u,A new workshop supply row with a sufficiently long descriptive label,1\n",i);STATUS(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"large","supplies","stock.csv",csv,used,NULL),UMI_STATUS_CAPACITY_EXCEEDED);REQUIRE(Revision()==v);OK(umi_data_server_delete(f.data,"filler/0"));OK(UmiEnterpriseWorkspaceReload(f.workspace));REQUIRE(Revision()==v);return 0;}
    fprintf(stderr,"Unknown workflow: %s\n",name);return 1;
}
static int Corruption(const char *name)
{
    REQUIRE(Setup(NULL)==0);REQUIRE(Seed()==0);
    if(strcmp(name,"orphan_storage")==0){OK(umi_data_server_delete(f.data,EWS_HEAD));}
    else if(strcmp(name,"missing_chunk")==0){OK(umi_data_server_delete(f.data,EWS_PREFIX "chunk/0000"));}
    else if(strcmp(name,"changed_chunk")==0){OK(umi_data_server_set(f.data,EWS_PREFIX "chunk/0000","corrupt"));}
    else if(strcmp(name,"unknown_owned_key")==0){OK(umi_data_server_set(f.data,EWS_PREFIX "unexpected","x"));}
    else if(strcmp(name,"malformed_head")==0){OK(umi_data_server_set(f.data,EWS_HEAD,"1\n999999\n"));}
    else return 1;
    uint64_t v=Revision();REQUIRE(UmiEnterpriseWorkspaceReload(f.workspace)!=UMI_STATUS_OK);REQUIRE(Revision()==v);return 0;
}
static int Policy(const char *name)
{
    UmiPolicyEngine *policy=NULL;UmiRoleRegistry *roles=NULL;UmiAuthorisationService *auth=NULL;
    UmiAuthorisationDecision decision;UmiStatus s;int failed=0;
    if(umi_policy_engine_create(&policy)!=UMI_STATUS_OK)return 1;
    if(umi_role_registry_create(&roles)!=UMI_STATUS_OK){umi_policy_engine_destroy(policy);return 1;}
    (void)umi_role_registry_define(roles,"operator","Operator");(void)umi_role_registry_grant(roles,"operator","enterprise.job.execute");
    (void)umi_authorisation_service_create(policy,roles,&auth);
    UmiPolicyRule deny={"operator","enterprise.job.execute","job",UMI_POLICY_DENY};
    if(strcmp(name,"policy_role_fallback")!=0)(void)umi_policy_engine_add(policy,&deny);
    if(strcmp(name,"policy_last_match") ==0){deny.effect=UMI_POLICY_ALLOW;(void)umi_policy_engine_add(policy,&deny);}
    s=umi_authorisation_check(auth,"operator","operator","enterprise.job.execute","job",&decision);
    failed=s!=UMI_STATUS_OK || (decision.allowed!=0)!=(strcmp(name,"policy_explicit_deny")!=0);
    umi_authorisation_service_destroy(auth);umi_role_registry_destroy(roles);umi_policy_engine_destroy(policy);
    REQUIRE(!failed);return 0;
}

static int Revoke(const char *name)
{
    UmiPolicyEngine *policy=NULL;UmiRoleRegistry *roles=NULL;UmiAuthorisationService *auth=NULL;
    REQUIRE(Setup(NULL)==0);REQUIRE(Seed()==0);REQUIRE(Prepare("first",stock)==0);REQUIRE(Approve("first")==0);
    if(umi_policy_engine_create(&policy)!=UMI_STATUS_OK)return 1;
    if(umi_role_registry_create(&roles)!=UMI_STATUS_OK){umi_policy_engine_destroy(policy);return 1;}
    (void)umi_role_registry_define(roles,"operator","Operator");(void)umi_role_registry_grant(roles,"operator","enterprise.job.execute");
    (void)umi_authorisation_service_create(policy,roles,&auth);
    UmiPolicyRule deny={"operator","enterprise.job.execute",strcmp(name,"policy_resource_scope")==0?"different":"first",UMI_POLICY_DENY};
    (void)umi_policy_engine_add(policy,&deny);
    /* Rebind only the test fixture's borrowed pointer; the real authorisation
     * service and its exact current policy are still used by Execute. */
    UmiAuthorisationService *old=f.workspace->authorisation;f.workspace->authorisation=auth;
    UmiStatus result=UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first");
    f.workspace->authorisation=old;
    umi_authorisation_service_destroy(auth);umi_role_registry_destroy(roles);umi_policy_engine_destroy(policy);
    REQUIRE(result==(strcmp(name,"policy_resource_scope")==0?UMI_STATUS_OK:UMI_STATUS_PERMISSION_DENIED));return 0;
}
static int Adversarial(void)
{
    UmiEnterpriseRow rows[UMI_ENTERPRISE_MAX_IMPORT_ROWS];size_t count;uint32_t random=17U;
    EwsState *state=calloc(1U,sizeof(*state));if(state==NULL)return 1;
    char text[512];
    for(unsigned attempt=0U;attempt<5000U;++attempt){
        random=random*1664525U+1013904223U;size_t length=(size_t)(random%sizeof(text));
        for(size_t j=0U;j<length;++j){random=random*1664525U+1013904223U;text[j]=(char)(random>>24U);}
        (void)EwsParse(text,length,0U,rows,&count,NULL);
        (void)EwsDecode(text,length,state);
    }
    free(state);return 0;
}
static int Sqlite(const char *name,const char *path)
{
#ifndef UMICOM_HAS_SQLITE
    (void)name;(void)path;return 77;
#else
    REQUIRE(Setup(path)==0);REQUIRE(Seed()==0);REQUIRE(Prepare("first",stock)==0);REQUIRE(Approve("first")==0);
    if(strcmp(name,"sqlite_restart")==0){REQUIRE(Apply("first")==0);UmiEnterpriseWorkspaceDestroy(f.workspace);f.workspace=NULL;umi_data_server_destroy(f.data);f.data=NULL;OK(umi_data_server_create_sqlite(path,&f.data));OK(UmiEnterpriseWorkspaceOpen(f.data,UmiEnterprisePracticeAuthorisation(f.access),&f.workspace));REQUIRE(Revision()==5U);return 0;}
    if(strcmp(name,"sqlite_two_writers")==0){OK(umi_data_server_create_sqlite(path,&f.second));OK(UmiEnterpriseWorkspaceOpen(f.second,UmiEnterprisePracticeAuthorisation(f.access),&f.other));REQUIRE(Apply("first")==0);STATUS(UmiEnterpriseWorkspaceExecute(f.other,ACT(3),"first"),UMI_STATUS_BUSY);OK(UmiEnterpriseWorkspaceReload(f.other));OK(UmiEnterpriseWorkspaceExecute(f.other,ACT(3),"first"));return 0;}
    if(strcmp(name,"sqlite_write_failure")==0){
        /* The real SQLite backend rejects the head write after chunk updates.
         * Rollback must restore every earlier write and keep approval retryable. */
        OK(umi_data_server_execute(f.data,"CREATE TRIGGER ews_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='enterprise.workspace/head' BEGIN SELECT RAISE(ABORT,'injected failure'); END;"));
        uint64_t v=Revision();REQUIRE(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first")!=UMI_STATUS_OK);REQUIRE(Revision()==v);OK(UmiEnterpriseWorkspaceReload(f.workspace));REQUIRE(Revision()==v);OK(umi_data_server_execute(f.data,"DROP TRIGGER ews_fail;"));REQUIRE(Apply("first")==0);return 0;
    }
    if(strcmp(name,"sqlite_rollback_fault")==0){
        OK(umi_data_server_execute(f.data,"CREATE TRIGGER ews_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='enterprise.workspace/head' BEGIN SELECT RAISE(ROLLBACK,'injected rollback'); END;"));
        REQUIRE(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"first")!=UMI_STATUS_OK);
        UmiEnterpriseSnapshot snapshot;OK(UmiEnterpriseWorkspaceSnapshot(f.workspace,&snapshot));REQUIRE(snapshot.storageFault&&snapshot.revision==4U);
        STATUS(UmiEnterpriseWorkspaceReload(f.workspace),UMI_STATUS_INVALID_STATE);
        UmiEnterpriseWorkspaceDestroy(f.workspace);f.workspace=NULL;umi_data_server_destroy(f.data);f.data=NULL;
        OK(umi_data_server_create_sqlite(path,&f.data));OK(umi_data_server_execute(f.data,"DROP TRIGGER ews_fail;"));
        OK(UmiEnterpriseWorkspaceOpen(f.data,UmiEnterprisePracticeAuthorisation(f.access),&f.workspace));REQUIRE(Revision()==4U);REQUIRE(Apply("first")==0);return 0;
    }
    if(strcmp(name,"sqlite_read_error")==0){OK(umi_data_server_execute(f.data,"DROP TABLE umicom_kv;"));REQUIRE(UmiEnterpriseWorkspaceReload(f.workspace)!=UMI_STATUS_OK);REQUIRE(Revision()==4U);return 0;}
    return 1;
#endif
}
int main(int argc,char **argv)
{
    int result;
    if(argc<2)return 2;
    const char *name=argv[1];
    if(strcmp(name,"adversarial_bytes")==0)result=Adversarial();
    else if(strcmp(name,"policy_revoked_after_approval")==0||strcmp(name,"policy_resource_scope")==0)result=Revoke(name);
    else if(strncmp(name,"csv_",4U)==0)result=Parser(name);
    else if(strncmp(name,"policy_",7U)==0)result=Policy(name);
    else if(strncmp(name,"sqlite_",7U)==0)result=Sqlite(name,argc>2?argv[2]:"enterprise-test.sqlite");
    else if(strcmp(name,"orphan_storage")==0||strcmp(name,"missing_chunk")==0||strcmp(name,"changed_chunk")==0||strcmp(name,"unknown_owned_key")==0||strcmp(name,"malformed_head")==0)result=Corruption(name);
    else result=Workflow(name);
    UmiEnterpriseWorkspaceDestroy(f.other);UmiEnterpriseWorkspaceDestroy(f.workspace);
    umi_data_server_destroy(f.second);umi_data_server_destroy(f.data);UmiEnterprisePracticeAccessDestroy(f.access);
    return result;
}
