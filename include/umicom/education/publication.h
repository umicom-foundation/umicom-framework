/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/publication.h
 * PURPOSE:
 *   Verify and inventory a completed educational website without copying
 *   publication logic into Bits2Banking or requiring Python.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_PUBLICATION_H
#define UMICOM_EDUCATION_PUBLICATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Hash every regular site file with the existing Framework SHA-256 service.
 * The manifest lives at output/native-publication/site.sha256, outside the
 * served site tree. Entries are lexically ordered by portable relative path.
 * On an identical existing manifest, return success without changing its
 * bytes. A different manifest is refused unless refresh is explicitly set.
 * The function never edits site files or original educational sources.
 * A manifest is integrity evidence, not a cryptographic signature. */
int UmicomEducationPublicationManifest(const char *repositoryRoot, int refresh);

/* Compare the complete current site against the recorded manifest, detecting
 * missing, additional, altered and linked files. Read-only: no publisher,
 * external dependency, Git push, network access or automatic repair.
 * This is a local build qualification, not a remote-host trust boundary. */
int UmicomEducationPublicationVerify(const char *repositoryRoot);

#ifdef __cplusplus
}
#endif
#endif
