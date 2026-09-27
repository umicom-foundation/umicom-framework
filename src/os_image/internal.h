/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private native image implementation. No guest policy or mutable global state. */
#ifndef UMICOM_OS_IMAGE_INTERNAL_H
#define UMICOM_OS_IMAGE_INTERNAL_H
#include "umicom/os_image/image.h"
#include "umicom/native_launcher/sha256.h"
#include "umicom/platform/process.h"
#include "umicom/platform/boot_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#define OI_META_LIMIT (256U * 1024U)
#define OI_INPUT_LIMIT (64U * 1024U * 1024U)
#define OI_TYPE_MASK 0170000U
#define OI_REG 0100000U
#define OI_DIR 0040000U
#define OI_CHR 0020000U

typedef struct OiText { unsigned char *data; size_t size, capacity; UmiStatus status; } OiText;
typedef struct OiInput { char name[UMI_OS_IMAGE_NAME]; char hash[65]; uint64_t size; } OiInput;
typedef struct OiPlan {
    UmiOsImageArch arch;
    char buildroot[UMI_OS_IMAGE_PATH], git[UMI_OS_IMAGE_PATH];
    char commit[41], linuxVersion[32], linuxHash[65], sourceId[65];
    OiInput inputs[UMI_OS_IMAGE_MAX_INPUTS];
    size_t count;
} OiPlan;
typedef struct OiBundle {
    UmiOsImageArch arch;
    char sourceId[65], kernelHash[65], archiveHash[65], manifestHash[65];
    uint64_t kernelSize, archiveSize, manifestSize;
} OiBundle;

void OiInit(UmiOsImageReport *r);
UmiStatus OiReport(UmiOsImageReport *r, UmiStatus s, const char *message);
void OiTextInit(OiText *t);
void OiAppend(OiText *t, const void *data, size_t size);
void OiPrint(OiText *t, const char *format, ...);
void OiTextClear(OiText *t);
int OiHash(const char *s, size_t digits);
int OiNumber(const char *s, uint64_t *out);
int OiArchValid(UmiOsImageArch arch);
const char *OiArchName(UmiOsImageArch arch);
const char *OiKernelName(UmiOsImageArch arch);
UmiStatus OiAbsolute(const char *path, int buildPath);
UmiStatus OiJoin(const char *root, const char *name, char out[UMI_OS_IMAGE_PATH]);
int OiContains(const char *parent, const char *child);
uint32_t OiCrc32(const void *data, size_t size);
uint16_t OiLe16(const unsigned char *p);
uint32_t OiLe32(const unsigned char *p);
uint64_t OiLe64(const unsigned char *p);
void OiPut32(unsigned char *p, uint32_t v);

/* Native regular-file and exclusive-output adapter. Existing symlink/reparse
 * ancestors and special files are refused; no deletion operation is exposed. */
UmiStatus OiRead(const char *path, size_t limit, unsigned char **data, size_t *size);
UmiStatus OiWrite(const char *path, const void *data, size_t size);
UmiStatus OiDirectory(const char *path, int create);
UmiStatus OiParents(const char *root, const char *relative);
UmiStatus OiDigest(const char *path, char outHash[65], uint64_t *outSize);
UmiStatus OiInventory(const char *root, const char *const *names, size_t count);
UmiStatus OiLock(const char *root, void **outLock);
void OiUnlock(void *lock);
int OiNormalLinuxUser(void);
UmiStatus OiNativeProgram(const char *path);
UmiStatus OiCapture(const char *program, const char *const *args, size_t count,
    const char *cwd, unsigned timeoutMs, const UmiCancellationToken *cancel,
    OiText *capture, UmiProcessResult *result);
UmiStatus OiCaptureLimited(const char *program, const char *const *args, size_t count,
    const char *cwd, unsigned timeoutMs, const UmiCancellationToken *cancel,
    size_t maximumBytes, OiText *capture, UmiProcessResult *result);
UmiStatus OiBuildroot(const OiPlan *plan);
UmiStatus OiPlanDecode(const void *data, size_t size, OiPlan *plan);
UmiStatus OiPlanLoad(const char *root, OiPlan *plan);
UmiStatus OiReadJoined(const char *root, const char *name, size_t limit, unsigned char **data, size_t *size);
UmiStatus OiProfileLoad(const char *os, OiPlan *p);
UmiStatus OiConfigCheck(const char *root, const OiPlan *p, int kernel);
UmiStatus OiBuildReady(const char *root, const OiPlan *plan, OiText *evidence);
UmiStatus OiCapturedArtifact(const OiText *evidence, const char *relative,
    const void *data, size_t size);
UmiStatus OiBundleLoad(const char *root, OiBundle *bundle);
UmiStatus OiRootfsCheck(UmiOsImageArchive *archive, const OiBundle *b, const OiPlan *p);
UmiStatus OiPackFiles(const char *target, const char *overlay, const char *kernel,
    const unsigned char *manifest, size_t manifestSize, const OiPlan *p, const OiText *evidence,
    const char *destination, UmiOsImageReport *r);
#endif
