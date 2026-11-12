![Umicom](../../resources/brand/umicom-logo.svg)

# Create workspace directories through checked parents

UmiRootedDirectoryEnsure creates missing descendants beneath an explicitly
selected existing directory. It extends the shared rooted-files service, so
clients use the same portable name policy for directory preparation and file
publication. It does not create the selected root or grant project execution
trust.

## Use the public contract

~~~c
UmiRootedDirectoryCreation progress;
UmiStatus status = UmiRootedDirectoryEnsure(
    workspace_root, "generated/source", &progress);
if (status == UMI_STATUS_OK) {
    /* The ordinary directory chain was available at completion. */
} else {
    /* Explain status and directories_created; retain partial work. */
}
~~~

Include umicom/platform/rooted_files.h and link the installed Umicom::platform
target. The root must be an ordinary absolute directory. The relative path can
contain several portable components; slash and backslash spelling is normalized.
The complete name is validated before any directory is opened or created.

The report is required and initialized for each attempt. Its status matches the
return value. directories_created counts successful native directory creations,
including progress before a later refusal. complete is true only after traversal
and owned-handle cleanup succeed. Existing directories contribute zero to the
creation count. Input strings must remain readable and must not overlap the
writable report.


## Reserve a new final directory name

Use UmiRootedDirectoryCreate when the final component must be new. Missing
intermediate directories are prepared using the same checked traversal, but the
native mkdirat or CreateDirectoryW operation makes the final existence decision.
An already existing final directory, file or link returns
UMI_STATUS_ALREADY_EXISTS. A link in an intermediate component is still refused.

~~~c
UmiRootedDirectoryCreation progress;
UmiStatus status = UmiRootedDirectoryCreate(
    projects_root, "examples/ledger", &progress);
if (status == UMI_STATUS_ALREADY_EXISTS) {
    /* Choose another name; do not erase or overwrite the existing project. */
}
~~~

A successful call proves that this attempt created the final name. It does not
keep a handle open for the caller or reserve that name indefinitely. Other
processes can still modify the destination after the call. Serialize project
creation, and use exclusive rooted file creation for each subsequent new file.
If a native creation succeeds but opening or closing its handle fails, the
report retains the creation count while complete remains false. Preserve that
partial result for review rather than silently retrying it as a new project.

## Follow the ownership boundary

On POSIX, each component is opened relative to the currently held directory
descriptor with directory and no-follow flags. A missing component is created
with mkdirat, then opened through the same admission rule. An EEXIST result alone
does not prove that the object is a directory: it can also describe another
object or a symbolic link. This follows the descriptor-relative semantics
described in the [Linux mkdir manual](https://man7.org/linux/man-pages/man2/mkdir.2.html)
and [open manual](https://man7.org/linux/man-pages/man2/open.2.html).

On Windows, the existing rooted-file directory helper opens and checks each
directory without following a reparse point and retains parent handles without
delete sharing. Missing components are created one at a time with CreateDirectoryW
and checked again before descending. Windows documents that CreateDirectoryW
creates only the final component and that default security attributes inherit
parent policy. See [CreateDirectoryW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createdirectoryw)
and [CreateFileW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew).

New POSIX directories request mode 0700, subject to the process umask and default
ACL rules. Windows uses the parent's inherited security policy. The operation
does not change permissions on existing directories.

## Interpret a partial result

1. Keep the returned status and creation count together.
2. Preserve any newly created directories after failure; this API never deletes
   or rolls back directories.
3. Inspect the failure before retrying. A retry can legitimately create zero
   directories because an earlier attempt already created part of the chain.
4. Do not treat a completed directory call as a reservation for a later file
   write. Use the shared rooted file operations for that later effect.
5. Retain failures and skips when recording qualification evidence.

Ancestors above the chosen root remain trusted. POSIX descriptors retain object
identity if names are moved; this is not a promise that external rename activity
cannot change the visible path hierarchy. Mounts, privileged modifications,
concurrent content edits and power-loss durability are not isolated by this
operation. Callers must serialize their own edits.

UmiWorkspaceCreateEntry remains the graph-authorized operation for one exclusive
new entry whose parents already exist. It has a different contract from ensuring
an existing-or-new chain. Both retain the shared portable name policy; this
lower-level operation does not manufacture a trusted workspace graph.

<!-- Historical integration status, retained for review:
The existing project generator has not yet been migrated to this directory
service. This additive API therefore does not, by itself, repair the generator's
legacy existence-check/write sequence. File creation and full project
transaction design remain separate integration work.
-->
New-project creation now composes this directory service with exclusive rooted
file creation. See [checked project creation](checked-project-creation.md) for
model/profile preflight, partial progress and owner qualification. The legacy
additive generator retains its earlier skip/overwrite contract. This integration
does not turn several filesystem operations into one transaction.

## Qualify native behavior

Run each command separately in a prepared Framework qualification checkout and
stop on errors.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-rooted-directories-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]filesystem[.]rooted-directory[.]"
~~~

Thirty-six authored cases cover single and nested creation, repeated calls,
existing components, separator spelling, Unicode names, retained child content,
invalid late components, missing/file roots, capacity, partial progress and
directory links at the leaf, parent and root. Exclusive final-name cases also
cover existing directories, files and links, checked parents, Unicode names,
invalid arguments, missing roots and retained child content. Fixtures create unique directories
under their CTest working folders and retain them for diagnosis.

Link cases return skip code 77 when the host cannot create the required link.
The partial-progress case uses a component longer than common native filesystem
limits; if the filesystem accepts it, that failure fixture is reported skipped.
A skip is missing coverage, not a pass.

Run the existing rooted file/read/write/create cases as well. Record the source
revision, operating system, filesystem, compiler and complete test output.
Compiler, native test and adversarial concurrency results are not established by
the authored cases or static source inspection.
