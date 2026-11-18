/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/hot_reload.h
 *
 * PURPOSE:
 *   Publish the public hot reload contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_HOT_RELOAD_H
#define UMICOM_FRONTEND_HOT_RELOAD_H
#include "umicom/frontend/dev_types.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the frontend changed file data shared with callers of this public contract.
 */
typedef struct UmiFrontendChangedFile { char path[UMI_FRONTEND_DEV_PATH_CAPACITY]; uint64_t revision; } UmiFrontendChangedFile;
/**
 * Represent the frontend hot reload data shared with callers of this public contract.
 */
typedef struct UmiFrontendHotReload { UmiFrontendChangedFile pending[UMI_FRONTEND_DEV_MAX_CHANGES]; size_t pending_count; uint64_t generation; uint64_t completed_generation; int enabled; } UmiFrontendHotReload;
/**
 * Initialise frontend hot reload from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_hot_reload_init(UmiFrontendHotReload *reload,int enabled);
/**
 * Provide the frontend hot reload queue operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_hot_reload_queue(UmiFrontendHotReload *reload,const char *path,uint64_t revision);
/**
 * Provide the frontend hot reload complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_hot_reload_complete(UmiFrontendHotReload *reload,uint64_t *out_generation);

/**
 * A caller-owned, process-local snapshot of work admitted to one build.
 * Keep it unchanged until acknowledgement or discard. It borrows the reload
 * identity, so it must not outlive, move with, or survive reinitialisation of
 * that reload object. Never serialise its pointer or use it as authorisation.
 */
typedef struct UmiFrontendReloadBatch {
    const UmiFrontendHotReload *owner;
    uint64_t base_generation;
    size_t count;
    UmiFrontendChangedFile changes[UMI_FRONTEND_DEV_MAX_CHANGES];
} UmiFrontendReloadBatch;

/** Report how much captured work was acknowledged and what still needs work. */
typedef struct UmiFrontendReloadReceipt {
    uint64_t generation;
    size_t acknowledged;
    size_t remaining;
} UmiFrontendReloadReceipt;

/**
 * Copy pending revisions without changing the queue. NOT_FOUND means no work.
 * On failure, batch is unchanged. Queue, batch and output storage must be
 * disjoint. All operations require one serial owner; there is no internal lock.
 */
UmiStatus umi_frontend_hot_reload_capture(const UmiFrontendHotReload *reload,
                                         UmiFrontendReloadBatch *batch);

/**
 * Acknowledge only path/revision pairs still matching a successfully applied
 * batch. Later edits and new paths remain pending. Call only after the consumer
 * has applied this exact batch; on failure discard it without acknowledging.
 * A competing completion makes this batch stale (INVALID_STATE).
 * Success advances the generation even when every path was edited again,
 * and the receipt then reports zero acknowledgements and remaining work.
 * Failures leave the queue and receipt unchanged. Revisions must distinguish
 * content changes; reusing a revision cannot detect a newer edit.
 */
UmiStatus umi_frontend_hot_reload_acknowledge(UmiFrontendHotReload *reload,
                                             const UmiFrontendReloadBatch *batch,
                                             UmiFrontendReloadReceipt *receipt);

#ifdef __cplusplus
}
#endif
#endif
