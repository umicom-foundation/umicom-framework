/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Framework-owned learning output; applications do not duplicate this service. */
#include "private.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "report_brand.inc"
/* Two-pass writing computes an exact required size before exposing any output. */
typedef struct Writer {char *output;size_t used;} Writer;
static void Text(Writer *w,const char *s)
{
    size_t n=strlen(s);if (w->output!=NULL) memcpy(w->output+w->used,s,n);w->used+=n;
}
static void Escaped(Writer *w,const char *s)
{
    for (;*s!='\0';++s) {
        switch (*s) {
        case '&':Text(w,"&amp;");break;case '<':Text(w,"&lt;");break;
        case '>':Text(w,"&gt;");break;case '"':Text(w,"&quot;");break;
        case '\'':Text(w,"&#39;");break;
        default:if(w->output!=NULL)w->output[w->used]=*s;++w->used;break;
        }
    }
}
static void Render(const UmiEducationWorkspace *w,Writer *writer)
{
    UmiEducationSnapshot snapshot;char line[256];
    (void)UmiEducationSnapshotRead(w,&snapshot);
    Text(writer,"<!doctype html><html lang=\"en-GB\"><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Umicom learning record</title><link rel=\"icon\" href=\"");
    Text(writer,EDUCATION_ICON);Text(writer,"\"><style>body{font:17px/1.6 system-ui;margin:0;color:#203243;background:#f2f5f7}header{padding:20px;background:#102b3d}header img{width:175px}main{max-width:900px;margin:auto;padding:30px}section{padding:24px;background:white;margin:22px 0;border-radius:10px}.note{white-space:pre-wrap;overflow-wrap:anywhere}h1{line-height:1.2}.notice{padding:18px;border-left:4px solid #256c80;background:#e7f2f5}@media print{body{background:white}section{break-inside:avoid}}</style><header><img alt=\"Umicom\" src=\"");
    Text(writer,EDUCATION_LOGO);Text(writer,"\"></header><main><h1>Your learning record</h1><h2>");
    Escaped(writer,snapshot.displayName);Text(writer,"</h2><p>Learner: ");Escaped(writer,snapshot.learnerId);
    int n=snprintf(line,sizeof line," · revision %" PRIu64 "</p><p>%u of %u quizzes passed (%u%%); %u lessons marked read.</p>",snapshot.revision,snapshot.passedQuizzes,UMI_EDUCATION_LESSONS,snapshot.quizCompletionPercent,snapshot.readLessons);
    if (n>0 && (size_t)n<sizeof line) Text(writer,line);
    Text(writer,"<p class=\"notice\">This is a local self-study quiz record, not an identity-verified certificate. It does not claim that edited C projects were compiled or tested. Notes and source data are plaintext.</p>");
    if (snapshot.recoveryRequired) Text(writer,"<p class=\"notice\">Recovery is required. This report shows the last confirmed local projection; reopen the storage to establish its durable state.</p>");
    for(size_t i=0U;i<UMI_EDUCATION_LESSONS;++i) {
        const UmiEducationLesson *lesson=UmiEducationLessonAt(i);
        const UmiEducationProgress *p=&w->state.lessons[i];
        Text(writer,"<section><h2>");Escaped(writer,lesson->title);Text(writer,"</h2><p>");
        Escaped(writer,lesson->courseId);Text(writer," · ");Text(writer,p->quizPassed?"Quiz passed":"Quiz not yet passed");
        n=snprintf(line,sizeof line," · attempts %u · latest %u/100 · best %u/100</p>",p->attempts,p->latestScore,p->bestScore);
        if(n>0&&(size_t)n<sizeof line) Text(writer,line);
        Text(writer,"<p>");Text(writer,p->read?"Marked read":"Not marked read");Text(writer,p->hintViewed?" · hint viewed":" · hint not viewed");Text(writer,"</p><div class=\"note\">");
        Escaped(writer,p->note);Text(writer,"</div></section>");
    }
    Text(writer,"<footer>Umicom Foundation · Learn, practise, check and explain.</footer></main></html>");
}
UmiStatus UmiEducationReportHtml(const UmiEducationWorkspace *w,char *output,
    size_t capacity,size_t *required)
{
    if(output!=NULL&&capacity!=0U) output[0]='\0';
    if(w==NULL||required==NULL||(output==NULL&&capacity!=0U)) return UMI_STATUS_INVALID_ARGUMENT;
    Writer writer={NULL,0U};Render(w,&writer);*required=writer.used+1U;
    if(output==NULL) return UMI_STATUS_OK;
    if(capacity<*required) return UMI_STATUS_CAPACITY_EXCEEDED;
    writer.output=output;writer.used=0U;Render(w,&writer);output[writer.used]='\0';return UMI_STATUS_OK;
}

const char *UmiEducationLogoDataUri(void) {return EDUCATION_LOGO;}
const char *UmiEducationIconDataUri(void) {return EDUCATION_ICON;}
