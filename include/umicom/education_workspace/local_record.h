/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education_workspace/local_record.h
 * PURPOSE: Open learning records only at a location explicitly selected by their owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDUCATION_WORKSPACE_LOCAL_RECORD_H
#define UMICOM_EDUCATION_WORKSPACE_LOCAL_RECORD_H
#include "umicom/education_workspace/workspace.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Open or create a SQLite learning database at an explicit absolute UTF-8 file
 * path. The parent directory must already exist; no application-data default or
 * memory fallback is selected. Windows requires a local drive path. URI-like
 * query/fragment syntax, directories and control characters are refused.
 * The learner ID and name are validated before creating a database. An existing
 * learner keeps its saved name and progress. Corrupt records are reported, not
 * reset; a newly created database may remain if a later step fails.
 * When both output pointers are supplied, both slots are cleared on failure.
 * They must be distinct, empty owner slots.
 * On success close the workspace first, then destroy the returned Data Server.
 * One owner thread uses the pair. This is plaintext local study storage, not an
 * authenticated account, encrypted vault or protection against path replacement. */
    UmiStatus UmiEducationLocalRecordOpenAt(const char *database_path, const char *learner_id,
                                            const char *display_name, UmiDataServer **out_server,
                                            UmiEducationWorkspace **out_workspace);
#ifdef __cplusplus
}
#endif
#endif
