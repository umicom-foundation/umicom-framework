/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/boot_media/media.h
 * PURPOSE: Review media transfers and keep device writes separate from file-based practice.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: include/umicom/boot_media/media.h
 * Reviewed media transfer. Inspection never executes boot code. Device writes
 * are irreversible and separate from the default, new-file practice workflow.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BOOT_MEDIA_MEDIA_H
#define UMICOM_BOOT_MEDIA_MEDIA_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BOOT_MEDIA_PATH 4096U
#define UMI_BOOT_MEDIA_ID 256U
#define UMI_BOOT_MEDIA_DEVICES 64U
#define UMI_BOOT_MEDIA_MAX_IMAGE (UINT64_C(64)*1024U*1024U*1024U)
#define UMI_BOOT_MEDIA_MAX_DEVICE (UINT64_C(256)*1024U*1024U*1024U)
typedef enum UmiBootMediaPhase {
    UMI_BOOT_MEDIA_INSPECT, UMI_BOOT_MEDIA_RECHECK, UMI_BOOT_MEDIA_WRITE,
    UMI_BOOT_MEDIA_FLUSH, UMI_BOOT_MEDIA_VERIFY, UMI_BOOT_MEDIA_FINISHED
} UmiBootMediaPhase;
typedef struct UmiBootMediaReport {
    UmiStatus status;
    UmiBootMediaPhase phase;
    uint64_t bytesDone, bytesTotal;
    int outputCreated, writeStarted, verified;
    char detail[384];
} UmiBootMediaReport;
/** Return nonzero to cancel. Called synchronously on the invoking thread; do
 * not mutate/free the plan, handles or transfer inputs inside this callback. */
typedef int (*UmiBootMediaProgress)(const UmiBootMediaReport *, void *);
typedef struct UmiBootMediaImage {
    uint64_t bytes;
    char sha256[65];
    unsigned partitions;
    int mbrLayout, gptLayout, iso9660, elTorito;
    int biosEntry, efiEntry;
} UmiBootMediaImage;
typedef struct UmiBootMediaDevice {
    char path[UMI_BOOT_MEDIA_PATH];
    char identity[UMI_BOOT_MEDIA_ID];
    char model[128], serial[128];
    uint64_t bytes;
    uint32_t sectorBytes;
    int usb, removable, protectedDevice, inUse, readOnly;
    char reason[192];
} UmiBootMediaDevice;
typedef struct UmiBootMediaInventory {
    UmiBootMediaDevice devices[UMI_BOOT_MEDIA_DEVICES];
    size_t count, omitted;
} UmiBootMediaInventory;
typedef struct UmiBootMediaPlan UmiBootMediaPlan;
/** A complete sector-addressable image, not a ZIP, gzip, kernel or directory.
 * Paths are UTF-8, host-absolute (drive-qualified on Windows, no UNC). Caller
 * inputs must be private against concurrent mutation; Linux locks cooperate.
 * Flags describe structural observations, never actual bootability or trust. */
UmiStatus UmiBootMediaInspect(const char *imagePath,UmiBootMediaImage *out,
    UmiBootMediaProgress progress,void *context,UmiBootMediaReport *report);
/** Read-only native inventory. No elevating, mounting or unmounting. Inaccessible
 * information blocks writing rather than being guessed from a drive letter. */
UmiStatus UmiBootMediaDiscover(UmiBootMediaInventory *out,UmiBootMediaReport *report);
/** New-file practice target. Parent must exist; destination must not. A failed
 * copy is retained. This never opens a block/optical device as its destination. */
UmiStatus UmiBootMediaReviewFile(const char *imagePath,const char *newFile,
    UmiBootMediaPlan **out,UmiBootMediaProgress progress,void *context,UmiBootMediaReport *report);
/** Device plan binds image contents and identity/geometry. No write occurs.
 * Only identified removable USB whole disks are candidates. */
UmiStatus UmiBootMediaReviewDevice(const char *imagePath,const UmiBootMediaDevice *device,
    UmiBootMediaPlan **out,UmiBootMediaProgress progress,void *context,UmiBootMediaReport *report);
const char *UmiBootMediaPlanFingerprint(const UmiBootMediaPlan *plan);
const char *UmiBootMediaPlanConfirmation(const UmiBootMediaPlan *plan);
const UmiBootMediaImage *UmiBootMediaPlanImage(const UmiBootMediaPlan *plan);
const char *UmiBootMediaPlanDestination(const UmiBootMediaPlan *plan);
void UmiBootMediaPlanDestroy(UmiBootMediaPlan *plan);
/** Single-use plan. Exact fingerprint and confirmation required. Device writes
 * are enabled only with UMICOM_BOOT_MEDIA_ENABLE_DEVICE_WRITES=ON. No automatic
 * elevation, retry, recovery, secure erase, formatting or partition expansion.
 * A device failure/cancellation after writeStarted can leave it unbootable. */
UmiStatus UmiBootMediaApply(UmiBootMediaPlan *plan,const char *fingerprint,
    const char *confirmation,UmiBootMediaProgress progress,void *context,UmiBootMediaReport *report);
/** Compare an entire regular-file copy with the inspected source image.
 * Both files must have equal lengths. Only regular-file handles are accepted;
 * this is not a physical-device verifier. Agreement does not prove bootability. */
UmiStatus UmiBootMediaVerifyFile(const char *imagePath,const char *copyPath,
    UmiBootMediaProgress progress,void *context,UmiBootMediaReport *report);
/** Pure policy check; does not establish that a caller-supplied observation is
 * true. The native device adapter re-observes everything before opening writes. */
UmiStatus UmiBootMediaCheckDevicePolicy(const UmiBootMediaImage *image,
    const UmiBootMediaDevice *device,UmiBootMediaReport *report);
int UmiBootMediaDeviceWritesEnabled(void);
int UmiBootMediaMain(int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
