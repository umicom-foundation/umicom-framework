/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/commit_message.h
 *
 * PURPOSE:
 *   Derive a short, reviewable commit message from the paths already staged in
 *   a repository. The service is reusable by the Umicom CLI, Studio and future
 *   source-control interfaces without giving presentation code access to Git.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_COMMIT_MESSAGE_H
#define UMICOM_REPOSITORY_COMMIT_MESSAGE_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_COMMIT_SCOPE_CAPACITY 64U
#define UMI_REPOSITORY_COMMIT_MESSAGE_CAPACITY 256U

/* Describe the broad purpose inferred from the complete staged path list. */
typedef enum UmiRepositoryCommitMessageKind {
    UMI_REPOSITORY_COMMIT_MESSAGE_FEATURE = 1,
    UMI_REPOSITORY_COMMIT_MESSAGE_DOCUMENTATION = 2,
    UMI_REPOSITORY_COMMIT_MESSAGE_TEST = 3,
    UMI_REPOSITORY_COMMIT_MESSAGE_BUILD = 4
} UmiRepositoryCommitMessageKind;

/* Return both the generated text and the evidence used to classify it so a UI
 * can preview the decision before asking the repository service to commit. */
typedef struct UmiRepositoryCommitMessage {
    UmiRepositoryCommitMessageKind kind;
    size_t changed_path_count;
    char scope[UMI_REPOSITORY_COMMIT_SCOPE_CAPACITY];
    char message[UMI_REPOSITORY_COMMIT_MESSAGE_CAPACITY];
} UmiRepositoryCommitMessage;

/* Generate a conservative message from newline-separated staged paths. The
 * caller remains responsible for showing or approving the message when its
 * workflow requires human confirmation. */
UmiStatus umi_repository_commit_message_generate(
    const char *repository_root,
    const char *staged_paths,
    UmiRepositoryCommitMessage *out_message);

#ifdef __cplusplus
}
#endif

#endif
