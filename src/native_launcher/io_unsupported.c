/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Honest host boundary: Linux launcher staging is not Windows installation.
 * Portable codecs and SHA-256 remain available on every supported C host. */
#include "internal.h"
static UmiStatus Unsupported(UmiNativeStageReport *report)
{
    UmiNativeReportInit(report);
    return UmiNativeReportSet(report,UMI_STATUS_UNAVAILABLE,"host","Linux session staging requires Linux and Linux executables. No fallback script was started.",0);
}
UmiStatus UmiNativeStagePlanCreate(const UmiNativeStageConfig *config,UmiNativeStagePlan **outPlan,UmiNativeStageReport *report)
{
    (void)config;if(outPlan!=NULL)*outPlan=NULL;return Unsupported(report);
}
UmiStatus UmiNativeStageCommit(UmiNativeStagePlan *plan,const char *expectedFingerprint,UmiNativeStageReport *report)
{
    (void)plan;(void)expectedFingerprint;return Unsupported(report);
}
UmiStatus UmiNativeStageVerify(const char *outputPath,UmiNativeStageReport *report)
{
    (void)outputPath;return Unsupported(report);
}
#ifndef _WIN32
UmiStatus UmiNativeStageDigestFile(const char *path,char outHex[65],uint64_t *outBytes,UmiNativeStageReport *report)
{
    (void)path;if(outHex!=NULL)outHex[0]='\0';if(outBytes!=NULL)*outBytes=0U;return Unsupported(report);
}
#endif
