/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/category.h
 *
 * PURPOSE:
 *   Classify findings into memory, buffer, architecture, duplication and other
 *   security domains.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_CATEGORY_H
#define UMICOM_CODEGUARD_CATEGORY_H
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named code guard category values accepted by this public contract.
 */
typedef enum UmiCodeGuardCategory {
    UMI_CODEGUARD_CATEGORY_MEMORY = 0,
    UMI_CODEGUARD_CATEGORY_BUFFER,
    UMI_CODEGUARD_CATEGORY_STRING,
    UMI_CODEGUARD_CATEGORY_INTEGER,
    UMI_CODEGUARD_CATEGORY_RESOURCE,
    UMI_CODEGUARD_CATEGORY_CONCURRENCY,
    UMI_CODEGUARD_CATEGORY_INPUT,
    UMI_CODEGUARD_CATEGORY_COMMAND,
    UMI_CODEGUARD_CATEGORY_FILESYSTEM,
    UMI_CODEGUARD_CATEGORY_SECRET,
    UMI_CODEGUARD_CATEGORY_NETWORK,
    UMI_CODEGUARD_CATEGORY_DATABASE,
    UMI_CODEGUARD_CATEGORY_FORMAT,
    UMI_CODEGUARD_CATEGORY_ARCHITECTURE,
    UMI_CODEGUARD_CATEGORY_DUPLICATION,
    UMI_CODEGUARD_CATEGORY_PORTABILITY,
    UMI_CODEGUARD_CATEGORY_QUALITY
} UmiCodeGuardCategory;
/**
 * Provide the codeguard category text operation used by this module and its client
 * applications.
 */
const char *umi_codeguard_category_text(UmiCodeGuardCategory category);
#ifdef __cplusplus
}
#endif
#endif
