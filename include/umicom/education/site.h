/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/site.h
 *
 * PURPOSE:
 *   Declare the bounded native educational website preview and publication
 *   contracts. The website and course text live in Bits2Banking; the reusable
 *   native mechanics belong to Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_SITE_H
#define UMICOM_EDUCATION_SITE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMICOM_EDUCATION_PATH_CAPACITY 4096U

typedef enum UmicomEducationPathStatus {
    UMICOM_EDUCATION_PATH_OK = 0,
    UMICOM_EDUCATION_PATH_INVALID = 1,
    UMICOM_EDUCATION_PATH_TOO_LONG = 2
} UmicomEducationPathStatus;

/* Converts a single URL request-target into an ordinary relative file path.
 * Directory paths resolve to index.html. Rejects dot components, backslashes,
 * encoded separators, control characters and Windows alternate-stream syntax.
 * Does not perform I/O; opening a file MUST additionally enforce root identity. */
UmicomEducationPathStatus UmicomEducationResolveUrlPath(
    const char *urlTarget, char *relative, size_t capacity);

/* MIME is an explicit allow-list of common documentation assets. Unknown file
 * extensions download as application/octet-stream, never as executable HTML. */
const char *UmicomEducationMimeType(const char *relative);

/* Runs a read-only single-process site preview on IPv4 127.0.0.1.
 * Requires an existing absolute site directory. A SIGINT/SIGTERM requests
 * shutdown. once != 0 accepts one connection for deterministic smoke tests.
 * This is a local learning preview, not a public Internet/TLS host. */
int UmicomEducationServeSite(const char *siteRoot, uint16_t port, int once);

/* Copies original manuscripts, illustrations and canonical Framework artwork
 * to the MkDocs content directory. Never changes original source files.
 * Without refresh, different destination content is preserved and reported. */
int UmicomEducationPrepareSite(const char *repositoryRoot,
                               const char *brandRoot, int refresh);

/* Read-only presence check of original and published chapters, artwork,
 * the navigation configuration file and branding assets. It does not parse
 * Markdown links or check every MkDocs nav target: keep the independent
 * documentation tests until equivalent native validation is implemented. */
int UmicomEducationVerifySite(const char *repositoryRoot);

/* Invokes MkDocs with an argv array (never through a shell) after verifying an
 * interpreter actually has both MkDocs and Material installed. The default
 * checks the repository .venv first, then the selected environment / host.
 * --python selects an exact interpreter and disables automatic fallbacks.
 * Build returns nonzero when the renderer fails or site/index.html is absent.
 * The old entry point remains supported for existing callers. */
int UmicomEducationBuildSite(const char *repositoryRoot);
int UmicomEducationBuildSiteWithPython(const char *repositoryRoot,
                                       const char *requestedPython);
int UmicomEducationCheckRenderer(const char *repositoryRoot,
                                 const char *requestedPython);

/* Read only the title and ordered chapter list of the project's volume.yml
 * manifests. Source chapters under dropzone/documents are never changed.
 * `book list` inspects available volumes. `book assemble` writes a reviewable
 * Markdown compilation to output/native-books/SLUG.md, rejecting different
 * existing outputs unless refresh is explicitly requested. This is an
 * intermediate manuscript, not a PDF/DOCX or a MkDocs replacement. */
/* Validate a source volume's title, ordered list and chapter files without
 * creating, changing or publishing any output. Returns nonzero on invalid or
 * missing source inputs. It does not validate external hyperlinks or images. */
int UmicomEducationBookCheck(const char *repositoryRoot,
                             const char *volumeSlug);
int UmicomEducationBookList(const char *repositoryRoot);
int UmicomEducationBookAssemble(const char *repositoryRoot,
                                const char *volumeSlug, int refresh);

#ifdef __cplusplus
}
#endif
#endif
