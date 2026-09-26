/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/catalogue.c
 * PURPOSE: Publish the shared C, Assembly and Framework practicum.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education_workspace/catalogue.h"
#include <string.h>
static const UmiEducationCourse COURSES[] = {
    {"notes", "C through a notes application", "Values, contracts, ownership and focused tests.", 0U, 4U},
    {"assembly", "Read and test an Assembly routine", "Bits, calling conventions, loops and independent checks.", 4U, 4U},
    {"framework", "Compose a Framework notes application", "Service ownership, transactions, views and delivery.", 8U, 4U}
};
static const UmiEducationLesson LESSONS[] = {
#include "lessons_notes.inc"
#include "lessons_assembly.inc"
#include "lessons_framework.inc"
};
_Static_assert(sizeof LESSONS / sizeof LESSONS[0] == UMI_EDUCATION_LESSONS, "Lesson count must match the bounded learning record");
size_t UmiEducationCourseCount(void) {return sizeof COURSES / sizeof COURSES[0];}
const UmiEducationCourse *UmiEducationCourseAt(size_t i) {return i<UmiEducationCourseCount()?&COURSES[i]:NULL;}
const UmiEducationLesson *UmiEducationLessonAt(size_t i) {return i<UMI_EDUCATION_LESSONS?&LESSONS[i]:NULL;}
const UmiEducationLesson *UmiEducationLessonFind(const char *id)
{
    if (id!=NULL) for (size_t i=0U;i<UMI_EDUCATION_LESSONS;++i)
        if (strcmp(LESSONS[i].id,id)==0) return &LESSONS[i];
    return NULL;
}
UmiStatus UmiEducationCatalogueValidate(void)
{
    size_t covered=0U;
    for (size_t c=0U;c<UmiEducationCourseCount();++c) {
        if (COURSES[c].firstLesson!=covered || COURSES[c].lessonCount==0U ||
            COURSES[c].lessonCount>UMI_EDUCATION_LESSONS-covered) return UMI_STATUS_INVALID_STATE;
        for (size_t i=covered;i<covered+COURSES[c].lessonCount;++i) {
            const UmiEducationLesson *l=&LESSONS[i];
            if (strcmp(l->courseId,COURSES[c].id)!=0 || l->id[0]=='\0' || l->title[0]=='\0' ||
                l->explanation[0]=='\0' || l->practice[0]=='\0' || l->hint[0]=='\0' ||
                l->minutes==0U || strncmp(l->foundationId,"foundations.",12U)!=0)
                return UMI_STATUS_INVALID_STATE;
            for (size_t j=0U;j<i;++j) if (strcmp(l->id,LESSONS[j].id)==0) return UMI_STATUS_ALREADY_EXISTS;
            for (size_t q=0U;q<UMI_EDUCATION_QUESTIONS;++q) {
                if (l->questions[q].correctChoice>=UMI_EDUCATION_CHOICES ||
                    l->questions[q].prompt[0]=='\0' || l->questions[q].explanation[0]=='\0')
                    return UMI_STATUS_INVALID_STATE;
                for (size_t a=0U;a<UMI_EDUCATION_CHOICES;++a)
                    if (l->questions[q].choices[a][0]=='\0') return UMI_STATUS_INVALID_STATE;
            }
        }
        covered+=COURSES[c].lessonCount;
    }
    return covered==UMI_EDUCATION_LESSONS?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
