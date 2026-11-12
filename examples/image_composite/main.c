/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/image_composite/main.c
 *
 * PURPOSE:
 *   Compose two owned images through the installed Framework SDK and export a branded PNG only on request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/image_composite.h"
#include "umicom/ui/gtk4/image_brand.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    bool exportImage = argc == 3 && strcmp(argv[1], "export") == 0;
    bool selfCheck = argc == 2 && strcmp(argv[1], "self-check") == 0;
    bool cancelled = argc == 2 && strcmp(argv[1], "cancel") == 0;
    bool reject = argc == 2 && strcmp(argv[1], "reject-position") == 0;
    if (!exportImage && !selfCheck && !cancelled && !reject) {
        fputs("Umicom image composition example\n"
              "Usage: image-composite self-check|cancel|reject-position\n"
              "       image-composite export ABSOLUTE_NEW_PNG\n", stderr);
        return 2;
    }
    UmiMediaImageSurface *background = NULL, *layer = NULL, *composed = NULL;
    UmiCancellationToken *cancel = NULL;
    UmiStatus status = umi_media_image_surface_create(640U, 360U, &background);
    if (status == UMI_STATUS_OK)
        status = umi_media_image_surface_clear(background, (UmiMediaRgbaPixel){0U,0U,255U,255U});
    if (status == UMI_STATUS_OK) status = umi_media_image_surface_create(160U, 90U, &layer);
    if (status == UMI_STATUS_OK)
        status = umi_media_image_surface_clear(layer, (UmiMediaRgbaPixel){255U,0U,0U,128U});
    if (status == UMI_STATUS_OK) status = umi_cancellation_token_create(&cancel);
    if (status == UMI_STATUS_OK && cancelled) umi_cancellation_token_request(cancel);
    if (status == UMI_STATUS_OK) {
        status = UmiMediaImageSurfaceComposite(background, layer, reject ? 640U : 32U,
                                               32U, cancel, &composed);
        if (cancelled || reject) {
            UmiStatus expected = cancelled ? UMI_STATUS_CANCELLED : UMI_STATUS_INVALID_ARGUMENT;
            bool refused = status == expected && composed == NULL;
            umi_media_image_surface_destroy(layer);
            umi_media_image_surface_destroy(background);
            umi_cancellation_token_destroy(cancel);
            puts(refused ? "Request refused; no result published and no file created." :
                           "Unexpected refusal result.");
            return refused ? 0 : 1;
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiMediaRgbaPixel original, blended;
        status = umi_media_image_surface_get_pixel(background, 32U, 32U, &original);
        if (status == UMI_STATUS_OK)
            status = umi_media_image_surface_get_pixel(composed, 32U, 32U, &blended);
        if (status == UMI_STATUS_OK && (original.red != 0U || original.blue != 255U ||
            blended.red != 128U || blended.green != 0U || blended.blue != 127U || blended.alpha != 255U))
            status = UMI_STATUS_INVALID_STATE;
    }
    /* Shared composition owns its result. The native export adapter adds the
     * actual embedded Umicom SVG to another copy before opening a new file.
     * The two original surfaces remain available until their owners release them. */
    if (status == UMI_STATUS_OK && exportImage) {
        UmiMediaPngWriteResult receipt;
        status = UmiGtk4ImageWriteBrandedPng(composed, argv[2], cancel, &receipt);
        printf("PNG: %s; created=%d\n", umi_status_text(status), (int)receipt.created);
        if (status == UMI_STATUS_OK)
            puts("Open the PNG and inspect the colored panel and lower-right Umicom logo.");
        else if (receipt.created)
            puts("A new partial file may remain. Inspect it and choose another name when retrying.");
    } else if (status == UMI_STATUS_OK) {
        puts("Pixel check passed: red=128, green=0, blue=127, alpha=255; original unchanged. No files created.");
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Image operation: %s\n", umi_status_text(status));
    umi_media_image_surface_destroy(composed);
    umi_media_image_surface_destroy(layer);
    umi_media_image_surface_destroy(background);
    umi_cancellation_token_destroy(cancel);
    return status == UMI_STATUS_OK ? 0 : 1;
}
