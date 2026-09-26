/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private bootstrap implementation. No GTK, interpreter or shell dependency. */

#ifndef UMICOM_SETUP_CENTRE_INTERNAL_H
#define UMICOM_SETUP_CENTRE_INTERNAL_H
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

void ScReport(UmiSetupReport *report,UmiStatus status,const char *format,...);

void ScTextInit(ScText *text);

void ScTextFree(ScText *text);

void ScPrint(ScText *text,const char *format,...);

int ScToken(const char *text);

int ScUtf8(const char *text);

int ScHashValid(const char *hash);

int ScNumber(const char *text,uint64_t *out);

UmiStatus ScJoin(const char *root,const char *relative,char out[UMI_SETUP_PATH_CAPACITY]);

int ScEqualFold(const char *a,const char *b);

UmiStatus ScRead(const char *path,size_t limit,char **data,size_t *length,UmiSetupReport *report);

UmiStatus ScWriteNew(const char *path,const void *data,size_t length,UmiSetupReport *report);

UmiStatus ScMakeDirectory(const char *path,UmiSetupReport *report);

UmiStatus ScParents(const char *root,const char *relative,UmiSetupReport *report);

UmiStatus ScCopy(const char *source,const char *destination,const char *expected,
uint64_t expectedSize,UmiSetupProgress progress,void *context,UmiSetupReport *report);

UmiStatus ScDigest(const char *path,char hash[65],uint64_t *bytes,UmiSetupProgress progress,
void *context,UmiSetupReport *report);

UmiStatus ScReadAt(const char *path,uint64_t offset,void *data,size_t length,uint64_t *fileSize,UmiSetupReport *report);

UmiStatus ScHeaderRead(const char *path,unsigned char *bytes,size_t capacity,size_t *length,UmiSetupReport *report);

UmiStatus ScFileRegular(const char *path,int directory,UmiSetupReport *report);

UmiStatus ScFreeSpace(const char *existingParent,uint64_t required,UmiSetupReport *report);

UmiStatus ScAddFile(UmiSetupBundle *bundle,const ScFile *file);

UmiStatus ScEncode(const UmiSetupBundle *bundle,uint64_t selected,int installed,ScText *out);

UmiStatus ScDecode(const char *data,size_t length,UmiSetupBundle *bundle,int installed);

UmiStatus ScImportReports(const char *list,UmiSetupBundle *bundle,UmiSetupReport *report);

int ScSelected(const ScFile *file,uint64_t selected);

UmiStatus ScHostProgram(const char *path,UmiSetupReport *report);

UmiStatus ScKernel(const char *path,const char *arch,UmiSetupReport *report);

#endif
