/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/seedream_response.c
 * PURPOSE: Validate a complete provider reply before publishing an owned image.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "seedream_internal.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdlib.h>
#include <string.h>

static int Base64Digit(unsigned char character)
{
    if (character >= 'A' && character <= 'Z')
        return character - 'A';
    if (character >= 'a' && character <= 'z')
        return character - 'a' + 26;
    if (character >= '0' && character <= '9')
        return character - '0' + 52;
    return character == '+' ? 62 : character == '/' ? 63 : -1;
}
/* Base64 is a transport encoding, not an image validator. Require canonical
 * padding and pad bits, bound decoded bytes, then pass the PNG to its decoder. */
static UmiStatus DecodePngBase64(const char *encoded, const UmiCancellationToken *cancel,
                                 UmiMediaImageSurface **out)
{
    size_t length = strlen(encoded);
    if (!length || length % 4U)
        return UMI_STATUS_PARSE_ERROR;
    size_t padding = (size_t)(encoded[length - 1] == '=') + (size_t)(encoded[length - 2] == '=');
    size_t bytes = length / 4U * 3U - padding;
    if (!bytes || bytes > UMI_SEEDREAM_IMAGE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *decoded = malloc(bytes);
    if (!decoded)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    size_t used = 0;
    for (size_t offset = 0; offset < length; offset += 4U)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        int first = Base64Digit((unsigned char)encoded[offset]),
            second = Base64Digit((unsigned char)encoded[offset + 1]);
        int third = Base64Digit((unsigned char)encoded[offset + 2]),
            fourth = Base64Digit((unsigned char)encoded[offset + 3]);
        bool tail = offset + 4U == length;
        if (first < 0 || second < 0 ||
            (third < 0 && !(tail && encoded[offset + 2] == '=' && encoded[offset + 3] == '=')) ||
            (fourth < 0 && !(tail && encoded[offset + 3] == '=')) ||
            (tail && padding == 2U && (second & 15)) || (tail && padding == 1U && (third & 3)))
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        unsigned word = ((unsigned)first << 18U) | ((unsigned)second << 12U) |
                        ((unsigned)(third < 0 ? 0 : third) << 6U) | (unsigned)(fourth < 0 ? 0 : fourth);
        decoded[used++] = (unsigned char)(word >> 16U);
        if (used < bytes)
            decoded[used++] = (unsigned char)(word >> 8U);
        if (used < bytes)
            decoded[used++] = (unsigned char)word;
    }
    if (status == UMI_STATUS_OK)
        status = UmiMediaPngDecode(decoded, bytes, cancel, out);
    umi_secret_clear(decoded, bytes);
    free(decoded);
    return status;
}
static UmiStatus ReadJsonText(const UmiJsonTree *tree, int object, const char *name, char *out,
                              size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}
static bool NoProviderError(const UmiJsonTree *tree, int object)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, "error", &node);
    return status == UMI_STATUS_NOT_FOUND || (status == UMI_STATUS_OK && UmiJsonTreeIsNull(tree, node));
}
/* Decode into temporary ownership. A malformed later field must not leave a
 * partially accepted image in a caller's previously empty result. */
UmiStatus UmiSeedreamDecode(const char *bytes, size_t length, const UmiCancellationToken *cancel,
                            UmiSeedreamResult *out)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_SEEDREAM_REPLY_LIMIT, 256U, 8U};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, &tree);
    int data_node = -1, image_node = -1, node = -1;
    char model[129] = {0}, format[16] = {0};
    char *encoded = NULL;
    const char *span = NULL;
    size_t span_length = 0;
    UmiMediaImageSurface *surface = NULL;
    if (status != UMI_STATUS_OK)
        goto done;
    if (!NoProviderError(tree, 0))
    {
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    status = ReadJsonText(tree, 0, "model", model, sizeof(model));
    if (status != UMI_STATUS_OK)
        goto done;
    if (!model[0])
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    status = UmiJsonTreeMember(tree, 0, "data", &data_node);
    if (status != UMI_STATUS_OK)
        goto done;
    if (UmiJsonTreeKind(tree, data_node) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
        UmiJsonTreeCount(tree, data_node) != 1U)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    image_node = UmiJsonTreeFirst(tree, data_node);
    if (!NoProviderError(tree, image_node))
    {
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    /* Some model revisions omit this optional field. If present, its exact
     * value must agree with the PNG bytes requested by this adapter. */
    status = ReadJsonText(tree, image_node, "output_format", format, sizeof(format));
    if (status == UMI_STATUS_OK && strcmp(format, "png"))
        status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
        goto done;
    status = UmiJsonTreeMember(tree, image_node, "b64_json", &node);
    if (status != UMI_STATUS_OK)
        goto done;
    status = UmiJsonTreeSourceSpan(tree, node, &span, &span_length);
    if (status != UMI_STATUS_OK)
        goto done;
    (void)span;
    encoded = malloc(span_length + 1U);
    if (!encoded)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    status = UmiJsonTreeText(tree, node, encoded, span_length + 1U);
    if (status == UMI_STATUS_OK)
        status = DecodePngBase64(encoded, cancel, &surface);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        memcpy(out->model, model, sizeof(model));
        out->image_node = surface;
        surface = NULL;
    }
done:
    if (encoded)
    {
        umi_secret_clear(encoded, span_length + 1U);
        free(encoded);
    }
    umi_media_image_surface_destroy(surface);
    UmiJsonTreeDestroy(tree);
    return status;
}
