/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/setup_centre/internal.h
 *
 * PURPOSE:
 *   Keep bootstrap implementation private without a GTK, interpreter or shell dependency.
 *
 * EDUCATIONAL OVERVIEW:
 *   Private helpers have cross-file linkage but are not public SDK functions.
 *   Component-qualified linker names prevent collisions in umbrella builds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SETUP_CENTRE_INTERNAL_H
#define UMICOM_SETUP_CENTRE_INTERNAL_H
/* Cross-file helpers keep source spellings while their emitted symbols are
 * private to Setup Centre, so unrelated static libraries can be linked safely. */
#include "symbol_names.h"
#include "umicom/setup_centre/setup.h"
#include "umicom/setup_centre/virtual_machine.h"
#include "umicom/native_launcher/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define SC_SHARED 64U
#define SC_HEADER "UMICOM_SUITE\t1\n"
#define SC_RECEIPT_HEADER "UMICOM_INSTALL\t1\n"
typedef struct ScFile {

    char relative[UMI_SETUP_RELATIVE_CAPACITY];

    char source[UMI_SETUP_PATH_CAPACITY];

    char hash[65];

    uint64_t bytes;

    unsigned owner;

}
ScFile;

struct UmiSetupBundle {

    char root[UMI_SETUP_PATH_CAPACITY];

    char catalogueHash[65];

    UmiSetupApplication apps[UMI_SETUP_MAX_APPS];

    size_t appCount;

    ScFile *files;

    size_t fileCount;

    size_t capacity;

    int installed;

};

typedef struct ScText {
    char *data;
    size_t size;
    size_t capacity;
    UmiStatus status;
}
ScText;

/* Format the existing structured diagnostic without exposing it as a public SDK helper. */
void ScReport(UmiSetupReport *report,UmiStatus status,const char *format,...);

/* Initialise the owned text builder before appending encoded bundle data. */
void ScTextInit(ScText *text);

/* Release the text builder through the same component that allocated it. */
void ScTextFree(ScText *text);

/* Append formatted text using the component's bounded builder contract. */
void ScPrint(ScText *text,const char *format,...);

/* Apply the existing token grammar before a field enters a bundle record. */
int ScToken(const char *text);

/* Check text encoding before accepting names in persistent metadata. */
int ScUtf8(const char *text);

/* Validate the digest spelling separately from computing or comparing file bytes. */
int ScHashValid(const char *hash);

/* Parse the numeric field used by bundle records without changing its caller contract. */
int ScNumber(const char *text,uint64_t *out);

/* Join paths through the established Setup Centre relative-path policy. */
UmiStatus ScJoin(const char *root,const char *relative,char out[UMI_SETUP_PATH_CAPACITY]);

/* Compare names using the component's existing case-folding rule. */
int ScEqualFold(const char *a,const char *b);

/* Read a bounded file and report its resulting buffer through existing ownership rules. */
UmiStatus ScRead(const char *path,size_t limit,char **data,size_t *length,UmiSetupReport *report);

/* Create a new payload through the existing no-overwrite writer. */
UmiStatus ScWriteNew(const char *path,const void *data,size_t length,UmiSetupReport *report);

/* Validate the destination parent before a write or directory operation. */
UmiStatus ScParentCheck(const char *path,UmiSetupReport *report);

/* Create a checked directory using the existing diagnostic contract. */
UmiStatus ScMakeDirectory(const char *path,UmiSetupReport *report);

/* Prepare bounded relative parent directories under an approved root. */
UmiStatus ScParents(const char *root,const char *relative,UmiSetupReport *report);

/* Copy a declared payload with the existing size, digest and progress checks. */
UmiStatus ScCopy(const char *source,const char *destination,const char *expected,
uint64_t expectedSize,UmiSetupProgress progress,void *context,UmiSetupReport *report);

/* Compute the existing payload digest while preserving progress reporting. */
UmiStatus ScDigest(const char *path,char hash[65],uint64_t *bytes,UmiSetupProgress progress,
void *context,UmiSetupReport *report);

/* Read one bounded file span for header or payload validation. */
UmiStatus ScReadAt(const char *path,uint64_t offset,void *data,size_t length,uint64_t *fileSize,UmiSetupReport *report);

/* Read only the supported initial header bytes for executable inspection. */
UmiStatus ScHeaderRead(const char *path,unsigned char *bytes,size_t capacity,size_t *length,UmiSetupReport *report);

/* Enforce the established file-versus-directory path expectation. */
UmiStatus ScFileRegular(const char *path,int directory,UmiSetupReport *report);

/* Check free space before beginning the corresponding installation operation. */
UmiStatus ScFreeSpace(const char *existingParent,uint64_t required,UmiSetupReport *report);

/* Append a declared file to the bundle using its bounded ownership rules. */
UmiStatus ScAddFile(UmiSetupBundle *bundle,const ScFile *file);

/* Encode bundle metadata into the component-owned text builder. */
UmiStatus ScEncode(const UmiSetupBundle *bundle,uint64_t selected,int installed,ScText *out);

/* Decode validated metadata through the existing bundle parser. */
UmiStatus ScDecode(const char *data,size_t length,UmiSetupBundle *bundle,int installed);

/* Import the declared installation reports without changing the report format. */
UmiStatus ScImportReports(const char *list,UmiSetupBundle *bundle,UmiSetupReport *report);

/* Evaluate bundle ownership selection independently of copying payload bytes. */
int ScSelected(const ScFile *file,uint64_t selected);

/* Apply existing host-executable admission rules to the selected file. */
UmiStatus ScHostProgram(const char *path,UmiSetupReport *report);

/* Apply the existing architecture-specific kernel admission checks. */
UmiStatus ScKernel(const char *path,const char *arch,UmiSetupReport *report);

#endif
