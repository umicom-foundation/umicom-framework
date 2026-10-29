/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_navigation_gtk4.c
 *
 * PURPOSE:
 *   Verify local links and declared examples preserve native reading ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); \
    failed = 1; goto cleanup; } } while (0)
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static int Drain(void)
{
    gint64 end = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static int Fixture(const char *directory)
{
    const char text[] = "<h1>Local guide</h1><p>Read example.c before copying it.</p>";
    const char code[] = "int answer(void) { return 42; }\n";
    char hash[UMI_SHA256_HEX_CAPACITY], example_hash[UMI_SHA256_HEX_CAPACITY], json[2400];
    if (UmiSha256Buffer(text, sizeof(text)-1U, hash) != UMI_STATUS_OK ||
        UmiSha256Buffer(code, sizeof(code)-1U, example_hash) != UMI_STATUS_OK) return 0;
    int size=snprintf(json,sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"navigation.fixture\","
        "\"title\":\"Native links\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"navigation\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\",\"example\":\"source\"},"
        "{\"id\":\"source\",\"title\":\"Source\",\"topic\":\"navigation\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"%s\"},"
        "{\"id\":\"plain\",\"title\":\"Unlinked guide\",\"topic\":\"navigation\",\"kind\":\"guide\","
        "\"path\":\"plain.html\",\"sha256\":\"%s\"}]}",hash,example_hash,hash);
    return size>0 && (size_t)size<sizeof(json) &&
        UmiRootedFileWrite(directory,"guide.html",text,sizeof(text)-1U)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"plain.html",text,sizeof(text)-1U)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"example.c",code,sizeof(code)-1U)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"learning-pack.json",json,(size_t)size)==UMI_STATUS_OK;
}
static void RetireOnMessage(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    int failed=0;
    GtkWidget *panel=NULL;
    gulong handler=0U;
    UmiLearningCatalogueGtkState before,after;
    CHECK(Fixture(argv[2]));
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0U,9U,0U});
    CHECK(panel!=NULL);
    g_object_ref_sink(panel);
    CHECK(UmiLearningCatalogueGtk4Follow(panel,"example.c")==UMI_STATUS_INVALID_STATE);
    CHECK(UmiLearningCatalogueGtk4OpenExample(panel)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2])==UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4Open(panel,strcmp(argv[1],"example-missing")==0?2U:0U)==UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.last_status==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4Search(panel,"absent")==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.search.result_count==0U);
    if(strcmp(argv[1],"refused-links")==0) {
        const char *links[]={"../outside.c","https://example.invalid/","/absolute","example.c?run=1","missing.c"};
        for(size_t i=0U;i<sizeof(links)/sizeof(links[0]);++i) {
            CHECK(UmiLearningCatalogueGtk4Follow(panel,links[i])!=UMI_STATUS_OK);
            CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        }
        CHECK(UmiLearningCatalogueGtk4Follow(panel,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
        CHECK(after.reader.revision==before.reader.revision);
        goto cleanup;
    }
    if(strcmp(argv[1],"example-missing")==0) {
        CHECK(UmiLearningCatalogueGtk4OpenExample(panel)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        goto cleanup;
    }
    if(strcmp(argv[1],"retired")==0) {
        UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4Follow(panel,"example.c")==UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4OpenExample(panel)==UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    if(g_str_has_suffix(argv[1],"-retire-callback")) {
        handler=g_signal_connect(Find(panel,"learning.catalogue.status"),"notify::label",
            G_CALLBACK(RetireOnMessage),panel);
        UmiStatus status=g_str_has_prefix(argv[1],"example-")?
            UmiLearningCatalogueGtk4OpenExample(panel):UmiLearningCatalogueGtk4Follow(panel,"example.c");
        CHECK(status==UMI_STATUS_INVALID_STATE && UmiLearningCatalogueGtk4PendingReads()==0U);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
        CHECK(after.retired && !after.busy && after.reader.revision==before.reader.revision);
        goto cleanup;
    }
    int changed=strcmp(argv[1],"follow-changed")==0 || strcmp(argv[1],"example-changed")==0;
    if(changed) CHECK(UmiRootedFileWrite(argv[2],"example.c","changed",7U)==UMI_STATUS_OK);
    if(strcmp(argv[1],"native-controls")==0) {
        GtkWidget *entry=Find(panel,"learning.catalogue.link");
        GtkWidget *button=Find(panel,"learning.catalogue.follow");
        CHECK(GTK_IS_ENTRY(entry) && GTK_IS_BUTTON(button));
        gtk_editable_set_text(GTK_EDITABLE(entry),"example.c");
        g_signal_emit_by_name(button,"clicked");
        CHECK(UmiLearningCatalogueGtk4PendingReads()==1U);
        gtk_editable_set_text(GTK_EDITABLE(entry),"missing.c");
    } else if(g_str_has_prefix(argv[1],"example-") || strcmp(argv[1],"example")==0) {
        CHECK(UmiLearningCatalogueGtk4OpenExample(panel)==UMI_STATUS_OK);
    } else {
        char link[128];
        const char *value=strcmp(argv[1],"anchor")==0?"#details":
            strcmp(argv[1],"empty-link")==0?"":"example.c";
        strcpy(link,value);
        CHECK(strcmp(argv[1],"follow")==0 || strcmp(argv[1],"anchor")==0 ||
            strcmp(argv[1],"empty-link")==0 || strcmp(argv[1],"copied-input")==0 ||
            strcmp(argv[1],"busy")==0 || strcmp(argv[1],"follow-changed")==0);
        CHECK(UmiLearningCatalogueGtk4Follow(panel,link)==UMI_STATUS_OK);
        strcpy(link,"missing.c");
    }
    CHECK(UmiLearningCatalogueGtk4Follow(panel,"guide.html")==UMI_STATUS_BUSY);
    CHECK(UmiLearningCatalogueGtk4OpenExample(panel)==UMI_STATUS_BUSY);
    CHECK(Drain() && UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
    CHECK(after.search.result_count==0U && !after.has_copy_result);
    if(changed) {
        CHECK(after.last_status==UMI_STATUS_INVALID_STATE && after.reader.revision==before.reader.revision);
        goto cleanup;
    }
    CHECK(after.last_status==UMI_STATUS_OK);
    if(strcmp(argv[1],"empty-link")==0) {
        CHECK(after.reader.history_count==1U && strcmp(after.reader.selection.resource.id,"guide")==0);
    } else if(strcmp(argv[1],"anchor")==0) {
        CHECK(strcmp(after.reader.selection.fragment,"details")==0 && after.reader.history_count==2U);
    } else {
        CHECK(strcmp(after.reader.selection.resource.id,"source")==0 && after.reader.history_count==2U);
        if(strcmp(argv[1],"example-current")==0) {
            CHECK(UmiLearningCatalogueGtk4OpenExample(panel)==UMI_STATUS_OK && Drain());
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
            CHECK(after.reader.history_count==2U);
        }
        CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
        CHECK(strcmp(after.reader.selection.resource.id,"guide")==0 && after.reader.can_go_forward);
    }
cleanup:
    if(handler!=0U)g_signal_handler_disconnect(Find(panel,"learning.catalogue.status"),handler);
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    if(!Drain())failed=1;
    g_clear_object(&panel);
    return failed;
}
