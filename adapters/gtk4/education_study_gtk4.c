/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/education_study_gtk4.c
 * PURPOSE: Present owned study routes without bypassing the existing note guard.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "education_study_private.h"
#include "umicom/education_workspace/study.h"
#include <inttypes.h>
#include <string.h>
#define STUDY_DATA "umicom-education-study-pane"
typedef struct StudyPane {
    GtkEntry *search;
    GtkDropDown *course,*filter;
    GtkLabel *output;
    GtkWidget *nextButton;
    UmiEducationStudy *capture;
    size_t nextIndex;
} StudyPane;
static StudyPane *Pane(UmiEducationGtkPanel *panel)
{return g_object_get_data(G_OBJECT(panel->root),STUDY_DATA);}
static void Release(gpointer data)
{
    StudyPane *pane=data;
    UmiEducationStudyDestroy(pane->capture);g_free(pane);
}
static GtkWidget *Label(const char *text)
{
    GtkWidget *label=gtk_label_new(text);gtk_label_set_wrap(GTK_LABEL(label),TRUE);
    gtk_label_set_xalign(GTK_LABEL(label),0.0F);gtk_label_set_selectable(GTK_LABEL(label),TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label),90);return label;
}
static void Tag(GtkWidget *widget,const char *id)
{g_object_set_data_full(G_OBJECT(widget),"umicom-automation-id",g_strdup(id),g_free);}
void EwGtkStudyInvalidate(UmiEducationGtkPanel *panel)
{
    StudyPane *pane=Pane(panel);if(pane==NULL)return;
    UmiEducationStudyDestroy(pane->capture);pane->capture=NULL;pane->nextIndex=SIZE_MAX;
    gtk_widget_set_sensitive(pane->nextButton,FALSE);
    gtk_label_set_text(pane->output,"Capture a study snapshot after choosing your filters. It reads the loaded learner record; it does not reload storage or grant progress.");
}
static void Changed(GObject *object,GParamSpec *spec,gpointer data)
{(void)object;(void)spec;EwGtkStudyInvalidate(data);}
static void TextChanged(GtkEditable *editable,gpointer data)
{(void)editable;EwGtkStudyInvalidate(data);}
static void Capture(GtkButton *button,gpointer data)
{
    (void)button;UmiEducationGtkPanel *panel=data;StudyPane *pane=Pane(panel);
    if(panel->workspace==NULL){EwGtkMessage(panel,"Open a learner record before capturing its study route.");return;}
    guint course=gtk_drop_down_get_selected(pane->course),filter=gtk_drop_down_get_selected(pane->filter);
    if(course>UmiEducationCourseCount()||filter>UMI_EDUCATION_STUDY_UNREAD){EwGtkMessage(panel,"Choose a course and study filter.");return;}
    UmiEducationStudy *candidate=NULL;UmiEducationStudyMatches matches={0};UmiEducationStudyRoute route={0};
    UmiEducationSnapshot snapshot={0};
    UmiEducationStudyQuery query={course==0U?"":UmiEducationCourseAt((size_t)course-1U)->id,
        gtk_editable_get_text(GTK_EDITABLE(pane->search)),(UmiEducationStudyFilter)filter};
    UmiStatus status=UmiEducationStudyCapture(panel->workspace,&candidate);
    if(status==UMI_STATUS_OK)status=UmiEducationStudySearch(candidate,&query,&matches);
    if(status==UMI_STATUS_OK)status=UmiEducationStudySnapshotRead(candidate,&snapshot);
    if(status==UMI_STATUS_OK&&!snapshot.recoveryRequired)
        status=UmiEducationStudyPlan(candidate,UmiEducationLessonAt(panel->lessonIndex)->id,&route);
    if(status!=UMI_STATUS_OK){UmiEducationStudyDestroy(candidate);EwGtkStatus(panel,status,"");return;}
    GString *text=g_string_new(NULL);
    g_string_append_printf(text,"Captured learner: %s | revision %" PRIu64 "\n%zu matching lessons. Private notes and quiz answers are not searched.\n\n",snapshot.learnerId,snapshot.revision,matches.count);
    for(size_t i=0U;i<matches.count;++i) {
        UmiEducationStudyItem item={0};
        if(UmiEducationStudyItemRead(candidate,matches.indices[i],&item)!=UMI_STATUS_OK)continue;
        g_string_append_printf(text,"%s | %s\n%s; best %u/100, %u attempts\n\n",item.lesson->id,item.lesson->title,
            UmiEducationStudyActionText(item.action),item.bestScore,item.attempts);
    }
    if(snapshot.recoveryRequired)g_string_append(text,"Storage recovery is required. No next step can be recommended from this capture.\n");
    else {
        g_string_append_printf(text,"Route to the lesson currently selected above: %s\n%zu quizzes remaining; %u catalogue minutes, not measured study time.\n",UmiEducationLessonAt(panel->lessonIndex)->title,route.count,route.estimatedMinutes);
        for(size_t i=0U;i<route.count;++i)g_string_append_printf(text,"%zu. %s\n",i+1U,UmiEducationLessonAt(route.indices[i])->title);
        if(route.count==0U)g_string_append(text,"This course goal's practice quizzes are passed. This is not a compiled-project result or certificate.\n");
    }
    UmiEducationStudyDestroy(pane->capture);pane->capture=candidate;
    pane->nextIndex=!snapshot.recoveryRequired&&route.count>0U?route.indices[0]:SIZE_MAX;
    gtk_widget_set_sensitive(pane->nextButton,pane->nextIndex!=SIZE_MAX);
    gtk_label_set_text(pane->output,text->str);g_string_free(text,TRUE);
    EwGtkMessage(panel,"Study snapshot captured. No progress was written.");
}
static void Next(GtkButton *button,gpointer data)
{
    (void)button;UmiEducationGtkPanel *panel=data;StudyPane *pane=Pane(panel);
    if(pane->capture==NULL||pane->nextIndex>=UMI_EDUCATION_LESSONS)return;
    if(panel->dirty){EwGtkMessage(panel,"Save or discard learning-note edits before opening another lesson.");return;}
    UmiEducationSnapshot captured={0},current={0};
    UmiStatus status=UmiEducationStudySnapshotRead(pane->capture,&captured);
    if(status==UMI_STATUS_OK)status=UmiEducationSnapshotRead(panel->workspace,&current);
    if(status!=UMI_STATUS_OK||current.recoveryRequired||current.revision!=captured.revision||strcmp(current.learnerId,captured.learnerId)!=0) {
        EwGtkStudyInvalidate(panel);EwGtkMessage(panel,"The loaded learner changed. Capture a new study snapshot.");return;
    }
    /* Use the original selector and its existing callback. Do not reimplement
     * lesson navigation or its unsaved-note protection. */
    gtk_drop_down_set_selected(panel->lessonSelect,(guint)pane->nextIndex);
}
static void Library(GtkButton *button,gpointer data)
{
    (void)button;UmiEducationGtkPanel *panel=data;StudyPane *pane=Pane(panel);
    UmiEducationLibraryMatches matches={0};
    UmiStatus status=UmiEducationLibrarySearch(gtk_editable_get_text(GTK_EDITABLE(pane->search)),&matches);
    if(status!=UMI_STATUS_OK){EwGtkStatus(panel,status,"");return;}
    EwGtkStudyInvalidate(panel);GString *text=g_string_new("Guide catalogue: source paths, not verified installed files.\nOpen docs/learning/LEARNING_LIBRARY.html beside the supplied guides for local links.\n\n");
    for(size_t i=0U;i<matches.count;++i) {
        const UmiEducationLibraryEntry *e=UmiEducationLibraryAt(matches.indices[i]);
        g_string_append_printf(text,"%s | %s\ndocs/learning/%s\n%s\n\n",e->category,e->title,e->guideFile,e->boundary);
    }
    gtk_label_set_text(pane->output,text->str);g_string_free(text,TRUE);
    EwGtkMessage(panel,"Guide metadata displayed. No file, browser or compiler was opened.");
}
static GtkWidget *Button(GtkWidget *box,const char *text,const char *id,GCallback callback,UmiEducationGtkPanel *panel)
{
    GtkWidget *button=gtk_button_new_with_label(text);Tag(button,id);
    /* The parent already disconnects every widget handler with panel as its
     * data before freeing that panel, even when a control is retained. The
     * child state deliberately does not hold an owning parent pointer. */
    g_signal_connect(button,"clicked",callback,panel);gtk_box_append(GTK_BOX(box),button);return button;
}
void EwGtkStudyAppend(UmiEducationGtkPanel *panel)
{
    if(Pane(panel)!=NULL)return;
    StudyPane *pane=g_new0(StudyPane,1U);pane->nextIndex=SIZE_MAX;
    GtkWidget *expander=gtk_expander_new("Study route and learning library");Tag(expander,"education.study");
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);gtk_expander_set_child(GTK_EXPANDER(expander),box);
    gtk_box_append(GTK_BOX(box),Label("Select a goal in the existing lesson selector above, then capture its unfinished prerequisites. Search changes the displayed list, not the goal or the course rules."));
    pane->search=GTK_ENTRY(gtk_entry_new());Tag(GTK_WIDGET(pane->search),"education.study.search");
    gtk_entry_set_placeholder_text(pane->search,"Search lessons or guides (up to 128 UTF-8 bytes)");
    gtk_box_append(GTK_BOX(box),GTK_WIDGET(pane->search));
    size_t courseCount=UmiEducationCourseCount();
    const char **courses=g_new0(const char *,courseCount+2U);
    courses[0]="All courses";
    for(size_t i=0U;i<courseCount;++i)courses[i+1U]=UmiEducationCourseAt(i)->title;
    const char *filters[]={"All lessons","Ready to study","Blocked","Practice quiz passed","Not marked read",NULL};
    pane->course=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(courses));g_free(courses);Tag(GTK_WIDGET(pane->course),"education.study.course");
    pane->filter=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(filters));Tag(GTK_WIDGET(pane->filter),"education.study.filter");
    gtk_box_append(GTK_BOX(box),GTK_WIDGET(pane->course));gtk_box_append(GTK_BOX(box),GTK_WIDGET(pane->filter));
    Button(box,"Capture study snapshot","education.study.capture",G_CALLBACK(Capture),panel);
    pane->nextButton=Button(box,"Open next step","education.study.next",G_CALLBACK(Next),panel);
    Button(box,"Search guide library","education.study.library",G_CALLBACK(Library),panel);
    pane->output=GTK_LABEL(Label(""));Tag(GTK_WIDGET(pane->output),"education.study.output");
    gtk_box_append(GTK_BOX(box),GTK_WIDGET(pane->output));gtk_box_append(GTK_BOX(panel->root),expander);
    g_object_set_data_full(G_OBJECT(panel->root),STUDY_DATA,pane,Release);
    g_signal_connect(pane->search,"changed",G_CALLBACK(TextChanged),panel);
    g_signal_connect(pane->course,"notify::selected",G_CALLBACK(Changed),panel);
    g_signal_connect(pane->filter,"notify::selected",G_CALLBACK(Changed),panel);
    EwGtkStudyInvalidate(panel);
}
