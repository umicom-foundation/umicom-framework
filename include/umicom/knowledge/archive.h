/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/archive.h
 * PURPOSE: Persist and restore a versioned offline local knowledge snapshot.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Archives make an indexed corpus available again without a network. */
#ifndef UMICOM_KNOWLEDGE_ARCHIVE_H
#define UMICOM_KNOWLEDGE_ARCHIVE_H

#include "umicom/knowledge/service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_KNOWLEDGE_ARCHIVE_VERSION 1U

/**
 * Write knowledge archive in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_knowledge_archive_save(const UmiKnowledgeService *service,
                                     const char *path);
/**
 * Read knowledge archive into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_knowledge_archive_load(UmiKnowledgeService *service,
                                     const char *path);

#ifdef __cplusplus
}
#endif
#endif
