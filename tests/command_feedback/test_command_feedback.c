/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/command_feedback/test_command_feedback.c
 * PURPOSE: Check command prerequisites, failure explanations and pending-draft state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"
#include "umicom/application/experience_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return 1; } } while (0)
typedef struct Fixture {
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost host;
    UmiApplicationPresentationSurfaceUpdate response;
    UmiStatus result;
    unsigned calls;
} Fixture;
static UmiStatus Controller(void *ctx, const UmiApplicationPresentationPanelPlacement *p,
    UmiApplicationPresentationSurfaceEvent e, const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out)
{
    Fixture *f=ctx; (void)p; (void)payload;
    *out=(UmiApplicationPresentationSurfaceUpdate){0};
    out->state=UMI_APPLICATION_PRESENTATION_STATE_READY;
    if (e==UMI_APPLICATION_PRESENTATION_EVENT_COMMAND || e==UMI_APPLICATION_PRESENTATION_EVENT_REFRESH) {
        f->calls++; *out=f->response; return f->result;
    }
    if(e==UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT || e==UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE)
        out->state=UMI_APPLICATION_PRESENTATION_STATE_DORMANT;
    return UMI_STATUS_OK;
}
static int Init(Fixture *f, const char *recipe, int bind, int start)
{
    REQUIRE(umi_application_presentation_surface_runtime_init(recipe,&f->runtime)==UMI_STATUS_OK);
    f->response.state=UMI_APPLICATION_PRESENTATION_STATE_READY;
    if(bind) REQUIRE(umi_application_presentation_surface_runtime_register_controller_for_all(
        &f->runtime,Controller,f)==UMI_STATUS_OK);
    umi_application_presentation_headless_surface_host_init(&f->host);
    REQUIRE(umi_application_presentation_surface_runtime_bind_host(&f->runtime,
        umi_application_presentation_headless_surface_host_interface(&f->host))==UMI_STATUS_OK);
    if(start) REQUIRE(umi_application_presentation_surface_runtime_start(&f->runtime)==UMI_STATUS_OK);
    return 0;
}
static UmiApplicationPresentationSurfaceItem *ActionItem(Fixture *f, int mode)
{
    size_t i;
    for(i=0;i<f->runtime.session.item_count;++i) {
        UmiApplicationPresentationSurfaceItem *p=&f->runtime.session.items[i];
        if(p->visible && (mode<0 ? p->behavior->command_mode!=UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY
            : (int)p->behavior->command_mode==mode))return p;
    }
    return NULL;
}
static UmiStatus HostFailure(void *context,const UmiApplicationPresentationSurfaceItem *item)
{(void)context;(void)item;return UMI_STATUS_IO_ERROR;}
static int Run(Fixture *f,const char *name)
{
    const char *recipe="org.umicom.workspace.studio.standard";
    int bind=1,start=1;
    UmiApplicationPresentationSurfaceItem *item;
    const char *id;
    UmiApplicationPresentationCommandAvailability a;
    if(strncmp(name,"recipe-",7)==0) {
        static const char *products[]={"studio","trader","desktop","bank","tms"};
        char recipeName[100];size_t i;
        for(i=0;i<5;++i)if(strcmp(name+7,products[i])==0)break;
        REQUIRE(i<5);
        snprintf(recipeName,sizeof recipeName,"org.umicom.workspace.%s.standard",products[i]);
        REQUIRE(Init(f,recipeName,1,1)==0);item=ActionItem(f,-1);REQUIRE(item);
        item->dirty=1;f->result=UMI_STATUS_IO_ERROR;
        snprintf(f->response.message,sizeof f->response.message,"Keep this pending change.");
        REQUIRE(umi_application_presentation_surface_runtime_command(&f->runtime,
            item->placement->panel->component_id,"test.save")==UMI_STATUS_IO_ERROR);
        REQUIRE(item->dirty==1 && f->calls==1 && strcmp(item->message,"Keep this pending change.")==0);
        return 0;
    }
    if(strcmp(name,"unbound")==0 || strcmp(name,"guarded-unbound")==0 || strcmp(name,"projection")==0)bind=0;
    if(strcmp(name,"not-started")==0)start=0;
    if(strcmp(name,"projection")==0)recipe="org.umicom.workspace.bank.standard";
    if(strcmp(name,"guarded-unbound")==0)recipe="org.umicom.workspace.trader.standard";
    if(strcmp(name,"background")==0)recipe="org.umicom.workspace.studio.focus";
    REQUIRE(Init(f,recipe,bind,start)==0);
    item=ActionItem(f,strcmp(name,"read-only")==0 ? UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY : -1);
    REQUIRE(item);id=item->placement->panel->component_id;
    item->dirty=1;
    if(strncmp(name,"update-",7)==0) {
        UmiApplicationPresentationSurfaceUpdate u={0};
        UmiApplicationPresentationSurfaceItem before=*item;
        uint64_t revision=f->runtime.session.revision;
        u.state=UMI_APPLICATION_PRESENTATION_STATE_READY;
        if(strcmp(name,"update-message")==0)memset(u.message,'x',sizeof u.message);
        else if(strcmp(name,"update-badge")==0)memset(u.badge,'x',sizeof u.badge);
        else if(strcmp(name,"update-state")==0)u.state=(UmiApplicationPresentationSurfaceState)99;
        else if(strcmp(name,"update-progress")==0)u.progress_percent=101;
        else if(strcmp(name,"update-dirty")==0)u.dirty=2;
        else if(strcmp(name,"update-flag")==0)u.has_progress=-1;
        else if(strcmp(name,"update-valid")==0) {
            memset(u.message,'m',sizeof(u.message)-1);memset(u.badge,'b',sizeof(u.badge)-1);
            REQUIRE(umi_application_presentation_surface_session_apply(&f->runtime.session,id,&u)==UMI_STATUS_OK);
            REQUIRE(item->dirty==0 && strlen(item->message)==sizeof(u.message)-1);return 0;
        } else return 1;
        REQUIRE(umi_application_presentation_surface_session_apply(&f->runtime.session,id,&u)==UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(item,&before,sizeof before)==0 && revision==f->runtime.session.revision);return 0;
    }
    if(strcmp(name,"check-pure")==0) {
        uint64_t revision=f->runtime.session.revision,events=f->host.event_count;
        size_t journal=f->runtime.journal.count;
        REQUIRE(UmiApplicationPresentationSurfaceCommandCheck(&f->runtime,id,&a)==UMI_STATUS_OK);
        REQUIRE(a.can_dispatch && a.controller_bound && a.status==UMI_STATUS_OK);
        REQUIRE(revision==f->runtime.session.revision && events==f->host.event_count && journal==f->runtime.journal.count && f->calls==0);
        return 0;
    }
    if(strcmp(name,"check-invalid")==0) {
        UmiApplicationPresentationCommandAvailability before;memset(&a,0x55,sizeof a);before=a;
        REQUIRE(UmiApplicationPresentationSurfaceCommandCheck(NULL,id,&a)==UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiApplicationPresentationSurfaceCommandCheck(&f->runtime,"no.component",&a)==UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&a,&before,sizeof a)==0);return 0;
    }
    if(strcmp(name,"failure-helper")==0) {
        UmiApplicationPresentationSurfaceUpdate u,before;char tooLong[UMI_APPLICATION_PRESENTATION_MESSAGE_CAPACITY+1];
        memset(&u,0x55,sizeof u);before=u;memset(tooLong,'x',sizeof tooLong);tooLong[sizeof tooLong-1]=0;
        REQUIRE(UmiApplicationPresentationSurfaceFailure(UMI_STATUS_OK,"",&u)==UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiApplicationPresentationSurfaceFailure(UMI_STATUS_IO_ERROR,tooLong,&u)==UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&u,&before,sizeof u)==0);
        REQUIRE(UmiApplicationPresentationSurfaceFailure(UMI_STATUS_NOT_IMPLEMENTED,"No service.",&u)==UMI_STATUS_NOT_IMPLEMENTED);
        REQUIRE(u.state==UMI_APPLICATION_PRESENTATION_STATE_OFFLINE && strcmp(u.message,"No service.")==0);
        return 0;
    }
    if(strcmp(name,"projection")==0) {
        /* A real registered panel must not enable a command through an unbound route. */
        const UmiApplicationExperienceDefinition *exp=umi_application_experience_catalogue_find("org.umicom.bank");
        size_t i;unsigned checked=0; REQUIRE(exp);
        for(i=0;i<exp->panel_count;++i) {
            UmiApplicationProductPanelProjection p;
            REQUIRE(umi_application_product_panel_project(exp,&f->runtime,exp->panels[i].panel_id,&p)==UMI_STATUS_OK);
            if(p.command_id[0]){REQUIRE(!p.command_enabled);checked++;}
        }
        /* Ensure this projection check exercised real bound command metadata. */
        REQUIRE(UmiApplicationPresentationSurfaceCommandCheck(&f->runtime,id,&a)==UMI_STATUS_OK && !a.can_dispatch);
        REQUIRE(checked>0); return 0;
    }
    if(strcmp(name,"unbound")==0 || strcmp(name,"guarded-unbound")==0 || strcmp(name,"read-only")==0 ||
       strcmp(name,"not-started")==0 || strcmp(name,"stopped")==0 || strcmp(name,"busy")==0 ||
       strcmp(name,"loading")==0 || strcmp(name,"hidden")==0 || strcmp(name,"background")==0) {
        UmiStatus expected=UMI_STATUS_INVALID_STATE;UmiApplicationPresentationSurfaceState before;
        if(strcmp(name,"stopped")==0)REQUIRE(umi_application_presentation_surface_runtime_stop(&f->runtime)==UMI_STATUS_OK);
        if(strcmp(name,"busy")==0){item->state=UMI_APPLICATION_PRESENTATION_STATE_BUSY;expected=UMI_STATUS_BUSY;}
        if(strcmp(name,"loading")==0){item->state=UMI_APPLICATION_PRESENTATION_STATE_LOADING;expected=UMI_STATUS_BUSY;}
        if(strcmp(name,"hidden")==0)item->visible=0;
        if(strcmp(name,"background")==0)REQUIRE(umi_application_presentation_surface_runtime_set_background(&f->runtime,1)==UMI_STATUS_OK);
        if(strcmp(name,"read-only")==0 || strcmp(name,"guarded-unbound")==0)expected=UMI_STATUS_PERMISSION_DENIED;
        if(strcmp(name,"unbound")==0)expected=item->behavior->command_mode==UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED ? UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_NOT_IMPLEMENTED;
        before=item->state;
        REQUIRE(UmiApplicationPresentationSurfaceCommandCheck(&f->runtime,id,&a)==UMI_STATUS_OK);
        REQUIRE(a.status==expected && !a.can_dispatch && a.reason[0]);
        REQUIRE(umi_application_presentation_surface_runtime_command(&f->runtime,id,"test.save")==expected);
        REQUIRE(f->calls==0);
        if(expected==UMI_STATUS_BUSY)REQUIRE(item->state==before);
        if(start && strcmp(name,"stopped")!=0)REQUIRE(item->dirty==1);
        return 0;
    }
    if(strcmp(name,"success")==0) {
        REQUIRE(umi_application_presentation_surface_runtime_command(&f->runtime,id,"test.save")==UMI_STATUS_OK);
        REQUIRE(f->calls==1 && item->dirty==0 && item->state==UMI_APPLICATION_PRESENTATION_STATE_READY);return 0;
    }
    f->result=UMI_STATUS_IO_ERROR;
    snprintf(f->response.message,sizeof f->response.message,"Cannot save Notes: destination is read-only.");
    if(strcmp(name,"denied")==0)f->result=UMI_STATUS_PERMISSION_DENIED;
    else if(strcmp(name,"unsupported")==0)f->result=UMI_STATUS_NOT_IMPLEMENTED;
    else if(strcmp(name,"cancelled")==0)f->result=UMI_STATUS_CANCELLED;
    else if(strcmp(name,"unknown-status")==0)f->result=(UmiStatus)99;
    else if(strcmp(name,"malformed-success")==0){f->result=UMI_STATUS_OK;memset(f->response.message,'x',sizeof f->response.message);}
    else if(strcmp(name,"malformed-failure")==0)memset(f->response.message,'x',sizeof f->response.message);
    else if(strcmp(name,"invalid-success-state")==0){f->result=UMI_STATUS_OK;f->response.state=(UmiApplicationPresentationSurfaceState)99;}
    else if(strcmp(name,"host-failure")==0) {
        UmiApplicationPresentationSurfaceHostOperations ops=*f->runtime.host.operations;
        ops.update=HostFailure;f->runtime.host.operations=&ops;f->result=UMI_STATUS_PERMISSION_DENIED;
        REQUIRE(umi_application_presentation_surface_runtime_command(&f->runtime,id,"test.save")==UMI_STATUS_PERMISSION_DENIED);
        f->runtime.host.operations=f->host.host.operations;
        REQUIRE(item->dirty==1);return 0;
    } else if(strcmp(name,"failure")!=0 && strcmp(name,"refresh-failure")!=0)return 1;
    {
        uint64_t events=f->host.event_count;
        UmiStatus expected=f->result==UMI_STATUS_OK?UMI_STATUS_INVALID_ARGUMENT:f->result;
        UmiStatus actual=strcmp(name,"refresh-failure")==0
            ?umi_application_presentation_surface_runtime_refresh(&f->runtime,id)
            :umi_application_presentation_surface_runtime_command(&f->runtime,id,"test.save");
        REQUIRE(actual==expected && item->dirty==1 && f->calls==1);
        REQUIRE(f->host.event_count==events+1);
        REQUIRE(item->message[0] && memchr(item->message,0,sizeof item->message));
        if(strncmp(name,"malformed",9)!=0 && strcmp(name,"invalid-success-state")!=0 && strcmp(name,"unknown-status")!=0)
            REQUIRE(strcmp(item->message,f->response.message)==0);
        if(expected==UMI_STATUS_PERMISSION_DENIED)REQUIRE(item->state==UMI_APPLICATION_PRESENTATION_STATE_PERMISSION_REQUIRED);
        if(expected==UMI_STATUS_NOT_IMPLEMENTED)REQUIRE(item->state==UMI_APPLICATION_PRESENTATION_STATE_OFFLINE);
    }
    return 0;
}
int main(int argc,char **argv)
{
    Fixture *f=calloc(1,sizeof *f);int result;
    if(!f || argc!=2){free(f);return 2;}
    result=Run(f,argv[1]);
    if(f->runtime.started)(void)umi_application_presentation_surface_runtime_stop(&f->runtime);
    free(f);return result;
}
