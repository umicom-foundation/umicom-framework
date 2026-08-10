/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository.h
 *
 * PURPOSE:
 *   Provide native local Git and optional GitHub repository operations used by
 *   the Umicom command, Studio, generated applications, suite tooling, and
 *   future delivery services without embedding shell scripts in each product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_REPOSITORY_H
#define UMICOM_REPOSITORY_REPOSITORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_TEXT_CAPACITY 256U
#define UMI_REPOSITORY_PATH_CAPACITY 2048U

typedef enum UmiRepositoryVisibility {
    UMI_REPOSITORY_PRIVATE = 0,
    UMI_REPOSITORY_PUBLIC = 1
} UmiRepositoryVisibility;

typedef struct UmiRepositoryRequest {
    const char *repository_root;
    const char *remote_owner;
    const char *remote_name;
    const char *remote_description;
    UmiRepositoryVisibility visibility;
    int initialise_local;
    int create_initial_commit;
    int create_remote;
    int push_initial;
    int dry_run;
} UmiRepositoryRequest;

typedef struct UmiRepositoryReport {
    char repository_root[UMI_REPOSITORY_PATH_CAPACITY];
    char remote_name[UMI_REPOSITORY_TEXT_CAPACITY];
    char remote_url[UMI_REPOSITORY_PATH_CAPACITY];
    int local_initialised;
    int initial_commit_created;
    int remote_created;
    int pushed;
    int last_exit_code;
    char last_output[UMI_PROCESS_OUTPUT_CAPACITY];
} UmiRepositoryReport;

UmiStatus umi_repository_initialise(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryRequest *request,
    UmiRepositoryReport *out_report
);
UmiStatus umi_repository_create_remote(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryRequest *request,
    UmiRepositoryReport *out_report
);
UmiStatus umi_repository_add_submodule(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    const char *url,
    const char *destination,
    const char *branch,
    int dry_run
);
UmiStatus umi_repository_status(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    char *out_text,
    size_t capacity
);

#ifdef __cplusplus
}
#endif

#endif
