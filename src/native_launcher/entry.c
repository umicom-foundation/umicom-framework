/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One canonical desktop-entry writer is used for both creation and verification.
 * Verification therefore checks meaning as well as an untrusted stored digest. */
#include "internal.h"
UmiStatus UmiNativeStageRenderEntry(const UmiNativeStageManifest *m,
    char *outText, size_t capacity, size_t *outLength)
{
    UmiNativeText out;
    UmiNativeTextInit(&out,outText,capacity);
    if (outLength != NULL) *outLength=0U;
    if (m==NULL || UmiNativeStageValidatePath(m->root,0)!=UMI_STATUS_OK
        || UmiNativeStageValidatePath(m->desk.path,0)!=UMI_STATUS_OK
        || UmiNativeStageValidatePath(m->launcher.path,1)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    UmiNativeTextString(&out,"[Desktop Entry]\nType=Application\nVersion=1.0\nName=Umicom Desk session\n");
    UmiNativeTextString(&out,"Comment=Open Umicom Desk in the current graphical session\nExec=");
    UmiNativeTextEntryArgument(&out,m->launcher.path);
    UmiNativeTextString(&out," --run --desk "); UmiNativeTextEntryArgument(&out,m->desk.path);
    UmiNativeTextString(&out,"\nTryExec="); UmiNativeTextEntryValue(&out,m->launcher.path);
    UmiNativeTextString(&out,"\nIcon="); UmiNativeTextEntryValue(&out,m->root);
    UmiNativeTextString(&out,"/" UMI_NATIVE_STAGE_ICON_NAME "\nTerminal=false\nDBusActivatable=false\nCategories=System;\nStartupNotify=false\n");
    return UmiNativeTextFinish(&out,outLength);
}
