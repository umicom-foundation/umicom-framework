/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_encoding.c
 * PURPOSE: Verify UTF-8 validation and UTF-8/UTF-16 round trips.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    static const char TEXT[] = "Umicom π 😀";
    static const unsigned char INVALID[] = {0xC0U, 0xAFU};
    UmiDocumentTextEncoding encodings[] = {
        UMI_DOCUMENT_ENCODING_UTF8,
        UMI_DOCUMENT_ENCODING_UTF8_BOM,
        UMI_DOCUMENT_ENCODING_UTF16_LE,
        UMI_DOCUMENT_ENCODING_UTF16_BE
    };
    size_t index;
    assert(umi_document_utf8_validate((const unsigned char *)TEXT,
                                      strlen(TEXT), NULL));
    assert(!umi_document_utf8_validate(INVALID, sizeof(INVALID), NULL));
    for (index = 0U; index < sizeof(encodings) / sizeof(encodings[0]); ++index) {
        unsigned char *encoded = NULL;
        size_t encoded_length = 0U;
        char *decoded = NULL;
        size_t decoded_length = 0U;
        UmiDocumentTextEncoding detected;
        int had_bom;
        assert(umi_document_encode_text(TEXT, strlen(TEXT), encodings[index],
                                        encodings[index] != UMI_DOCUMENT_ENCODING_UTF8,
                                        &encoded, &encoded_length) == UMI_STATUS_OK);
        assert(umi_document_decode_text(encoded, encoded_length,
                                        UMI_DOCUMENT_ENCODING_UTF8,
                                        &decoded, &decoded_length,
                                        &detected, &had_bom) == UMI_STATUS_OK);
        assert(decoded_length == strlen(TEXT));
        assert(strcmp(decoded, TEXT) == 0);
        umi_document_encoding_free(encoded);
        umi_document_encoding_free(decoded);
    }
    (void)printf("document encoding passed\n");
    return 0;
}
