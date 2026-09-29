/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education_workspace/study.h
 * PURPOSE: Inspect learning progress and plan a route without recording completion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_WORKSPACE_STUDY_H
#define UMICOM_EDUCATION_WORKSPACE_STUDY_H
#include "umicom/education_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDUCATION_SEARCH_CAPACITY 129U
/* This is an owned copy of progress, not a second learner store. Capture must
 * run on the workspace owner thread. An immutable capture can be shared for
 * reads while its owner keeps it alive; Destroy requires exclusive ownership.
 * No call reloads storage, writes progress, runs a compiler, grants credit or
 * verifies a learner's identity. Private learning notes are not captured. */
typedef struct UmiEducationStudy UmiEducationStudy;
typedef enum UmiEducationStudyAction {
    UMI_EDUCATION_STUDY_READ = 0,
    UMI_EDUCATION_STUDY_QUIZ = 1,
    UMI_EDUCATION_STUDY_PREREQUISITE = 2,
    UMI_EDUCATION_STUDY_COMPLETE = 3
} UmiEducationStudyAction;
typedef struct UmiEducationStudyItem {
    /* Catalogue storage is immutable and lasts for the process. Do not free it. */
    const UmiEducationLesson *lesson;
    size_t catalogueIndex;
    bool read, hintViewed, prerequisitesMet, quizPassed;
    uint32_t attempts, latestScore, bestScore;
    UmiEducationStudyAction action;
} UmiEducationStudyItem;
typedef enum UmiEducationStudyFilter {
    UMI_EDUCATION_STUDY_ALL = 0,
    UMI_EDUCATION_STUDY_READY = 1,
    UMI_EDUCATION_STUDY_BLOCKED = 2,
    UMI_EDUCATION_STUDY_PASSED = 3,
    UMI_EDUCATION_STUDY_UNREAD = 4
} UmiEducationStudyFilter;
typedef struct UmiEducationStudyQuery {
    /* Empty strings mean every course/no text filter. UTF-8 byte substring,
     * folding ASCII A-Z only: no regex, locale collation or stemming. Search
     * covers ID, course, title, explanation, practice and project ID. It does
     * not inspect hints, quiz answers or private notes. */
    const char *courseId;
    const char *text;
    UmiEducationStudyFilter filter;
} UmiEducationStudyQuery;
typedef struct UmiEducationStudyMatches {
    size_t count;
    size_t indices[UMI_EDUCATION_LESSONS];
} UmiEducationStudyMatches;
typedef struct UmiEducationStudyRoute {
    uint64_t capturedRevision;
    size_t goalIndex;
    size_t count;
    size_t indices[UMI_EDUCATION_LESSONS];
    /* Sum of catalogue estimates for unfinished lessons, not measured time. */
    uint32_t estimatedMinutes;
} UmiEducationStudyRoute;
/* *outStudy must be NULL. Failure preserves the output and never frees caller
 * storage. Scalar/structure outputs below also remain unchanged on failure. */
UmiStatus UmiEducationStudyCapture(const UmiEducationWorkspace *workspace,
    UmiEducationStudy **outStudy);
void UmiEducationStudyDestroy(UmiEducationStudy *study);
UmiStatus UmiEducationStudySnapshotRead(const UmiEducationStudy *study,
    UmiEducationSnapshot *outSnapshot);
UmiStatus UmiEducationStudyItemRead(const UmiEducationStudy *study,size_t index,
    UmiEducationStudyItem *outItem);
UmiStatus UmiEducationStudySearch(const UmiEducationStudy *study,
    const UmiEducationStudyQuery *query,UmiEducationStudyMatches *outMatches);
/* The route contains unfinished quizzes from this course's beginning through
 * the selected goal, in canonical prerequisite order. Courses are independent;
 * this does not invent a dependency between C, Assembly and Framework courses.
 * A recovery-required capture cannot recommend a mutation. */
UmiStatus UmiEducationStudyPlan(const UmiEducationStudy *study,const char *goalId,
    UmiEducationStudyRoute *outRoute);
/* NOT_FOUND means no unfinished lesson in the requested course, not a storage
 * failure or qualification certificate. Empty courseId searches all courses. */
UmiStatus UmiEducationStudyNext(const UmiEducationStudy *study,const char *courseId,
    UmiEducationStudyItem *outItem);
const char *UmiEducationStudyActionText(UmiEducationStudyAction action);

/* Curated documentation metadata is immutable, not a filesystem scan. Presence
 * in this catalogue does not attest that a file was installed, that the reader
 * read it, or that the demonstrated feature passed platform acceptance. */
typedef struct UmiEducationLibraryEntry {
    const char *id;
    const char *title;
    const char *category;
    const char *guideFile; /* Relative to docs/learning; no directory traversal. */
    const char *examplePath; /* Relative to the Framework source root. */
    const char *boundary;
} UmiEducationLibraryEntry;
#define UMI_EDUCATION_LIBRARY_LIMIT 64U
typedef struct UmiEducationLibraryMatches {
    size_t count;
    size_t indices[UMI_EDUCATION_LIBRARY_LIMIT];
} UmiEducationLibraryMatches;
size_t UmiEducationLibraryCount(void);
const UmiEducationLibraryEntry *UmiEducationLibraryAt(size_t index);
UmiStatus UmiEducationLibraryValidate(void);
UmiStatus UmiEducationLibrarySearch(const char *text,UmiEducationLibraryMatches *outMatches);
/* Report generation uses the same catalogue as the native search. It performs
 * no file I/O. Place a generated index beside the guides for relative links.
 * Probe with output==NULL,capacity==0. required includes NUL. On insufficient
 * capacity output[0] is cleared and no partial HTML is published. */
UmiStatus UmiEducationLibraryHtml(char *output,size_t capacity,size_t *outRequired);
#ifdef __cplusplus
}
#endif
#endif
