/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_profile/test_profile_gate_gtk4.c
 * PURPOSE: Exercise native profile actions and delayed-completion ownership with an isolated store.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "fixture.h"
#include "umicom/security/gtk4/local_profile_gate.h"
#include "umicom/ui/gtk4/workstation/workspace_storage.h"

static gint hold_worker, worker_entered, release_worker, fixture_destroyed;
static void NativeFixtureDestroy(void *data)
{ g_free(data); g_atomic_int_set(&fixture_destroyed,1); }
static UmiStatus NativeDerive(void *data,const char *password,size_t length,
                              const UmiLocalProfileRecord *record,unsigned char out[32])
{
    if(g_atomic_int_get(&hold_worker)) {
        g_atomic_int_set(&worker_entered,1);
        gint64 deadline=g_get_monotonic_time()+5000000;
        while(!g_atomic_int_get(&release_worker) && g_get_monotonic_time()<deadline) g_usleep(1000U);
        if(!g_atomic_int_get(&release_worker)) return UMI_STATUS_TIMEOUT;
    }
    return FixtureDerive(data,password,length,record,out);
}
static void WaitFlag(gint *flag)
{
    gint64 deadline=g_get_monotonic_time()+5000000;
    while(!g_atomic_int_get(flag) && g_get_monotonic_time()<deadline) {
        (void)g_main_context_iteration(NULL,FALSE); g_usleep(1000U);
    }
    CHECK(g_atomic_int_get(flag));
}
/* Broker intent is observed separately from local workspace authentication. */
typedef struct Opened { int count; char name[49]; int broker_calls, live; } Opened;
static void BrokerEnvironment(void *data, int live)
{ Opened *opened = data; ++opened->broker_calls; opened->live = live; }
static void BrokerLegacy(void *data) { (void)data; }

static UmiStatus Open(void *data,const char *name)
{ Opened *opened=data; ++opened->count; strcpy(opened->name,name); return UMI_STATUS_OK; }
/* The chart inspector owns controls while collapsed. Shared logical lookup replaces the rendered-child walk, retained here for review. The previous implementation is retained for engineering review. */
#if 0
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    /* Resolve controls by their public automation identity, not layout position. */
    const char *value=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(value!=NULL && strcmp(value,id)==0) return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id); if(found!=NULL) return found;
    }
    return NULL;
}
#endif
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    /* Read controls owned by collapsed chart inspectors without changing layout. */
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
static GtkWidget *Required(GtkWidget *root,const char *id)
{ GtkWidget *widget=Find(root,id); CHECK(widget!=NULL); return widget; }
static void Drain(Opened *opened,int target)
{
    gint64 deadline=g_get_monotonic_time()+5000000;
    while(opened->count<target && g_get_monotonic_time()<deadline) {
        (void)g_main_context_iteration(NULL,FALSE); g_usleep(1000U);
    }
    CHECK(opened->count==target);
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    if(!gtk_init_check()) return 77;
    ProfileFixture *fixture=g_new0(ProfileFixture,1);
    UmiLocalProfileBackend backend={fixture,FixtureRead,FixtureCreate,FixtureRemove,FixtureRandom,NativeDerive,NativeFixtureDestroy};
    UmiLocalProfileStore *store=NULL; CHECK(UmiLocalProfileStoreCreate(&backend,&store)==UMI_STATUS_OK);
    Opened opened={0}; UmiLocalProfileGateConfig config={"org.umicom.test","Local profile test",store,&opened,Open,NULL};
    config.open_broker = BrokerLegacy;
    UmiLocalProfileGate *gate=NULL;
    CHECK(UmiLocalProfileGateCreate(&config,&gate)==UMI_STATUS_OK);
    GtkWidget *root=UmiLocalProfileGateWidget(gate);
    if (strcmp(argv[1], "pages") == 0) {
        GtkStack *pages = GTK_STACK(Required(root, "profile.pages"));
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "login") == 0);
        gtk_editable_set_text(GTK_EDITABLE(Required(root, "profile.password")), "local test phrase");
        gtk_stack_set_visible_child_name(pages, "register");
        CHECK(gtk_editable_get_text(GTK_EDITABLE(Required(root, "profile.password")))[0] == '\0');
        gtk_editable_set_text(GTK_EDITABLE(Required(root, "profile.register-password")), "local test phrase");
        gtk_editable_set_text(GTK_EDITABLE(Required(root, "profile.confirm-password")), "local test phrase");
        gtk_stack_set_visible_child_name(pages, "login");
        CHECK(gtk_editable_get_text(GTK_EDITABLE(Required(root, "profile.register-password")))[0] == '\0');
        CHECK(gtk_editable_get_text(GTK_EDITABLE(Required(root, "profile.confirm-password")))[0] == '\0');
        CHECK(fixture->writes == 0 && opened.count == 0);
    } else if (strcmp(argv[1], "broker-intent") == 0) {
        UmiLocalProfileGateSetBrokerEnvironmentAction(gate, BrokerEnvironment);
        g_signal_emit_by_name(Required(root, "profile.broker-paper"), "clicked");
        CHECK(opened.broker_calls == 1 && opened.live == 0);
        g_signal_emit_by_name(Required(root, "profile.broker-live"), "clicked");
        CHECK(opened.broker_calls == 2 && opened.live == 1 && opened.count == 0 && fixture->writes == 0);
        GtkWidget *retained = Required(root, "profile.broker-live"); g_object_ref(retained);
        UmiLocalProfileGateDestroy(gate); gate = NULL;
        g_signal_emit_by_name(retained, "clicked");
        CHECK(opened.broker_calls == 2); g_object_unref(retained);
    } else if(strcmp(argv[1],"storage-isolation")==0) {
        char alice[256], canonical[256], bob[256], untouched[8]="keep";
        CHECK(UmiGtk4WorkspaceProfileStorageId("org.umicom.trader","Alice",alice,sizeof(alice))==UMI_STATUS_OK);
        CHECK(UmiGtk4WorkspaceProfileStorageId("org.umicom.trader","ALICE",canonical,sizeof(canonical))==UMI_STATUS_OK);
        CHECK(strcmp(alice,canonical)==0);
        CHECK(UmiGtk4WorkspaceProfileStorageId("org.umicom.trader","bob",bob,sizeof(bob))==UMI_STATUS_OK);
        CHECK(strcmp(alice,bob)!=0 && strcmp(alice,"org.umicom.trader.profile.user_alice")==0);
        CHECK(UmiGtk4WorkspaceProfileStorageId("../trader","alice",untouched,sizeof(untouched))==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4WorkspaceProfileStorageId("org.umicom.trader","../alice",untouched,sizeof(untouched))==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4WorkspaceProfileStorageId("org.umicom.trader","alice",untouched,sizeof(untouched))==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(untouched,"keep")==0 && opened.count==0 && fixture->writes==0);
    } else if(strcmp(argv[1],"simulator")==0) {
        g_signal_emit_by_name(Required(root,"profile.simulator"),"clicked");
        CHECK(opened.count==1 && opened.name[0]=='\0' && fixture->writes==0);
    } else if(strcmp(argv[1],"create")==0 || strcmp(argv[1],"close-pending")==0) {
/* The previous combined form used profile.username here; retained below. */
#if 0
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.username")),"Alice");
#endif
        /* Registration is a separate page with its own fields. */
        gtk_stack_set_visible_child_name(GTK_STACK(Required(root,"profile.pages")), "register");
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.register-username")),"Alice");
/* The registration page now owns its separate password field. */
#if 0
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.password")),"local test phrase");
#endif
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.register-password")),"local test phrase");
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.confirm-password")),"local test phrase");
        if(strcmp(argv[1],"close-pending")==0) g_atomic_int_set(&hold_worker,1);
        g_signal_emit_by_name(Required(root,"profile.create"),"clicked");
        CHECK(gtk_editable_get_text(GTK_EDITABLE(Required(root,"profile.password")))[0]=='\0');
        CHECK(gtk_editable_get_text(GTK_EDITABLE(Required(root,"profile.register-password")))[0]=='\0');
        if(strcmp(argv[1],"create")==0) {
            Drain(&opened,1); CHECK(strcmp(opened.name,"alice")==0 && fixture->writes==1);
        } else {
            WaitFlag(&worker_entered);
            GtkWidget *retained=Required(root,"profile.simulator"); g_object_ref(retained);
            UmiLocalProfileGateDestroy(gate); gate=NULL;
            g_signal_emit_by_name(retained,"clicked");
            g_atomic_int_set(&release_worker,1);
            UmiLocalProfileStoreRelease(store); store=NULL;
            /* The final store ref disappears only after both the cancelled
             * completion and its task payload have released their ownership. */
            WaitFlag(&fixture_destroyed);
            CHECK(opened.count==0); g_object_unref(retained);
        }
    } else if(strcmp(argv[1],"remove-confirmation")==0) {
        CHECK(UmiLocalProfileRegister(store,"alice","local test phrase")==UMI_STATUS_OK);
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.username")),"alice");
        gtk_editable_set_text(GTK_EDITABLE(Required(root,"profile.password")),"local test phrase");
        g_signal_emit_by_name(Required(root,"profile.remove"),"clicked");
        CHECK(fixture->present && fixture->removals==0 && opened.count==0);
    } else return 2;
    UmiLocalProfileGateDestroy(gate); UmiLocalProfileStoreRelease(store); return 0;
}
