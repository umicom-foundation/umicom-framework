/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_help_binding_gtk4.c
 *
 * PURPOSE:
 *   Check copied native help policy, keyboard parity, nearest-context routing and retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_help_binding.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#value); failed=1; goto cleanup; } } while (0)

static int Fixture(const char *root)
{
    static const char guide[] = "<h1>Local help</h1><p>Read the example.</p>";
    char digest[UMI_SHA256_HEX_CAPACITY], manifest[1600];
    if (UmiSha256Buffer(guide,sizeof(guide)-1U,digest) != UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"guide.html",guide,sizeof(guide)-1U) != UMI_STATUS_OK) return 0;
    int n = snprintf(manifest,sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"help.fixture\","
        "\"title\":\"Help fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Local help\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\"}]}",digest);
    return n > 0 && (size_t)n < sizeof(manifest) &&
        UmiRootedFileWrite(root,"learning-pack.json",manifest,(size_t)n) == UMI_STATUS_OK;
}
static UmiLearningHelpRoute Route(void)
{
    UmiLearningHelpRoute route = {0};
    strcpy(route.request.product,"studio"); strcpy(route.request.locale,"en");
    route.request.kind = UMI_LEARNING_HELP_COMMAND; strcpy(route.request.key,"help.local");
    strcpy(route.pack_id,"help.fixture"); strcpy(route.resource_id,"guide");
    return route;
}
static int Finish(void)
{
    gint64 deadline = g_get_monotonic_time() + 15*G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL,FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if (tag != NULL && strcmp(tag,id) == 0) return widget;
    for (GtkWidget *child=gtk_widget_get_first_child(widget); child!=NULL; child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id); if (found!=NULL) return found;
    }
    return NULL;
}
static GtkEventController *Keys(GtkWidget *context)
{
    GListModel *items = gtk_widget_observe_controllers(context);
    GtkEventController *result = NULL;
    for (guint i=0U;i<g_list_model_get_n_items(items);++i) {
        GtkEventController *item = g_list_model_get_item(items,i);
        if (GTK_IS_EVENT_CONTROLLER_KEY(item)) { result=item; break; }
        g_object_unref(item);
    }
    g_object_unref(items);
    return result;
}
static gboolean Key(GtkEventController *controller, guint key, GdkModifierType modifiers)
{
    gboolean handled=FALSE;
    g_signal_emit_by_name(controller,"key-pressed",key,0U,modifiers,&handled);
    return handled;
}
typedef struct Observer {
    GtkWidget *context, *panel;
    const char *mode;
    unsigned int calls;
    UmiStatus nested;
} Observer;
static void StatusChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Observer *observer=data; ++observer->calls;
    if (observer->calls != 1U) return;
    if (strcmp(observer->mode,"reentrant") == 0)
        observer->nested=UmiLearningHelpGtk4Open(observer->context);
    else if (strcmp(observer->mode,"unbind-observer") == 0)
        UmiLearningHelpGtk4Unbind(observer->context);
    else if (strcmp(observer->mode,"retire-observer") == 0)
        UmiLearningCatalogueGtk4Retire(observer->panel);
}
int main(int argc, char **argv)
{
    if (argc!=3) return 2;
    if (!gtk_init_check()) return 77;
    int failed=0,matched=0;
    const char *name=argv[1];
    GtkWidget *context=NULL,*panel=NULL,*inner=NULL,*button=NULL;
    GtkEventController *keys=NULL,*old_keys=NULL;
    GtkWidget *label=NULL;
    gulong observer_signal=0U;
    UmiLearningHelpGtkState state={0},unchanged={0};
    UmiLearningCatalogueGtkState catalogue={0};
    UmiLearningHelpRoute route=Route();
    Observer observer={NULL,NULL,name,0U,UMI_STATUS_OK};
    CHECK(Fixture(argv[2]));
    context=gtk_box_new(GTK_ORIENTATION_VERTICAL,0); g_object_ref_sink(context);
    inner=gtk_box_new(GTK_ORIENTATION_VERTICAL,0); gtk_box_append(GTK_BOX(context),inner);
    button=gtk_button_new_with_label("Help"); gtk_box_append(GTK_BOX(inner),button);
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0}); CHECK(panel!=NULL); g_object_ref_sink(panel);
    if (strcmp(name,"arguments")==0) {
        matched=1;
        CHECK(UmiLearningHelpGtk4Bind(NULL,panel,&route)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningHelpGtk4Bind(context,NULL,&route)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningHelpGtk4Bind(context,panel,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningHelpGtk4Open(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_NOT_FOUND);
        state.attempts=71U; unchanged=state;
        CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_NOT_FOUND);
        CHECK(state.attempts==unchanged.attempts);
        strcpy(route.request.key,"https://invalid.example/private");
        CHECK(UmiLearningHelpGtk4Bind(context,panel,&route)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Keys(context)==NULL);
        goto cleanup;
    }
    if (strcmp(name,"missing-topic")==0) strcpy(route.resource_id,"absent");
    if (strcmp(name,"locale")==0) strcpy(route.request.locale,"fr");
    CHECK(UmiLearningHelpGtk4Bind(context,panel,&route)==UMI_STATUS_OK);
    CHECK(UmiLearningHelpGtk4Bind(context,panel,&route)==UMI_STATUS_ALREADY_EXISTS);
    keys=Keys(context); CHECK(keys!=NULL);
    CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_OK);
    CHECK(state.attempts==0U && !state.dispatching && strcmp(state.request.product,"studio")==0);
    if (strcmp(name,"missing-pack")==0) {
        matched=1;
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_NOT_FOUND);
        label=Find(panel,"learning.catalogue.status"); CHECK(label!=NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)),"matching local learning pack")!=NULL);
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        /* The same binding becomes usable after explicit pack admission. */
    }
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2])==UMI_STATUS_OK && Finish());
    if (strcmp(name,"copied-policy")==0) memset(&route,0,sizeof(route));
    if (strcmp(name,"explicit")==0 || strcmp(name,"copied-policy")==0 || strcmp(name,"missing-pack")==0) {
        matched=1; CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_OK);
    } else if (strcmp(name,"f1")==0) {
        matched=1; CHECK(Key(keys,GDK_KEY_F1,0));
        CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_OK && state.last_status==UMI_STATUS_OK && state.attempts==1U);
    } else if (strcmp(name,"modified-f1")==0 || strcmp(name,"unrelated-key")==0) {
        matched=1;
        if (strcmp(name,"modified-f1")==0) {
            const GdkModifierType modifiers[]={GDK_SHIFT_MASK,GDK_CONTROL_MASK,GDK_ALT_MASK,GDK_SUPER_MASK,GDK_HYPER_MASK,GDK_META_MASK};
            for(size_t i=0U;i<sizeof(modifiers)/sizeof(modifiers[0]);++i) CHECK(!Key(keys,GDK_KEY_F1,modifiers[i]));
        } else CHECK(!Key(keys,GDK_KEY_F2,0));
        CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_OK && state.attempts==0U);
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U); goto cleanup;
    } else if (strcmp(name,"missing-topic")==0 || strcmp(name,"locale")==0) {
        matched=1;
        CHECK(UmiLearningHelpGtk4Open(button)==(strcmp(name,"locale")==0?UMI_STATUS_UNAVAILABLE:UMI_STATUS_NOT_FOUND));
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&catalogue)==UMI_STATUS_OK && !catalogue.reader.has_document);
        goto cleanup;
    } else if (strcmp(name,"busy")==0) {
        matched=1; CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_OK);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_BUSY);
        CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_OK && state.attempts==2U);
    } else if (strcmp(name,"retired")==0 || strcmp(name,"dead-destination")==0) {
        matched=1; UmiLearningCatalogueGtk4Retire(panel);
        if (strcmp(name,"dead-destination")==0) g_clear_object(&panel);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U); goto cleanup;
    } else if (strcmp(name,"nearest")==0) {
        matched=1; strcpy(route.resource_id,"missing-nearest");
        CHECK(UmiLearningHelpGtk4Bind(inner,panel,&route)==UMI_STATUS_OK);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningHelpGtk4CopyState(context,&state)==UMI_STATUS_OK && state.attempts==0U);
        UmiLearningHelpGtk4Unbind(inner);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_OK);
    } else if (strcmp(name,"unbind")==0) {
        matched=1; old_keys=keys;keys=NULL;
        UmiLearningHelpGtk4Unbind(context);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_NOT_FOUND && !Key(old_keys,GDK_KEY_F1,0));
        CHECK(UmiLearningHelpGtk4Bind(context,panel,&route)==UMI_STATUS_OK);
        CHECK(!Key(old_keys,GDK_KEY_F1,0));
        keys=Keys(context);CHECK(keys!=NULL && keys!=old_keys && Key(keys,GDK_KEY_F1,0));
    } else if (strcmp(name,"destroy-context")==0) {
        matched=1;g_clear_object(&context);inner=NULL;button=NULL;
        CHECK(!Key(keys,GDK_KEY_F1,0));CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);goto cleanup;
    } else if (strcmp(name,"reentrant")==0 || strcmp(name,"unbind-observer")==0 || strcmp(name,"retire-observer")==0) {
        matched=1;
        /* Missing help publishes a status synchronously. Mutations from that
         * observer must not free the in-flight policy or recursively dispatch. */
        UmiLearningHelpGtk4Unbind(context); strcpy(route.resource_id,"absent");
        CHECK(UmiLearningHelpGtk4Bind(context,panel,&route)==UMI_STATUS_OK);
        observer.context=context;observer.panel=panel;
        label=Find(panel,"learning.catalogue.status");CHECK(label!=NULL);
        observer_signal=g_signal_connect(label,"notify::label",G_CALLBACK(StatusChanged),&observer);
        CHECK(UmiLearningHelpGtk4Open(button)==UMI_STATUS_NOT_FOUND);
        CHECK(observer.calls==1U && UmiLearningCatalogueGtk4PendingReads()==0U);
        if(strcmp(name,"reentrant")==0) {
            CHECK(observer.nested==UMI_STATUS_BUSY);
            CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_OK && state.attempts==1U && !state.dispatching);
        } else if(strcmp(name,"unbind-observer")==0)
            CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_NOT_FOUND);
        else CHECK(UmiLearningCatalogueGtk4CopyState(panel,&catalogue)==UMI_STATUS_OK && catalogue.retired);
        goto cleanup;
    }
    CHECK(matched && Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&catalogue)==UMI_STATUS_OK);
    CHECK(!catalogue.busy && catalogue.last_status==UMI_STATUS_OK && catalogue.reader.has_document);
    CHECK(strcmp(catalogue.reader.selection.resource.id,"guide")==0);
    CHECK(UmiLearningHelpGtk4CopyState(button,&state)==UMI_STATUS_OK && !state.dispatching);
cleanup:
    if(observer_signal!=0U && label!=NULL) g_signal_handler_disconnect(label,observer_signal);
    if(context!=NULL) UmiLearningHelpGtk4Unbind(context);
    if(panel!=NULL) UmiLearningCatalogueGtk4Retire(panel);
    if(!Finish()) failed=1;
    g_clear_object(&keys);g_clear_object(&old_keys);
    g_clear_object(&context);g_clear_object(&panel);
    return failed;
}
