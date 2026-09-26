/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Private native launcher ownership and checked text-building contracts. */
#ifndef UMICOM_NATIVE_LAUNCHER_INTERNAL_H
#define UMICOM_NATIVE_LAUNCHER_INTERNAL_H
#include "umicom/native_launcher/stage.h"
#include "umicom/native_launcher/sha256.h"
#include <stdbool.h>
typedef struct UmiNativeText {
    char *data;
    size_t capacity;
    size_t length;
    UmiStatus status;
} UmiNativeText;
struct UmiNativeStagePlan {
    UmiNativeStageManifest manifest;
    unsigned char *icon;
    size_t iconLength;
    char *entry;
    size_t entryLength;
    char fingerprint[65];
    int consumed;
};
void UmiNativeTextInit(UmiNativeText *text, char *buffer, size_t capacity);
void UmiNativeTextBytes(UmiNativeText *text, const void *bytes, size_t length);
void UmiNativeTextString(UmiNativeText *text, const char *value);
void UmiNativeTextDecimal(UmiNativeText *text, uint64_t value);
void UmiNativeTextJson(UmiNativeText *text, const char *value);
void UmiNativeTextEntryValue(UmiNativeText *text, const char *value);
void UmiNativeTextEntryArgument(UmiNativeText *text, const char *value);
UmiStatus UmiNativeTextFinish(UmiNativeText *text, size_t *outLength);
int UmiNativeUtf8(const unsigned char *bytes, size_t length);
int UmiNativeHashValid(const char *hash);
UmiStatus UmiNativeManifestValidate(const UmiNativeStageManifest *manifest);
void UmiNativeReportInit(UmiNativeStageReport *report);
UmiStatus UmiNativeReportSet(UmiNativeStageReport *report, UmiStatus status,
    const char *phase, const char *detail, int systemError);
UmiStatus UmiNativePlanFingerprint(UmiNativeStagePlan *plan);
const unsigned char *UmiNativeBrandIcon(size_t *outLength);
#endif
