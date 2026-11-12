/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_contracts/source_contracts.h
 *
 * PURPOSE:
 *   * Expose native source, attribution and workbench qualification contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SOURCE_CONTRACTS_H
#define UMICOM_SOURCE_CONTRACTS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Borrowed diagnostic strings are valid only for the duration of the callback.
 * The library never changes source files or exits its caller's process. */
typedef void (*UmiSourceContractDiagnostic)(void *context, const char *path,
    size_t line, const char *rule, const char *message);

typedef struct UmiSourceContractReport {
    size_t files_checked;
    size_t checks;
    size_t findings;
    size_t input_errors;
    UmiSourceContractDiagnostic diagnostic;
    void *context;
} UmiSourceContractReport;

typedef struct UmiSourceAttribution {
    bool leading_comment;
    bool file;
    bool purpose;
    bool author;
    bool organisation;
    bool combined;
    bool licence;
} UmiSourceAttribution;

/* Parse only the leading block comment. A nonempty author OR organisation is
 * sufficient; field labels are case/spacing independent. Source remains owned
 * by the caller. Length excludes any optional terminal NUL. */
int UmiSourceContractReadAttribution(const char *source, size_t length,
    UmiSourceAttribution *out);

/* Run the same metadata parser for headers and C/include-fragment audits.
 * A narrow lexical guard also rejects broken quoted literals/block comments.
 * C23 numeric separators, escaped quotes, physical line splices and comments
 * are recognised. This does not compile, expand macros or validate C semantics.
 * Zero means no findings; one means a contract failure; two means input/I/O
 * failure. Missing roots and empty scans are errors, never silent passes. */
int UmiSourceContractAudit(const char *const *roots, size_t count,
    bool public_headers, UmiSourceContractReport *report);

/* Validate Framework-owned workspace source wiring and literal in-file CMake
 * preset inheritance. This is not a compiler, preprocessor or GUI runtime test.
 * Source root is the Applications composition containing framework/ and apps. */
int UmiSourceContractWorkbench(const char *source_root,
    UmiSourceContractReport *report);

/* Run one named group for diagnosis. The complete CTest entry always invokes
 * the all-groups function above. Unknown groups fail, never silently skip. */
int UmiSourceContractWorkbenchGroup(const char *source_root, const char *group,
    UmiSourceContractReport *report);

/* Probe window creation and shared identity calls using balanced C tokens.
 * Nested GTK casts are accepted; comments and string literals are not calls.
 * The Studio fragment borrows its externally created main window. */
int UmiSourceContractWindowIdentity(const char *source, const char *path,
    bool studio_fragment, UmiSourceContractReport *report);

/* Resolve a literal cache value using Framework's existing JSON reader. The
 * return is 0 for a value, 1 for absent/explicitly unset, 2 for malformed input.
 * A malformed graph is rejected even if a requested key is overridden locally.
 * Preset includes are rejected; macros are returned literally, not expanded. */
int UmiSourceContractPresetValue(const char *json, const char *preset,
    const char *key, char *out, size_t capacity);
int UmiSourceContractAuditPresets(const char *json, const char *path,
    UmiSourceContractReport *report);


/** Borrowed canonical assets. The caller owns every byte for the call duration.
 * Each asset must be nonempty and at most 2 MiB. Supply the approved resources
 * from one Framework checkout; this check does not establish their provenance. */
typedef struct UmiSourceBrandAsset {
    const unsigned char *bytes;
    size_t length;
} UmiSourceBrandAsset;
typedef struct UmiSourceBrandAssets {
    UmiSourceBrandAsset ico;
    UmiSourceBrandAsset logo_light;
    UmiSourceBrandAsset logo_dark;
    UmiSourceBrandAsset symbol;
} UmiSourceBrandAssets;
/** Check the owned standalone-guide markers, font roles and actual decoded
 * SVG/ICO bytes. Comments, inert templates and raw script/text regions do not
 * provide visible identity. The input is a bounded HTML span, not a browser
 * DOM. This checks the controlled guide format; it is not HTML sanitization,
 * a layout/contrast test, proof of installed fonts, or a signature check.
 * Return 0: no findings, 1: branding findings, 2: input/structural error.
 * Nothing is written to the source. Diagnostics omit source/asset bodies. */
int UmiSourceContractGuideBranding(const char *html, size_t length,
    const char *path, const UmiSourceBrandAssets *assets,
    UmiSourceContractReport *report);
/** Recursively audit lowercase .html files using existing bounded source
 * collection. Symlinks/reparse entries and build/.git directories are skipped;
 * missing roots and an empty selection fail. Load umicom.ico, umicom-logo.svg,
 * umicom-logo-on-dark.svg and umicom-icon.svg from the explicit asset directory.
 * Results compare local source bytes only, without rendering or modifying files. */
int UmiSourceContractAuditGuides(const char *const *roots, size_t count,
    const char *asset_directory, UmiSourceContractReport *report);

#ifdef __cplusplus
}
#endif
#endif
