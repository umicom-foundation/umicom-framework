/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_watermark_resource.c
 *
 * PURPOSE:
 *   Verify canonical temporary artwork bytes, independent owners and cleanup after injected I/O failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/media_watermark_gtk4.h"
#include "../../adapters/gtk4/media_workspace_style.h"
#include "fixture.h"

static const char *Failure;
static gboolean Refuse(const char *operation)
{
    return Failure != NULL && strcmp(Failure, operation) == 0;
}
static GFile *Create(const char *pattern, GFileIOStream **stream, GError **error)
{
    if (!Refuse("fail-create")) return g_file_new_tmp(pattern, stream, error);
    *stream = NULL;
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Synthetic create failure");
    return NULL;
}
static gboolean Write(GOutputStream *stream, const void *bytes, gsize size,
                      gsize *written, GCancellable *cancel, GError **error)
{
    if (!Refuse("fail-write"))
        return g_output_stream_write_all(stream, bytes, size, written, cancel, error);
    CHECK(g_output_stream_write_all(stream, bytes, size / 2U, written, cancel, error));
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Synthetic partial write");
    return FALSE;
}
static gboolean Close(GIOStream *stream, GCancellable *cancel, GError **error)
{
    gboolean result = g_io_stream_close(stream, cancel, error);
    if (!result || !Refuse("fail-close")) return result;
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Synthetic close failure");
    return FALSE;
}
static gboolean Delete(GFile *file, GCancellable *cancel, GError **error)
{
    if (!Refuse("fail-delete")) return g_file_delete(file, cancel, error);
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Synthetic delete failure");
    return FALSE;
}
/* Interpose only resource I/O boundaries. The test never changes TMPDIR or
 * touches an existing user file; every pathname originates in g_file_new_tmp. */
#define g_file_new_tmp Create
#define g_output_stream_write_all Write
#define g_io_stream_close Close
#define g_file_delete Delete
#include "../../adapters/gtk4/media_watermark_gtk4.c"
#undef g_file_new_tmp
#undef g_output_stream_write_all
#undef g_io_stream_close
#undef g_file_delete

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    Failure = argv[1];
    UmiMediaWatermarkFile *owner = NULL;
    if (strcmp(Failure, "null-output") == 0) {
        CHECK(UmiMediaWatermarkFileCreate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaWatermarkFilePath(NULL) == NULL);
        CHECK(UmiMediaWatermarkFileDestroy(NULL) == UMI_STATUS_OK);
        return 0;
    }
    CHECK(strcmp(Failure, "bytes") == 0 || strcmp(Failure, "unique") == 0 ||
        strcmp(Failure, "fail-create") == 0 || strcmp(Failure, "fail-write") == 0 ||
        strcmp(Failure, "fail-close") == 0 || strcmp(Failure, "fail-delete") == 0 ||
        strcmp(Failure, "missing-cleanup") == 0 || strcmp(Failure, "occupied-output") == 0);
    UmiStatus expected = Refuse("fail-create") || Refuse("fail-write") || Refuse("fail-close")
        ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
    CHECK(UmiMediaWatermarkFileCreate(&owner) == expected && owner != NULL);
    char *captured = g_strdup(UmiMediaWatermarkFilePath(owner));
    if (Refuse("fail-create")) {
        CHECK(captured == NULL);
    } else {
        CHECK(captured != NULL && g_file_test(captured, G_FILE_TEST_IS_REGULAR));
        if (expected == UMI_STATUS_OK) {
            char *actual = NULL, *source = NULL;
            gsize actualSize = 0U, sourceSize = 0U;
            CHECK(g_file_get_contents(captured, &actual, &actualSize, NULL));
            CHECK(g_file_get_contents(argv[2], &source, &sourceSize, NULL));
            CHECK(actualSize == sourceSize && memcmp(actual, source, sourceSize) == 0);
            g_free(actual); g_free(source);
        }
    }
    if (Refuse("unique")) {
        UmiMediaWatermarkFile *other = NULL;
        CHECK(UmiMediaWatermarkFileCreate(&other) == UMI_STATUS_OK);
        CHECK(strcmp(captured, UmiMediaWatermarkFilePath(other)) != 0);
        CHECK(UmiMediaWatermarkFileDestroy(other) == UMI_STATUS_OK);
        CHECK(g_file_test(captured, G_FILE_TEST_IS_REGULAR));
    }
    if (Refuse("occupied-output")) {
        UmiMediaWatermarkFile *before = owner;
        CHECK(UmiMediaWatermarkFileCreate(&owner) == UMI_STATUS_INVALID_STATE && owner == before);
        CHECK(strcmp(captured, UmiMediaWatermarkFilePath(owner)) == 0);
    }
    if (Refuse("missing-cleanup")) {
        GFile *file = g_file_new_for_path(captured);
        CHECK(g_file_delete(file, NULL, NULL)); g_object_unref(file);
    }
    CHECK(UmiMediaWatermarkFileDestroy(owner) ==
        (Refuse("fail-delete") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
    if (Refuse("fail-delete")) {
        CHECK(g_file_test(captured, G_FILE_TEST_IS_REGULAR));
        GFile *file = g_file_new_for_path(captured);
        CHECK(g_file_delete(file, NULL, NULL)); g_object_unref(file);
    }
    if (captured != NULL) CHECK(!g_file_test(captured, G_FILE_TEST_EXISTS));
    g_free(captured);
    return 0;
}
