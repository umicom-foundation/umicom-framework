/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education_workspace/workspace.h
 * PURPOSE: Provide the Framework-owned Education Workspace capability.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_WORKSPACE_WORKSPACE_H
#define UMICOM_EDUCATION_WORKSPACE_WORKSPACE_H
#include <stdbool.h>
#include "umicom/education_workspace/catalogue.h"
#include "umicom/data/data_server.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDUCATION_ID_CAPACITY 49U
#define UMI_EDUCATION_NAME_CAPACITY 97U
#define UMI_EDUCATION_NOTE_CAPACITY 1024U
#define UMI_EDUCATION_MAX_EVENTS 256U
/* A workspace owns its learner projection; it borrows an exclusive Data Server
 * connection. One owner thread performs all operations. Separate connections
 * may edit the same learner: stale revisions are rejected, never merged.
 * Open/Reload only read. Calls never compile, launch, grade code or use AI.
 * Local identifiers are not authenticated accounts or assessment certificates. */
typedef struct UmiEducationWorkspace UmiEducationWorkspace;
typedef struct UmiEducationProgress {
    bool read;
    bool hintViewed;
    bool prerequisitesMet;
    bool quizPassed;
    uint32_t attempts;
    uint32_t latestScore;
    uint32_t bestScore;
    char note[UMI_EDUCATION_NOTE_CAPACITY];
} UmiEducationProgress;
typedef struct UmiEducationSnapshot {
    char learnerId[UMI_EDUCATION_ID_CAPACITY];
    char displayName[UMI_EDUCATION_NAME_CAPACITY];
    uint64_t revision;
    uint32_t readLessons;
    uint32_t passedQuizzes;
    uint32_t quizCompletionPercent;
    uint32_t remainingChanges;
    bool recoveryRequired;
} UmiEducationSnapshot;
typedef struct UmiEducationFeedback {
    uint32_t score;
    bool passed;
    bool correct[UMI_EDUCATION_QUESTIONS];
    /* Immutable explanations remain available even when a response is wrong. */
    const char *explanations[UMI_EDUCATION_QUESTIONS];
} UmiEducationFeedback;
/* id: 1..48 ASCII letters, digits, hyphen or underscore. displayName: nonempty
 * valid UTF-8, at most 96 bytes. Existing learners retain their stored name.
 * Fresh state is not persisted until the first explicit learning action. */
UmiStatus UmiEducationOpen(UmiDataServer *server, const char *id,
    const char *displayName, UmiEducationWorkspace **outWorkspace);
void UmiEducationClose(UmiEducationWorkspace *workspace);
UmiStatus UmiEducationReload(UmiEducationWorkspace *workspace);
UmiStatus UmiEducationSnapshotRead(const UmiEducationWorkspace *workspace,
    UmiEducationSnapshot *outSnapshot);
UmiStatus UmiEducationProgressRead(const UmiEducationWorkspace *workspace,
    const char *lessonId, UmiEducationProgress *outProgress);
UmiStatus UmiEducationMarkRead(UmiEducationWorkspace *workspace, const char *lessonId);
UmiStatus UmiEducationRevealHint(UmiEducationWorkspace *workspace, const char *lessonId);
UmiStatus UmiEducationSaveNote(UmiEducationWorkspace *workspace,
    const char *lessonId, const char *note);
/* All three answers must be in [0,2]. Requires this lesson to be marked read
 * and the preceding quiz in the same course to have passed. Full marks are
 * needed to pass; retries keep the previous best score. Never marks code as
 * tested. Each accepted submission is a distinct, durable practice attempt. */
UmiStatus UmiEducationSubmitQuiz(UmiEducationWorkspace *workspace,
    const char *lessonId, const uint32_t answers[UMI_EDUCATION_QUESTIONS],
    UmiEducationFeedback *outFeedback);
/* Fresh capacity probe: output==NULL, capacity==0 returns needed bytes including
 * NUL in outRequired. Too-small output is cleared, never partially published.
 * HTML escapes all learner text. The report is a self-study record only. */
UmiStatus UmiEducationReportHtml(const UmiEducationWorkspace *workspace,
    char *output, size_t capacity, size_t *outRequired);
/* Immutable supplied branding for native views and self-contained reports. */
const char *UmiEducationLogoDataUri(void);
const char *UmiEducationIconDataUri(void);
#ifdef __cplusplus
}
#endif
#endif
