/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/provider.h
 *
 * PURPOSE:
 *   Define the provider-neutral source-control operations used by Framework products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_PROVIDER_H
#define UMICOM_VCS_PROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/vcs/branch.h"
#include "umicom/vcs/change.h"
#include "umicom/vcs/history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsProvider {
    uint32_t structure_size;
    const char *provider_id;
    void *instance;
    UmiStatus (*status)(void *instance,
                        const char *root,
                        UmiVcsChangeList *out_changes,
                        UmiVcsBranch *out_branch);
    UmiStatus (*history)(void *instance,
                         const char *root,
                         size_t limit,
                         UmiVcsHistory *out_history);
    UmiStatus (*stage)(void *instance,
                       const char *root,
                       const char *path);
    UmiStatus (*unstage)(void *instance,
                         const char *root,
                         const char *path);
    UmiStatus (*commit)(void *instance,
                        const char *root,
                        const char *message,
                        char *out_commit_id,
                        size_t capacity);
    UmiStatus (*pull)(void *instance,
                      const char *root);
    UmiStatus (*push)(void *instance,
                      const char *root);
    void (*destroy)(void *instance);
} UmiVcsProvider;

UmiStatus umi_vcs_provider_validate(const UmiVcsProvider *provider);

#ifdef __cplusplus
}
#endif

#endif
