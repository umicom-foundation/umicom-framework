/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Public facade for the bootstrap's canonical checked file operations. Shared
 * tools reuse these rather than copy platform-specific path and hash logic. */
#ifndef UMICOM_SETUP_CENTRE_FILES_H
#define UMICOM_SETUP_CENTRE_FILES_H
#include "umicom/setup_centre/setup.h"
#ifdef __cplusplus
extern "C" {
#endif
    UmiStatus UmiSetupFileRead(const char *path, size_t limit, char **outBytes,     size_t *outLength, UmiSetupReport *report);
    /* free() returned allocation. */
    UmiStatus UmiSetupFileWriteNew(const char *path, const void *data, size_t length,     UmiSetupReport *report);
    UmiStatus UmiSetupFileDigest(const char *path, char outHash[65], uint64_t *outBytes,     UmiSetupReport *report);
    UmiStatus UmiSetupFileCopyChecked(const char *source, const char *destination,     const char *hash, uint64_t bytes, UmiSetupReport *report);
    UmiStatus UmiSetupDirectoryCreate(const char *path, UmiSetupReport *report);
    UmiStatus UmiSetupFileCheck(const char *path, int directory, UmiSetupReport *report);
    UmiStatus UmiSetupFileParents(const char *root, const char *relative, UmiSetupReport *report);
    UmiStatus UmiSetupPathJoin(const char *root, const char *relative,     char outPath[UMI_SETUP_PATH_CAPACITY]);
    /** Grant owner execute only on a copied, regular, owner-held file. */
    UmiStatus UmiSetupFileGrantOwnerExecute(const char *path,UmiSetupReport *report);
    UmiStatus UmiSetupFileSingleLink(const char *path,UmiSetupReport *report);
    UmiStatus UmiSetupNativeProgramCheck(const char *path,UmiSetupReport *report);
    UmiStatus UmiSetupFileReadHeader(const char *path,unsigned char *data,size_t capacity,     size_t *outLength,UmiSetupReport *report);
#ifdef __cplusplus
}
#endif
#endif
