![Umicom](../../resources/brand/umicom-logo.svg)

# Review project paths before generation

A project plan contains relative file names and their rendered text. Before a
writer creates anything, Framework validates every owned string, admits every
portable relative name and compares the complete set for conflicts. This
prevents predictable path failures from leaving a partially generated project.

## Prepare and inspect a plan

1. Select the recipe by its stable identifier.
2. Validate the request and prepare variables with the shared project service.
3. Build an owned generation plan. A refused replacement leaves the previous plan
   unchanged.
4. Inspect the complete proposed file list and destination.
5. Explicitly authorize creation only after reviewing the current plan and the
   destination. The native CLI rerenders its current recipe when creating; it
   does not apply a persisted immutable preview token.

The plan retains its previous restrictions on rooted names, colons and any
double-dot substring. It now also uses the portable rooted-file name policy:
empty components, dot components, trailing spaces or dots, reserved device names,
.git components, control bytes and ambiguous Windows punctuation are refused.
Names accepted on one host should not silently become device names on another.

## Compare names through the shared service

~~~c
UmiRootedPathRelation relation;
UmiStatus status = UmiRootedFileCompareRelativePaths(
    "SRC\\Main.c", "src/main.C", &relation);
if (status == UMI_STATUS_OK) {
    /* relation is UMI_ROOTED_PATH_EQUAL under the portable lexical policy. */
}
~~~

The comparison admits both names before publishing its result. On refusal the
output value remains unchanged. UmiRootedFileValidateRelativePath exposes the same
name policy without requiring an existing or hypothetical root.

| First path | Second path | Relation |
| --- | --- | --- |
| src/main.c | SRC/Main.C | Equal |
| src\main.c | src/main.c | Equal |
| src | src/main.c | First is parent |
| src/main.c | SRC | Second is parent |
| src | src2/main.c | Disjoint |
| file | file.txt | Disjoint |

The relation normalizes slash/backslash spelling and folds only ASCII letters.
Every generation-plan record describes a file, so equal or parent/child relations
are refused with ALREADY_EXISTS. Sibling paths remain valid. All name admission
finishes before pairwise comparison, preserving the diagnostic for a malformed
later path even if earlier records also collide.

This is deliberately conservative across platforms: two ASCII case variants may
be distinct on a particular filesystem but are still refused in one portable
project plan. Unicode letters are compared as bytes beyond ASCII folding. Do not
infer Unicode case equivalence, normalization equivalence, short-name identity,
symbolic-link resolution or mount containment from a disjoint result.

## Interpret generator reports

The generator resets its supplied report at the start of every attempt, including
a null request or refused plan. Report storage must not overlap the request or
plan. All root-plus-relative path joins are checked before directory creation or
filesystem probes. A later destination-length failure therefore does not generate
earlier files.

A dry run can still inspect existing paths to report skip behavior; it performs
no creation. A pure generation-plan preview does not inspect the filesystem.
Counts in a dry-run report describe proposed work, not completed disk writes.

The existing generator preserves its skip/overwrite options and supports legacy
callers. It does not provide a multi-file rollback transaction. Its existence
check and legacy text write do not reserve a leaf against another writer.
Creation requires an exclusively controlled destination, and new partial files
must be retained for inspection after an I/O failure. The rooted-file service has
stronger individual file operations, but this path preflight does not claim that
the entire project writer now uses handle-relative directory creation.

## Qualify the change

Use a Framework checkout and toolchain selected for qualification. Run each
command separately and stop on errors.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-relative-path-relation-test umicom-project-path-conflicts-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.](filesystem[.]relative-path|developer_project[.]path-conflicts)[.]"
~~~

There are 26 pure relation cases and 17 generator admission cases. The generator
cases compile its actual implementation through a narrow filesystem probe/effect
seam; path joining and plan validation remain real Framework services. They
require zero filesystem probes and effects on admission refusal, fresh reports,
unchanged input plans and the expected dry-run observation on accepted input.

These cases are authored qualification coverage, not recorded execution results.
Also run the existing template, generation, project CLI and installed-SDK course
cases. Separately qualify real filesystem failures, links, concurrent writers and
generated application behavior before making broader claims.
