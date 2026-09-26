/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education_workspace/private.h
 * PURPOSE: Provide the Framework-owned Education Workspace capability.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_WORKSPACE_PRIVATE_H
#define UMICOM_EDUCATION_WORKSPACE_PRIVATE_H
#include "umicom/education_workspace/workspace.h"
#define EW_RECORD_CAPACITY 3072U
#define EW_NAMESPACE_CAPACITY 96U
typedef enum EwAction { EW_READ=1, EW_HINT=2, EW_NOTE=3, EW_QUIZ=4 } EwAction;
typedef struct EwState {
    uint64_t revision;
    char displayName[UMI_EDUCATION_NAME_CAPACITY];
    UmiEducationProgress lessons[UMI_EDUCATION_LESSONS];
} EwState;
struct UmiEducationWorkspace {
    UmiDataServer *server;
    char learnerId[UMI_EDUCATION_ID_CAPACITY];
    char prefix[EW_NAMESPACE_CAPACITY];
    EwState state;
    bool recoveryRequired;
};
/* Private helpers are shared between the model and persistence translation
 * units. No application is permitted to include this header. */
bool EwTextValid(const char *text, size_t capacity, bool allowEmpty);
bool EwIdValid(const char *text);
size_t EwLessonIndex(const char *id);
UmiStatus EwApply(EwState *state, EwAction action, size_t index,
    const char *payload, UmiEducationFeedback *feedback);
UmiStatus EwLoad(UmiEducationWorkspace *workspace, EwState *outState);
UmiStatus EwCommit(UmiEducationWorkspace *workspace, EwAction action,
    size_t index, const char *payload, UmiEducationFeedback *feedback);
#endif
