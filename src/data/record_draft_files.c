/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reuse exclusive output and bounded rooted reads instead of inventing another
 * file replacement policy. Recovery must never overwrite another draft. */
#include "umicom/data/record_draft_files.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/rooted_files.h"
UmiStatus umi_record_draft_snapshot_write_new(const char *path, const UmiRecordDraftSnapshot *snapshot)
{
    UmiStatus status = UmiOutputFileValidatePath(path);
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_encode(snapshot, &bytes, &length);
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, bytes, length);
    if (file != NULL) {
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    UmiOutputFileDestroy(file);
    umi_record_draft_snapshot_bytes_free(bytes);
    return status;
}
UmiStatus umi_record_draft_snapshot_read(const char *directory, const char *relative_path, UmiRecordDraftSnapshot *output)
{
    if (output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiRootedFileRead(directory, relative_path,
        UMI_RECORD_DRAFT_SNAPSHOT_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_decode(bytes, length, output);
    UmiRootedFileFree(bytes);
    return status;
}
