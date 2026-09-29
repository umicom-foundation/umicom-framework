/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/test_recovery.c
 * PURPOSE: Check exact re-review binding, retained old decisions and atomic new-job lineage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int TestRecovery(const char *name,const char *path)
{
    TestFixture f;UmiEnterpriseRecoveryReview *review=NULL;UmiEnterpriseRecoveryInfo *info=NULL;
    UmiEnterpriseJob original,after,created;UmiEnterpriseIssue issue;
    bool sql=strncmp(name,"sqlite_",7U)==0;
    int opened=TestOpen(&f,sql?path:NULL);if(opened!=0)return opened;
    REQUIRE(TestStale(&f)==0);
    OK(UmiEnterpriseWorkspaceJobFind(f.workspace,"delivery",&original));
    if(strcmp(name,"recovery_rejected")==0||strcmp(name,"recovery_cancelled")==0){
        /* A different review-stage job exercises terminal non-applied sources. */
        const char *csv="item_id,label,quantity\na,Notebooks,25\n";
        OK(UmiEnterpriseWorkspacePrepare(f.workspace,ACT(1),"source","supplies","stock.csv",csv,strlen(csv),NULL));
        if(strcmp(name,"recovery_rejected")==0)OK(UmiEnterpriseWorkspaceReview(f.workspace,ACT(2),"source",false,"Incorrect stock sheet"));
        else OK(UmiEnterpriseWorkspaceCancel(f.workspace,ACT(1),"source","Recheck the count"));
    }
    if(strcmp(name,"recovery_disabled")==0)OK(UmiEnterpriseWorkspaceSetRecipe(f.workspace,ACT(4),"stock.csv",false));
    if(strcmp(name,"recovery_paused")==0)OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),true));
    const char *id=strcmp(name,"recovery_applied")==0?"count":strcmp(name,"recovery_missing")==0?"absent":
        (strcmp(name,"recovery_rejected")==0||strcmp(name,"recovery_cancelled")==0)?"source":"delivery";
    uint64_t before=TestRevision(&f);
    UmiStatus status=UmiEnterpriseRecoveryInspect(f.workspace,strcmp(name,"recovery_denied")==0?ACT(0):ACT(1),id,&review,&issue);
    if(strcmp(name,"recovery_applied")==0){REQUIRE(status==UMI_STATUS_INVALID_STATE&&review==NULL);}
    else if(strcmp(name,"recovery_missing")==0){REQUIRE(status==UMI_STATUS_NOT_FOUND&&review==NULL);}
    else if(strcmp(name,"recovery_denied")==0||strcmp(name,"recovery_disabled")==0){REQUIRE(status==UMI_STATUS_PERMISSION_DENIED&&review==NULL);}
    else {
        REQUIRE(status==UMI_STATUS_OK&&TestRevision(&f)==before);
        info=malloc(sizeof(*info));REQUIRE(info!=NULL);OK(UmiEnterpriseRecoveryDescribe(review,info));
        REQUIRE(info->proposedPreview.changes[0].before.quantity==15U);
        if(strcmp(name,"recovery_paused")==0) REQUIRE(info->executionPaused);
        if(strcmp(id,"delivery")==0){REQUIRE(info->originalPreview.changes[0].before.quantity==12U&&info->targetChanged);}
        if(strcmp(name,"recovery_report")==0){
            char report[32768];OK(UmiEnterpriseRecoveryFormat(review,report,sizeof(report)));
            REQUIRE(strstr(report,"quantity 15")!=NULL&&strstr(report,"quantity 20")!=NULL&&strstr(report,"job")!=NULL);
            report[0]='x';STATUS(UmiEnterpriseRecoveryFormat(review,report,1U),UMI_STATUS_CAPACITY_EXCEEDED);REQUIRE(report[0]=='\0');
        } else if(strcmp(name,"recovery_lifetime")==0){TestClose(&f);OK(UmiEnterpriseRecoveryDescribe(review,info));REQUIRE(info->originalJob.datasetGeneration==2U);}
        else {
            if(strcmp(name,"recovery_stale")==0)OK(UmiEnterpriseWorkspaceSetPaused(f.workspace,ACT(4),true));
            if(strcmp(name,"sqlite_write_failure")==0)OK(umi_data_server_execute(f.data,"CREATE TRIGGER recovery_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='enterprise.workspace/head' BEGIN SELECT RAISE(ABORT,'injected'); END;"));
            if(strcmp(name,"recovery_revoked")==0){
                UmiPolicyEngine *policy=NULL;UmiRoleRegistry *roles=NULL;UmiAuthorisationService *auth=NULL;UmiEnterpriseWorkspace *other=NULL;
                OK(umi_policy_engine_create(&policy));OK(umi_role_registry_create(&roles));
                UmiPolicyRule rule={"author","enterprise.*","*",UMI_POLICY_ALLOW};OK(umi_policy_engine_add(policy,&rule));
                rule.capability="enterprise.job.prepare";rule.effect=UMI_POLICY_DENY;OK(umi_policy_engine_add(policy,&rule));
                OK(umi_authorisation_service_create(policy,roles,&auth));OK(UmiEnterpriseWorkspaceOpen(f.data,auth,&other));
                STATUS(UmiEnterpriseRecoveryPrepare(other,ACT(1),review,"recovered",NULL),UMI_STATUS_PERMISSION_DENIED);
                UmiEnterpriseWorkspaceDestroy(other);umi_authorisation_service_destroy(auth);umi_role_registry_destroy(roles);umi_policy_engine_destroy(policy);
            } else {
                status=UmiEnterpriseRecoveryPrepare(f.workspace,strcmp(name,"recovery_actor_changed")==0?ACT(4):ACT(1),review,
                    strcmp(name,"recovery_existing_id")==0?"count":strcmp(name,"recovery_invalid_id")==0?"../bad":"recovered",&issue);
                if(strcmp(name,"recovery_stale")==0)REQUIRE(status==UMI_STATUS_BUSY);
                else if(strcmp(name,"recovery_actor_changed")==0)REQUIRE(status==UMI_STATUS_PERMISSION_DENIED);
                else if(strcmp(name,"recovery_existing_id")==0)REQUIRE(status==UMI_STATUS_ALREADY_EXISTS);
                else if(strcmp(name,"recovery_invalid_id")==0)REQUIRE(status==UMI_STATUS_INVALID_ARGUMENT);
                else if(strcmp(name,"sqlite_write_failure")==0){
                    REQUIRE(status!=UMI_STATUS_OK&&TestRevision(&f)==before);
                    STATUS(UmiEnterpriseWorkspaceJobFind(f.workspace,"recovered",&created),UMI_STATUS_NOT_FOUND);
                    OK(umi_data_server_execute(f.data,"DROP TRIGGER recovery_fail;"));
                    OK(UmiEnterpriseRecoveryPrepare(f.workspace,ACT(1),review,"recovered",NULL));
                } else {
                    REQUIRE(status==UMI_STATUS_OK);
                    OK(UmiEnterpriseWorkspaceJobFind(f.workspace,"recovered",&created));
                    REQUIRE(created.state==UMI_ENTERPRISE_JOB_REVIEW&&created.reviewer[0]=='\0'&&created.appliedRevision==0U);
                    REQUIRE(TestRevision(&f)==before+1U);
                    OK(UmiEnterpriseWorkspaceJobFind(f.workspace,"delivery",&after));REQUIRE(memcmp(&original,&after,sizeof(original))==0);
                    UmiEnterpriseAuditEntry audit;OK(UmiEnterpriseWorkspaceAuditAt(f.workspace,(size_t)before,&audit));
                    REQUIRE(strcmp(audit.action,"job.reprepare")==0&&strcmp(audit.target,"recovered")==0&&strcmp(audit.detail,id)==0);
                    STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"recovered"),UMI_STATUS_INVALID_STATE);
                    if(strcmp(name,"recovery_workflow")==0||strcmp(name,"sqlite_restart")==0){
                        OK(UmiEnterpriseWorkspaceReview(f.workspace,ACT(2),"recovered",true,"New values checked"));
                        OK(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"recovered"));
                        uint64_t revision=TestRevision(&f);OK(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"recovered"));REQUIRE(revision==TestRevision(&f));
                        STATUS(UmiEnterpriseWorkspaceExecute(f.workspace,ACT(3),"delivery"),UMI_STATUS_BUSY);
                        if(sql){TestClose(&f);OK(umi_data_server_create_sqlite(path,&f.data));OK(UmiEnterprisePracticeAccessCreate(&f.access));OK(UmiEnterpriseWorkspaceOpen(f.data,UmiEnterprisePracticeAuthorisation(f.access),&f.workspace));REQUIRE(TestRevision(&f)==revision);}
                        UmiEnterpriseRow row;OK(UmiEnterpriseWorkspaceRowAt(f.workspace,"supplies",0U,&row));REQUIRE(row.quantity==20U&&strcmp(row.sourceJob,"recovered")==0);
                    }
                }
            }
        }
    }
    free(info);UmiEnterpriseRecoveryReviewDestroy(review);TestClose(&f);return 0;
}
