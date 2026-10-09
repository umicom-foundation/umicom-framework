/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/new_project.h
 *
 * PURPOSE:
 *   Create a fresh project without merging into an existing folder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_NEW_PROJECT_H
#define UMICOM_DEVELOPER_PROJECT_NEW_PROJECT_H
#include "umicom/developer_project/service.h"
#include "umicom/build/project_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Form a new project path from an explicitly selected absolute parent and one
 * portable folder name. The shared rooted-file rules reject traversal, .git,
 * device names and ambiguous Windows characters on every host. No directory
 * is created, checked for existence or granted execution trust.
 * Failure leaves out unchanged. Inputs must be terminated and distinct from out.
 */
UmiStatus UmiDeveloperProjectChooseDestination(const char *parent,
    const char *folderName, char *out, size_t capacity);

/** Create using the canonical template service. The destination must be an
 * absolute, new path. Existing destinations are rejected, never overwritten.
 * No compiler, Git command, or network request is run. On an I/O failure new
 * partial files may remain for inspection; existing user files are not removed.
 * The caller must not concurrently modify the selected destination. */
UmiStatus UmiDeveloperProjectCreateNew(UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *outReport,
    UmiDeveloperProjectModel *outModel, UmiBuildProfile *outProfile);
/** Resolve the model's source entry relative to its explicit project root.
 * An empty entry uses the legacy src/main.c convention. Reject absolute entries
 * and lexical escapes outside the root. This only prepares a path: it does not
 * check existence or resolve filesystem links. Failure leaves output unchanged. */
UmiStatus UmiDeveloperProjectEntryPath(const UmiDeveloperProjectModel *project,
    char *out_path, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
