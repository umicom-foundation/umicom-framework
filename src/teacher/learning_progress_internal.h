/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_progress_internal.h
 *
 * PURPOSE:
 *   Keep journal projection and storage ownership private.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_PROGRESS_INTERNAL_H
#define UMICOM_LEARNING_PROGRESS_INTERNAL_H
#include "umicom/teacher/learning_progress.h"

#define LP_WIRE_CAPACITY 640U
#define LP_KEY_CAPACITY 192U
typedef struct LpState {
    size_t count;
    UmiLearningProgressEvent events[UMI_LEARNING_PROGRESS_MAX_EVENTS];
    char wire[UMI_LEARNING_PROGRESS_MAX_EVENTS][LP_WIRE_CAPACITY];
} LpState;
struct UmiLearningProgress {
    UmiDataServer *server;
    char learner_id[UMI_TEACHER_ID_CAPACITY];
    char prefix[128];
    LpState *state;
    int recovery_required;
};
UmiStatus LpToken(const char *value);
UmiStatus LpKey(const UmiLearningProgress *progress, const char *suffix,
    char out[LP_KEY_CAPACITY]);
UmiStatus LpEncode(const UmiLearningProgressEvent *event, char out[LP_WIRE_CAPACITY]);
/* Caller owns a Data Server transaction. out is a zeroed private candidate. */
UmiStatus LpLoad(UmiLearningProgress *progress, LpState *out);
UmiStatus LpRollback(UmiLearningProgress *progress, UmiStatus prior);
UmiStatus LpDecode(const char *wire, uint64_t sequence, UmiLearningProgressEvent *out);
/* Write the appended suffix and header, then finish the caller-owned transaction. */
UmiStatus LpStore(UmiLearningProgress *progress, const LpState *candidate, size_t first);
#endif
