/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_progress_local.h
 *
 * PURPOSE:
 *   Capture explicit local checkpoint operations with independent storage ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEACHER_LEARNING_PROGRESS_LOCAL_H
#define UMICOM_TEACHER_LEARNING_PROGRESS_LOCAL_H
#include "umicom/teacher/learning_progress.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiLearningProgressLocalAction {
    UMI_LEARNING_PROGRESS_INSPECT = 1,
    UMI_LEARNING_PROGRESS_RECORD = 2
} UmiLearningProgressLocalAction;
typedef struct UmiLearningProgressLocalRequest {
    UmiLearningProgressLocalAction action;
    char database_path[UMI_PATH_CAPACITY];
    char learner_id[UMI_TEACHER_ID_CAPACITY];
    char checkpoint[UMI_TEACHER_ID_CAPACITY];
    /* Required for RECORD; empty for INSPECT. This is an opaque reference,
     * not a path, personal note, assessment grade or authenticated evidence. */
    char evidence_reference[UMI_TEACHER_ID_CAPACITY];
} UmiLearningProgressLocalRequest;
typedef struct UmiLearningProgressLocalReceipt {
    UmiLearningProgressLocalRequest request;
    UmiLearningReaderSnapshot selection;
    UmiLearningProgressSnapshot journal;
    UmiLearningProgressEvent event;
    UmiStatus status;
    int storage_opened;
    int journal_loaded;
    int write_attempted;
    int has_event;
} UmiLearningProgressLocalReceipt;

/* Validate copied fixed-size fields without storage access. Require an ordinary
 * nonempty absolute path, terminated buffers and the progress identifier policy.
 * INSPECT requires an empty evidence reference. No path is chosen implicitly.
 * The caller chooses a private location outside course and source repositories.
 * This admission does not inspect filesystem permissions, links or file type. */
UmiStatus UmiLearningProgressLocalValidate(const UmiLearningProgressLocalRequest *request);

/* Execute one explicit request, owning its Data Server connection and journal
 * from open to close on the calling thread. May block; use a host worker.
 * The reader is borrowed, serialized and unchanged. Record binds to its already
 * verified document, not the latest disk bytes. Only GUIDE/LESSON are accepted.
 *
 * INSPECT opens an existing SQLite Data Server read-only; it never creates a
 * missing database. RECORD may create the chosen database and its storage table,
 * then appends through the ordinary journal transaction. There is no memory
 * fallback, implicit retry, import, grading, remote sync or file cleanup.
 *
 * Invalid arguments/request/reader leave out_receipt unchanged and touch no
 * storage. After admission, every return publishes a captured receipt, including
 * failures. Inspect has_event only when status is OK; NOT_FOUND can mean a loaded
 * journal has no checkpoint for this exact revision/digest. ALREADY_EXISTS on
 * RECORD does not fabricate a new event. Uncertain writes expose recovery_required
 * in journal; inspect the stored state before deliberately attempting another
 * write. These plaintext receipts are private learner data, not public assets. */
UmiStatus UmiLearningProgressLocalExecute(const UmiLearningReader *reader,
    const UmiLearningProgressLocalRequest *request, UmiLearningProgressLocalReceipt *out_receipt);
#ifdef __cplusplus
}
#endif
#endif
