/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/provider_document/main.c
 *
 * PURPOSE:
 *   Round-trip public provider settings through the native SDK without saving or contacting a provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/document.h"
#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/native_arguments.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
/* Retain the original no-argument entry signature for review. The dispatcher below keeps its round-trip behavior and adds shared UTF-8 file observation. The former implementation is retained for owner review. */
#if 0
int main(void)
#endif
static int RoundTrip(void)
{
    UmiProviderConnection connection={0},decoded;
    strcpy(connection.id,"comfy-example");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"ComfyUI local");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;connection.enabled=true;
    char document[UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES];size_t length=0U;uint64_t observed=0U;
    UmiStatus status=UmiProviderConnectionDocumentExport(&connection,7U,document,sizeof document,&length);
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionDocumentImport(document,length,&decoded,&observed);
    unsigned char before[UMI_SHA256_BYTES],after[UMI_SHA256_BYTES];
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionFingerprint(&connection,before);
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionFingerprint(&decoded,after);
    if(status!=UMI_STATUS_OK || observed!=7U || memcmp(before,after,sizeof before)!=0){
        fputs("Provider settings round-trip failed.\n",stderr);return 1;
    }
    puts("Settings identity preserved; no store, vault or provider was accessed.");
    return 0;
}

/* The SDK client composes public services only. Imported fields stay private;
 * a fingerprint identifies canonical settings, not a trusted provider. */
static int InspectSettings(int argc,char **argv)
{
    if(argc==1)return RoundTrip();
    if(argc==2 && strcmp(argv[1],"--help")==0){
        puts("Use no arguments for the synthetic round trip, or one absolute .umiconnection path.");
        return fflush(stdout)==0?0:1;
    }
    if(argc!=2){fputs("Expected one absolute settings document path.\n",stderr);return 2;}
    UmiProviderConnection connection={0};uint64_t revision=0U;
    UmiStatus status=UmiProviderConnectionDocumentFileRead(argv[1],NULL,&connection,&revision);
    unsigned char digest[UMI_SHA256_BYTES];char hex[UMI_SHA256_HEX_CAPACITY];
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionFingerprint(&connection,digest);
    umi_secret_clear(&connection,sizeof connection);
    if(status!=UMI_STATUS_OK){
        fprintf(stderr,"Settings observation refused (status %d); no settings activated.\n",(int)status);return 1;
    }
    UmiSha256Hex(digest,hex);
    if(printf("Observed revision: %" PRIu64 "\nCanonical settings SHA-256: %s\nNo settings activated; no provider accessed.\n",revision,hex)<0)return 1;
    return fflush(stdout)==0?0:1;
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,InspectSettings,&result);
    return status==UMI_STATUS_OK?result:1;
}
