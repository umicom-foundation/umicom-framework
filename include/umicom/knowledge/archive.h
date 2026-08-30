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
/* BEGINNER NOTE: Archives make an indexed corpus available again without a network. */
#ifndef UMICOM_KNOWLEDGE_ARCHIVE_H
#define UMICOM_KNOWLEDGE_ARCHIVE_H

#include "umicom/knowledge/service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_KNOWLEDGE_ARCHIVE_VERSION 1U

UmiStatus umi_knowledge_archive_save(const UmiKnowledgeService *service,
                                     const char *path);
UmiStatus umi_knowledge_archive_load(UmiKnowledgeService *service,
                                     const char *path);

#ifdef __cplusplus
}
#endif
#endif
