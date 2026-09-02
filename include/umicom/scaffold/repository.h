/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/scaffold/repository.h
 *
 * PURPOSE:
 *   Create a complete independent Umicom application repository with selected
 *   console, GTK4, web, Qt, Wt, or mobile frontends, a Framework submodule,
 *   tests, manifests, build presets, documentation, and optional GitHub remote.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SCAFFOLD_REPOSITORY_H
#define UMICOM_SCAFFOLD_REPOSITORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/repository/repository.h"
#include "umicom/runtime/application_manifest.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named framework link mode values accepted by this public contract.
 */
typedef enum UmiFrameworkLinkMode {
    UMI_FRAMEWORK_LINK_SUBMODULE = 1,
    UMI_FRAMEWORK_LINK_INSTALLED = 2,
    UMI_FRAMEWORK_LINK_VENDORED = 3
} UmiFrameworkLinkMode;

/**
 * Represent the repository scaffold request data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryScaffoldRequest {
    const char *template_root;
    const char *destination_parent;
    const char *application_name;
    const char *repository_name;
    const char *application_id;
    const char *remote_owner;
    const char *remote_description;
    const char *framework_url;
    const char *framework_branch;
    unsigned frontends;
    UmiFrameworkLinkMode framework_link;
    UmiRepositoryVisibility visibility;
    int initialise_git;
    int create_initial_commit;
    int create_remote;
    int push_initial;
    int dry_run;
} UmiRepositoryScaffoldRequest;

/**
 * Represent the repository scaffold report data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryScaffoldReport {
    char repository_root[UMI_REPOSITORY_PATH_CAPACITY];
    char repository_name[UMI_REPOSITORY_TEXT_CAPACITY];
    char application_name[UMI_REPOSITORY_TEXT_CAPACITY];
    char application_id[UMI_REPOSITORY_TEXT_CAPACITY];
    char application_slug[UMI_REPOSITORY_TEXT_CAPACITY];
    size_t directories_created;
    size_t files_created;
    int framework_linked;
    UmiRepositoryReport repository_report;
} UmiRepositoryScaffoldReport;

/**
 * Initialise repository scaffold from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_repository_scaffold_create(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryScaffoldRequest *request,
    UmiRepositoryScaffoldReport *out_report
);
/**
 * Provide the repository scaffold slug operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_scaffold_slug(const char *name,
                                       char *out_slug,
                                       size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
