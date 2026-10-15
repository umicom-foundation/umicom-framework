/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/scaffold/application_module.h
 * PURPOSE: Preview and create standalone application modules through shared native services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_SCAFFOLD_APPLICATION_MODULE_H
#define UMICOM_SCAFFOLD_APPLICATION_MODULE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/runtime/application_module.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SCAFFOLD_FILE_LIMIT 16U
#define UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT 32768U

/** Inputs are borrowed only during planning. Paths must be absolute, normalised
 * ordinary local paths. The destination parent must already exist and be owned
 * by the user. Names use a deliberately small portable ASCII subset so a name
 * cannot introduce C, CMake, YAML or Markdown syntax into generated files.
 * Both console-only and console plus GTK4 projects are supported. */
/**
 * Find module templates relative to the running executable, with the configured
 * source-tree location as a fallback for developer builds. No current working
 * directory or user profile is consulted. This reads directory metadata only.
 * Output must have nonzero capacity and is changed only on success. The caller
 * may instead supply an explicit template_root to the planning service.
 */
UmiStatus umi_application_scaffold_template_root(char *output, size_t capacity);

typedef struct UmiApplicationScaffoldRequest {
    const char *template_root;
    const char *destination_parent;
    const char *slug;
    const char *application_name;
    const char *application_id;
    bool gtk_frontend;
} UmiApplicationScaffoldRequest;

/** A plan owns all rendered bytes. Template changes after planning cannot change
 * what apply writes. Keep access and apply on one owner thread; destruction
 * invalidates all borrowed file views. No compiler or Git process is started. */
typedef struct UmiApplicationScaffoldPlan UmiApplicationScaffoldPlan;

/** Copied metadata for a preview screen; the descriptor is the same contract
 * used by native manifest inspection and configure-time discovery. */
typedef struct UmiApplicationScaffoldSummary {
    char root[UMI_PATH_CAPACITY];
    UmiApplicationModuleDescriptor module;
    size_t file_count;
    size_t byte_count;
    bool apply_started;
} UmiApplicationScaffoldSummary;

/** A borrowed, NUL-terminated file view remains valid until plan destruction.
 * length excludes the NUL. Paths are relative to the planned application root. */
typedef struct UmiApplicationScaffoldFile {
    const char *relative_path;
    const char *text;
    size_t length;
} UmiApplicationScaffoldFile;

/** Report partial work honestly. completed_files counts fully written and closed
 * files; created_files also includes a file left by a failed write or close.
 * Failed creation keeps all files for inspection, without automatic deletion.
 * last_path identifies the operation that last ran, including on failure. */
typedef struct UmiApplicationScaffoldResult {
    UmiStatus status;
    size_t created_directories;
    size_t created_files;
    size_t completed_files;
    char last_path[UMI_PATH_CAPACITY];
} UmiApplicationScaffoldResult;

/** Read a fixed, bounded template catalogue and render a complete source preview.
 * out_plan is cleared on failure. No destination files are created. Existing
 * destinations are rejected during planning and checked exclusively at apply.
 * Parent aliases are trusted; this is not a hostile-filesystem sandbox. */
UmiStatus umi_application_scaffold_plan(
    const UmiApplicationScaffoldRequest *request,
    UmiApplicationScaffoldPlan **out_plan);

/** Copy plan metadata into caller storage; clear output on invalid arguments. */
UmiStatus umi_application_scaffold_summary(
    const UmiApplicationScaffoldPlan *plan,
    UmiApplicationScaffoldSummary *out_summary);

/** Borrow the rendered file at index; clear output on an invalid index. */
UmiStatus umi_application_scaffold_file(
    const UmiApplicationScaffoldPlan *plan, size_t index,
    UmiApplicationScaffoldFile *out_file);

/** Create the reviewed source in an exclusively created application directory.
 * Every file uses exclusive creation; existing entries are never overwritten.
 * An apply attempt consumes the plan, even on failure. Inspect retained partial
 * work and choose another destination for a new plan. No Git index, submodule
 * registration, commit, remote, build or launch is performed. Run on a worker;
 * keep the chosen parent and generated directories free of concurrent writers.
 * Success does not promise power-loss atomicity or whole-project durability. */
UmiStatus umi_application_scaffold_apply(
    UmiApplicationScaffoldPlan *plan, UmiApplicationScaffoldResult *out_result);

/** Release plan-owned bytes. A NULL plan is accepted. */
void umi_application_scaffold_destroy(UmiApplicationScaffoldPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
