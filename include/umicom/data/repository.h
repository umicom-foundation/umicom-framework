/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/repository.h
 *
 * PURPOSE:
 *   Declare a namespaced repository over a provider-neutral store so product modules do not construct persistence keys directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_REPOSITORY_H
#define UMICOM_DATA_REPOSITORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/data/store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository data shared with callers of this public contract.
 */
typedef struct UmiRepository {
    UmiStore store;
    char prefix[160];
} UmiRepository;

/**
 * Initialise repository from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_repository_init(UmiRepository *repository,
                              const UmiStore *store,
                              const char *namespace_name);
/**
 * Write repository in its stable representation and report capacity or input failures to
 * the caller.
 */
UmiStatus umi_repository_save(UmiRepository *repository,
                              const char *record_id,
                              const char *serialised_value);
/**
 * Read repository into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_repository_load(const UmiRepository *repository,
                              const char *record_id,
                              char *out_value,
                              size_t value_capacity);
/**
 * Remove repository while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_repository_remove(UmiRepository *repository,
                                const char *record_id);

#ifdef __cplusplus
}
#endif

#endif
