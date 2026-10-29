/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_find_gtk4.c
 *
 * PURPOSE:
 *   Check guide search selection, Unicode offsets, refusal and synchronous callback ownership.
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
#include "../support/gtk4_expanders.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#value); failed=1; goto cleanup; } } while (0)
/* Preserve the original two-resource fixture for review. The expanded fixture
 * exercises source character offsets and refuses to search diagnostic text. */
#if 0
static int Fixture(const char *root)
{
    static const char guide[] = "<h1 id='start'>Find Guide</h1><p>Alpha beta alpha. "
        "\xce\xa9 and caf\xc3\xa9.</p><p><code>alpha_call()</code></p>"
        "<script>hidden_secret</script><a href='example.c'>Source</a>";
    static const char source[] = "int alpha(void) { return 42; }\n";
    char hash[UMI_SHA256_HEX_CAPACITY], source_hash[UMI_SHA256_HEX_CAPACITY], json[1800];
    if (UmiSha256Buffer(guide,sizeof(guide)-1U,hash)!=UMI_STATUS_OK ||
        UmiSha256Buffer(source,sizeof(source)-1U,source_hash)!=UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"guide.html",guide,sizeof(guide)-1U)!=UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"example.c",source,sizeof(source)-1U)!=UMI_STATUS_OK) return 0;
    int size=snprintf(json,sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"find.fixture\","
        "\"title\":\"Find fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"source\",\"title\":\"Source\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"%s\"}]}",hash,source_hash);
    return size>0 && (size_t)size<sizeof(json) &&
        UmiRootedFileWrite(root,"learning-pack.json",json,(size_t)size)==UMI_STATUS_OK;
}
#endif
static int Fixture(const char *root)
{
    static const char guide[] = "<h1 id='start'>Find Guide</h1><p>Alpha beta alpha. "
        "\xce\xa9 and caf\xc3\xa9.</p><p><code>alpha_call()</code></p>"
        "<script>hidden_secret</script><a href='example.c'>Source</a>";
    static const char source[] = "/* \xce\xa9: verified example */\n"
        "int alpha(void) { return 42; }\n/* alpha is a function. */\n";
    static const unsigned char invalid[] = {0xc3U, 0x28U};
    static const char image[] = "not a PNG image";
    char hash[UMI_SHA256_HEX_CAPACITY], source_hash[UMI_SHA256_HEX_CAPACITY], json[2600], invalid_hash[UMI_SHA256_HEX_CAPACITY], image_hash[UMI_SHA256_HEX_CAPACITY];
    if (UmiSha256Buffer(guide,sizeof(guide)-1U,hash)!=UMI_STATUS_OK ||
        UmiSha256Buffer(source,sizeof(source)-1U,source_hash)!=UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"guide.html",guide,sizeof(guide)-1U)!=UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"example.c",source,sizeof(source)-1U)!=UMI_STATUS_OK ||
        UmiSha256Buffer(invalid,sizeof(invalid),invalid_hash)!=UMI_STATUS_OK ||
        UmiSha256Buffer(image,sizeof(image)-1U,image_hash)!=UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"invalid.c",invalid,sizeof(invalid))!=UMI_STATUS_OK ||
        UmiRootedFileWrite(root,"invalid.png",image,sizeof(image)-1U)!=UMI_STATUS_OK) return 0;
    int size=snprintf(json,sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"find.fixture\","
        "\"title\":\"Find fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"source\",\"title\":\"Source\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"%s\"},"
        "{\"id\":\"invalid\",\"title\":\"Invalid text\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"invalid.c\",\"sha256\":\"%s\"},"
        "{\"id\":\"image\",\"title\":\"Image\",\"topic\":\"reading\",\"kind\":\"image\","
        "\"path\":\"invalid.png\",\"sha256\":\"%s\"}]}",hash,source_hash,invalid_hash,image_hash);
    return size>0 && (size_t)size<sizeof(json) &&
        UmiRootedFileWrite(root,"learning-pack.json",json,(size_t)size)==UMI_STATUS_OK;
}
static int Finish(void)
{
    gint64 deadline=g_get_monotonic_time()+15*G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads()!=0U && g_get_monotonic_time()<deadline) {
        while(g_main_context_iteration(NULL,FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads()==0U;
}
static GtkWidget *Find(GtkWidget *widget,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if(tag!=NULL && strcmp(tag,id)==0)return widget;
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }
    return NULL;
}
static char *Text(GtkTextBuffer *buffer)
{
    GtkTextIter first,last;gtk_text_buffer_get_bounds(buffer,&first,&last);
    return gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
}
static int Selected(GtkTextBuffer *buffer,const char *expected)
{
    GtkTextIter first,last;
    if(!gtk_text_buffer_get_selection_bounds(buffer,&first,&last))return 0;
    char *text=gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
    int matches=strcmp(text,expected)==0;g_free(text);return matches;
}
static GtkEventController *Keys(GtkWidget *panel)
{
    GListModel *items=gtk_widget_observe_controllers(panel);
    GtkEventController *found=NULL;
    for(guint i=0U;i<g_list_model_get_n_items(items);++i) {
        GtkEventController *item=g_list_model_get_item(items,i);
        if(g_object_get_data(G_OBJECT(item),"umicom-learning-find-controller")!=NULL){found=item;break;}
        g_object_unref(item);
    }
    g_object_unref(items);return found;
}
static gboolean Key(GtkEventController *keys,guint key,GdkModifierType modifiers)
{
    gboolean handled=FALSE;g_signal_emit_by_name(keys,"key-pressed",key,0U,modifiers,&handled);return handled;
}
typedef struct Observer {
    GtkWidget *panel;
    unsigned calls;
    int retire,busy;
    UmiStatus nested;
} Observer;
static void Observe(Observer *observer)
{
    if(++observer->calls!=1U)return;
    UmiLearningCatalogueGtkState state;
    observer->busy=UmiLearningCatalogueGtk4CopyState(observer->panel,&state)==UMI_STATUS_OK && state.busy;
    observer->nested=UmiLearningCatalogueGtk4FindText(observer->panel,"beta",1,0,NULL);
    if(observer->retire)UmiLearningCatalogueGtk4Retire(observer->panel);
}
static void MarkSet(GtkTextBuffer *buffer,const GtkTextIter *location,GtkTextMark *mark,gpointer data)
{ (void)buffer;(void)location;(void)mark;Observe(data); }
static void Notice(GObject *object,GParamSpec *property,gpointer data)
{ (void)object;(void)property;Observe(data); }

/* Source search uses the same native controls, but its bytes are owned by the
 * reader instead of the HTML model. These cases exercise that ownership at
 * history, failed reload, diagnostic and synchronous callback boundaries. */
static int SourceCase(const char *name, const char *directory, GtkWidget *panel, GtkTextBuffer *buffer)
{
    int failed=0;
    char *text=NULL;
    UmiLearningCatalogueGtkState before,after;
    UmiLearningCatalogueFindMatch result={71U,72U,73U,1},sentinel=result;
    CHECK(UmiLearningCatalogueGtk4Follow(panel,"example.c")==UMI_STATUS_OK);
    if(strcmp(name,"source-busy")==0) {
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,0,&result)==UMI_STATUS_BUSY);
        CHECK(result.byte_start==sentinel.byte_start);
    }
    CHECK(Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK);
    CHECK(before.last_status==UMI_STATUS_OK && !before.has_reading_structure);
    text=Text(buffer);CHECK(text!=NULL && strstr(text,"int alpha")!=NULL);
    GtkTextIter start;gtk_text_buffer_get_start_iter(buffer,&start);
    gtk_text_buffer_place_cursor(buffer,&start);
    if(strcmp(name,"source-invalid")==0 || strcmp(name,"source-image")==0) {
        CHECK(UmiLearningCatalogueGtk4Follow(panel,strcmp(name,"source-invalid")==0?
            "invalid.c":"invalid.png")==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
        CHECK(after.last_status==UMI_STATUS_OK && !after.has_reading_structure);
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,0,&result)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"verified",0,0,&result)==UMI_STATUS_NOT_FOUND);
        CHECK(result.byte_start==sentinel.byte_start && result.reader_revision==sentinel.reader_revision);
        goto cleanup;
    }
    if(strcmp(name,"source-history")==0) {
        CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"Alpha",1,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,"Alpha"));
        CHECK(UmiLearningCatalogueGtk4Forward(panel)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK);
        CHECK(!before.has_reading_structure && before.last_status==UMI_STATUS_OK);
    } else if(strcmp(name,"source-reload-refusal")==0 || strcmp(name,"source-snapshot")==0) {
        CHECK(UmiRootedFileWrite(directory,"example.c","replacement",11U)==UMI_STATUS_OK);
        if(strcmp(name,"source-reload-refusal")==0) {
            CHECK(UmiLearningCatalogueGtk4Reload(panel)==UMI_STATUS_OK && Finish());
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
            CHECK(after.last_status!=UMI_STATUS_OK && after.reader.revision==before.reader.revision);
        }
    }
    if(strcmp(name,"source-unicode")==0) {
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"\xce\xa9",1,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,"\xce\xa9") && result.byte_length==2U);
        CHECK(result.byte_start==(size_t)(strstr(text,"\xce\xa9")-text));
    } else if(strcmp(name,"source-reentrant")==0 || strcmp(name,"source-retire")==0) {
        Observer observer={panel,0U,strcmp(name,"source-retire")==0,0,UMI_STATUS_OK};
        gulong signal=g_signal_connect(buffer,"mark-set",G_CALLBACK(MarkSet),&observer);
        UmiStatus status=UmiLearningCatalogueGtk4FindText(panel,"alpha",1,0,&result);
        g_signal_handler_disconnect(buffer,signal);
        CHECK(status==UMI_STATUS_OK && observer.calls>0U && observer.busy && observer.nested==UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK && after.retired==observer.retire && !after.busy);
    } else if(strcmp(name,"source-controls")==0) {
        GtkWidget *entry=Find(panel,"learning.catalogue.find-query");CHECK(GTK_IS_ENTRY(entry));
        gtk_editable_set_text(GTK_EDITABLE(entry),"alpha");g_signal_emit_by_name(entry,"activate");
        CHECK(Selected(buffer,"alpha"));
    } else {
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,"alpha") && !result.wrapped);
        CHECK(result.byte_start==(size_t)(strstr(text,"alpha")-text));
        if(strcmp(name,"source-next")==0 || strcmp(name,"source-previous")==0 || strcmp(name,"source-wrap")==0) {
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,0,&result)==UMI_STATUS_OK);
            CHECK(Selected(buffer,"alpha") && !result.wrapped);
            CHECK(result.byte_start==(size_t)(strstr(strstr(text,"alpha")+5,"alpha")-text));
            if(strcmp(name,"source-previous")==0 || strcmp(name,"source-wrap")==0) {
                int backwards=strcmp(name,"source-previous")==0;
                CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,backwards,&result)==UMI_STATUS_OK);
                CHECK(result.byte_start==(size_t)(strstr(text,"alpha")-text) && result.wrapped==!backwards);
            }
        } else if(strcmp(name,"source-no-match")==0) {
            sentinel=result;
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"absent",1,0,&result)==UMI_STATUS_NOT_FOUND);
            CHECK(Selected(buffer,"alpha") && result.byte_start==sentinel.byte_start && result.byte_length==sentinel.byte_length);
        }
    }
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
    CHECK(after.reader.revision==before.reader.revision && after.reader.history_count==before.reader.history_count);
    CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
cleanup:
    g_free(text);return failed;
}

int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    int failed=0,matched=0;
    const char *name=argv[1];
    GtkWidget *panel=NULL,*button=NULL;
    GtkTextBuffer *buffer=NULL;
    GtkEventController *keys=NULL;
    char *text=NULL;
    UmiLearningCatalogueFindMatch result={71U,72U,73U,1},sentinel=result;
    UmiLearningCatalogueGtkState before,after;
    CHECK(Fixture(argv[2]));
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0});CHECK(panel!=NULL);g_object_ref_sink(panel);
    UmiTestExpandSections(panel);
    GtkWidget *view=Find(panel,"learning.catalogue.content");CHECK(GTK_IS_TEXT_VIEW(view));
    buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    if(strcmp(name,"empty")==0) {
        matched=1;CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,0,&result)==UMI_STATUS_NOT_FOUND);
        CHECK(result.reader_revision==sentinel.reader_revision);goto cleanup;
    }
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2])==UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4Search(panel,"guide")==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK);
    if(strcmp(name,"busy")==0) {
        matched=1;CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_BUSY);
        CHECK(result.byte_start==sentinel.byte_start);CHECK(Finish());goto cleanup;
    }
    CHECK(Finish());CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"start")==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.has_reading_structure);
    text=Text(buffer);CHECK(text!=NULL);
    if(strncmp(name,"source-",7U)==0) {
        matched=1;CHECK(SourceCase(name,argv[2],panel,buffer)==0);goto cleanup;
    }
    if(strcmp(name,"invalid-query")==0 || strcmp(name,"capacity")==0) {
        matched=1;
        if(strcmp(name,"capacity")==0) {
            char query[UMI_LEARNING_READING_FIND_CAPACITY];memset(query,'a',sizeof(query));
            CHECK(UmiLearningCatalogueGtk4FindText(panel,query,1,0,&result)==UMI_STATUS_CAPACITY_EXCEEDED);
        } else {
            CHECK(UmiLearningCatalogueGtk4FindText(NULL,"alpha",1,0,&result)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4FindText(panel,NULL,1,0,&result)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"",1,0,&result)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"\xc3",1,0,&result)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",2,0,&result)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",1,2,&result)==UMI_STATUS_INVALID_ARGUMENT);
        }
        CHECK(result.reader_revision==sentinel.reader_revision && result.byte_start==sentinel.byte_start &&
            result.byte_length==sentinel.byte_length && result.wrapped==sentinel.wrapped);goto cleanup;
    }
    if(strcmp(name,"retired")==0) {
        matched=1;UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_INVALID_STATE);goto cleanup;
    }
    if(strcmp(name,"next")==0 || strcmp(name,"previous")==0 || strcmp(name,"wrapped")==0 ||
        strcmp(name,"ascii-insensitive")==0 || strcmp(name,"case-sensitive")==0) {
        matched=1;int sensitive=strcmp(name,"case-sensitive")==0;
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",sensitive,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,sensitive?"alpha":"Alpha") && !result.wrapped);
        CHECK(result.byte_start==(size_t)(strstr(text,sensitive?"alpha":"Alpha")-text));
        if(strcmp(name,"next")==0 || strcmp(name,"previous")==0) {
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_OK);
            CHECK(Selected(buffer,"alpha") && !result.wrapped);
            if(strcmp(name,"previous")==0) {
                CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,1,&result)==UMI_STATUS_OK);
                CHECK(Selected(buffer,"Alpha") && !result.wrapped);
            }
        } else if(strcmp(name,"wrapped")==0) {
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,1,&result)==UMI_STATUS_OK);
            CHECK(Selected(buffer,"alpha") && result.wrapped);
            CHECK(result.byte_start==(size_t)(strstr(text,"alpha_call")-text));
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_OK);
            CHECK(Selected(buffer,"Alpha") && result.wrapped);
        }
    } else if(strcmp(name,"unicode")==0 || strcmp(name,"nonascii-case")==0) {
        matched=1;CHECK(UmiLearningCatalogueGtk4FindText(panel,"\xce\xa9",0,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,"\xce\xa9") && result.byte_length==2U);
        CHECK(result.byte_start==(size_t)(strstr(text,"\xce\xa9")-text));
        if(strcmp(name,"nonascii-case")==0) {
            sentinel=result;
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"\xcf\x89",0,0,&result)==UMI_STATUS_NOT_FOUND);
            CHECK(Selected(buffer,"\xce\xa9") && result.byte_start==sentinel.byte_start);
        } else {
            CHECK(UmiLearningCatalogueGtk4FindText(panel,"caf\xc3\xa9",1,0,&result)==UMI_STATUS_OK);
            CHECK(Selected(buffer,"caf\xc3\xa9") && result.byte_length==5U);
        }
    } else if(strcmp(name,"no-match")==0 || strcmp(name,"hidden-content")==0) {
        matched=1;CHECK(UmiLearningCatalogueGtk4FindText(panel,"beta",1,0,&result)==UMI_STATUS_OK);sentinel=result;
        CHECK(UmiLearningCatalogueGtk4FindText(panel,strcmp(name,"no-match")==0?"missing":"hidden_secret",1,0,&result)==UMI_STATUS_NOT_FOUND);
        CHECK(Selected(buffer,"beta") && result.byte_start==sentinel.byte_start && result.byte_length==sentinel.byte_length);
    } else if(strcmp(name,"history")==0) {
        matched=1;CHECK(UmiLearningCatalogueGtk4Follow(panel,"example.c")==UMI_STATUS_OK && Finish());
        /* Source Find now uses the verified snapshot; retain the old expectation for review. */
#if 0
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_NOT_FOUND);
#endif
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,"alpha"));
        CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result)==UMI_STATUS_OK);
        CHECK(Selected(buffer,"Alpha"));goto cleanup;
    } else if(strcmp(name,"controls")==0 || strcmp(name,"enter")==0 || strcmp(name,"notice-reentrant")==0) {
        matched=1;GtkWidget *entry=Find(panel,"learning.catalogue.find-query");CHECK(GTK_IS_ENTRY(entry));
        gtk_editable_set_text(GTK_EDITABLE(entry),strcmp(name,"notice-reentrant")==0?"missing":"beta");
        GtkWidget *next=Find(panel,"learning.catalogue.find-next");CHECK(GTK_IS_BUTTON(next));
        Observer observer={panel,0U,0,0,UMI_STATUS_OK};
        GtkWidget *label=Find(panel,"learning.catalogue.status");CHECK(label!=NULL);
        gulong signal=strcmp(name,"notice-reentrant")==0?
            g_signal_connect(label,"notify::label",G_CALLBACK(Notice),&observer):0U;
        if(strcmp(name,"enter")==0)g_signal_emit_by_name(entry,"activate");
        else g_signal_emit_by_name(next,"clicked");
        if(signal!=0U)g_signal_handler_disconnect(label,signal);
        if(strcmp(name,"notice-reentrant")==0)CHECK(observer.calls==1U && observer.busy && observer.nested==UMI_STATUS_BUSY);
        else CHECK(Selected(buffer,"beta"));
    } else if(strcmp(name,"key")==0 || strcmp(name,"modified-key")==0) {
        matched=1;keys=Keys(panel);CHECK(keys!=NULL);
        GtkWidget *section=Find(panel,"learning.catalogue.find-section");CHECK(GTK_IS_EXPANDER(section));
        gtk_expander_set_expanded(GTK_EXPANDER(section),FALSE);
        if(strcmp(name,"key")==0) {
            CHECK(Key(keys,GDK_KEY_f,GDK_CONTROL_MASK));CHECK(gtk_expander_get_expanded(GTK_EXPANDER(section)));
        } else {
            CHECK(!Key(keys,GDK_KEY_f,0));CHECK(!Key(keys,GDK_KEY_f,GDK_CONTROL_MASK|GDK_SHIFT_MASK));
            CHECK(!Key(keys,GDK_KEY_F1,GDK_CONTROL_MASK));CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(section)));
        }
    } else if(strcmp(name,"reentrant")==0 || strcmp(name,"retire-selection")==0) {
        matched=1;Observer observer={panel,0U,strcmp(name,"retire-selection")==0,0,UMI_STATUS_OK};
        gulong signal=g_signal_connect(buffer,"mark-set",G_CALLBACK(MarkSet),&observer);
        UmiStatus status=UmiLearningCatalogueGtk4FindText(panel,"alpha",0,0,&result);
        g_signal_handler_disconnect(buffer,signal);
        CHECK(status==UMI_STATUS_OK && observer.calls>0U && observer.busy && observer.nested==UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK && after.retired==observer.retire && !after.busy);
    } else if(strcmp(name,"retained")==0) {
        matched=1;button=Find(panel,"learning.catalogue.find-next");CHECK(button!=NULL);g_object_ref(button);
        UmiLearningCatalogueGtk4Retire(panel);g_clear_object(&panel);
        g_signal_emit_by_name(button,"clicked");CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);goto cleanup;
    }
    CHECK(matched);CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
    CHECK(after.reader.revision==before.reader.revision && after.reader.history_count==before.reader.history_count);
    CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
cleanup:
    g_free(text);g_clear_object(&keys);g_clear_object(&button);
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    if(!Finish())failed=1;
    g_clear_object(&panel);return failed;
}
