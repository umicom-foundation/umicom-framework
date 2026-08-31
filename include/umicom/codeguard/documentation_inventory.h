/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/documentation_inventory.h
 *
 * PURPOSE:
 *   Aggregate documentation coverage from any number of source files without
 *   retaining per-file paths or allocating unbounded workspace memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_DOCUMENTATION_INVENTORY_H
#define UMICOM_CODEGUARD_DOCUMENTATION_INVENTORY_H

#include "umicom/codeguard/documentation_coverage.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This version identifies the meaning of all counters in the public structure. */
#define UMI_CODEGUARD_DOCUMENTATION_INVENTORY_API_VERSION 1U

/* The inventory owns only totals. Callers may therefore stream reports from a
 * large repository and discard every individual report after recording it. */
typedef struct UmiCodeGuardDocumentationInventory {
    uint32_t structure_size;            /* Size protects compatible ABI growth. */
    uint32_t api_version;               /* Counter semantics used by this object. */
    size_t file_count;                  /* Valid reports recorded. */
    size_t passing_file_count;          /* Reports that passed their scan policy. */
    size_t standard_header_count;       /* Files with purpose, author and licence. */
    size_t line_count;                  /* Physical source-line total. */
    size_t function_count;              /* Function candidates found. */
    size_t documented_function_count;   /* Function candidates with guidance. */
    size_t decision_count;              /* Conditions, loops and switches found. */
    size_t explained_decision_count;    /* Decision candidates with guidance. */
} UmiCodeGuardDocumentationInventory;

/* Initialise empty caller-owned inventory storage. Passing NULL is harmless. */
void umi_codeguard_documentation_inventory_init(
    UmiCodeGuardDocumentationInventory *inventory);

/* Validate structure identity and all subset relationships without changing it. */
UmiStatus umi_codeguard_documentation_inventory_validate(
    const UmiCodeGuardDocumentationInventory *inventory);

/* Record one validated report. The report is copied into totals and not retained. */
UmiStatus umi_codeguard_documentation_inventory_record(
    UmiCodeGuardDocumentationInventory *inventory,
    const UmiCodeGuardDocumentationReport *report);

/* Merge another validated inventory, enabling parallel directory scans. */
UmiStatus umi_codeguard_documentation_inventory_merge(
    UmiCodeGuardDocumentationInventory *inventory,
    const UmiCodeGuardDocumentationInventory *source);

/* Return the percentage of files that passed their individual scan policy. */
unsigned umi_codeguard_documentation_inventory_file_percent(
    const UmiCodeGuardDocumentationInventory *inventory);

/* Return aggregate explained-function coverage across all recorded files. */
unsigned umi_codeguard_documentation_inventory_function_percent(
    const UmiCodeGuardDocumentationInventory *inventory);

/* Return aggregate explained-decision coverage across all recorded files. */
unsigned umi_codeguard_documentation_inventory_decision_percent(
    const UmiCodeGuardDocumentationInventory *inventory);

/* Return true only when at least one file was recorded and every file passed. */
bool umi_codeguard_documentation_inventory_passes(
    const UmiCodeGuardDocumentationInventory *inventory);

#ifdef __cplusplus
}
#endif

#endif
