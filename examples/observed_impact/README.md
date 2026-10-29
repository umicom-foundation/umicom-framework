# Select affected work from source observations

This C23 example uses the installed Framework SDK to turn changed source bytes
into a target and test selection. It uses a small synthetic inventory and an
in-memory reader. It does not open a real project, launch its build, run its
tests, change Git state or reuse earlier test results.

## Understand the example

The inventory describes three libraries:

- **core** owns core.c.
- **view** depends on core, and view.check belongs to view.
- **other** owns other.c, and other.check is independent.

Each source observation records the size and SHA-256 digest of listed input
bytes. A digest is a content identifier, not proof that a compiler used the
content. The example changes only core.c in its source scenario. Framework
uses the existing dependency graph to select core, view and view.check.

The inventory's string fields are encoded as hexadecimal bytes. This prevents
names containing separators from becoming extra records. Real project
inventories come from configured CMake metadata; the constant in this example
is a deliberately small teaching fixture.

## Build against an installed SDK

First build and install a matching Framework SDK containing the distribution
component. Keep its headers and libraries from the same build. Then run each
command separately and stop if it fails:

```powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\examples\observed_impact" -B ".\build\observed-impact-consumer" -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DUMICOM_EXPECTED_SDK_PREFIX=C:/umicom/installed/applications -DBUILD_TESTING=ON
cmake --build ".\build\observed-impact-consumer" --parallel 2
ctest --test-dir ".\build\observed-impact-consumer" --output-on-failure --no-tests=error
```

Only this consumer is built by these commands. The configure step requires an
explicit installed prefix and checks the package origin and imported library
layout. If the new header or symbol is absent, update the SDK instead of adding
a source-tree include path as a fallback.

## Run the three scenarios

```powershell
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" same
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" source
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" first
```

| Scenario | Expected target count | Expected test count | Meaning |
| --- | --- | --- | --- |
| same | 0 | 0 | Listed input bytes match; this says nothing about previous failures. |
| source | 2 | 1 | The core edit affects its library, dependent view and owned check. |
| first | 3 | 2 | No baseline exists, so selection widens to the whole inventory. |

The programme returns 0 when planning and printing complete, and 2 on an input,
allocation or output error. These statuses do not claim a product passed.
CTest checks the exact expected selections and retains consumer.log below the
consumer's evidence directory. These are expected results to verify, not
results from a particular installation.

## Use the service in a host

Include umicom/distribution/runtime/observed_impact.h and link
Umicom::distribution. Keep a previous source observation, capture the current
one, then call UmiReleaseImpactFromObservations. Check both the returned status
and the explanation in UmiObservedImpactSummary. Destroy the owned plan after
use; the observations and inventory may be destroyed after successful creation.

A changed inventory generation or absent baseline widens selection.
A current observation from another inventory is rejected. A changed input
outside the source root also widens selection. Unknown change counts remain
explicitly unknown, rather than being described as no changes.

This service is one part of automatic qualification. A host still needs to
refresh metadata for new or removed files, observe declared non-source inputs,
detect toolchain or environment changes, schedule earlier failed or missing
checks, retain baselines and evidence, and run authorized work. It must never
treat an empty selection as a passing test run. This example does not provide
those host integrations.

Read the installed [scoped qualification guide](../../docs/guides/scoped-qualification.html)
for recording executions separately from selections.

## Try a retry without a source edit

The following additional scenarios use the same installed example:

```powershell
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" retry
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" union
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" unknown-retry
```

| Scenario | Expected target count | Expected test count | Meaning |
| --- | --- | --- | --- |
| retry | 1 | 1 | Listed bytes match, but other.check is explicitly required. |
| union | 3 | 2 | The core edit and independent other.check retry are combined. |
| unknown-retry | 3 | 2 | An absent test name widens to the complete configured graph. |

The unknown-retry scenario prints graph fallback 64, meaning UNMAPPED_TEST.
Its source-observation fallback remains zero: the source comparison is known,
while the test registration is not. These are separate explanations.

A real application supplies retry identities from checked evidence. This example
uses synthetic names and grants no reusable pass. Follow
UmiReleaseImpactFromObservationsWithTests in the public header: unchanged
source bytes do not remove explicitly required tests. The shared graph adds
declared fixture groups and executable owners; CTest still controls their
execution order. A missing registration is investigated rather than silently
dropped from the retry list.

## Derive retry names from a recorded attempt

The history scenarios exercise UmiReleaseQualificationPlanNext. Their records
are synthetic teaching inputs, not claims that a compiler or test process ran.

```powershell
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" history-pass
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" history-failed
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" history-unknown-tools
```

| Scenario | Expected targets / tests | Explanation |
| --- | --- | --- |
| history-pass | 0 / 0 | Both synthetic individual passes have compatible, stable attempt evidence. No new execution is claimed. |
| history-failed | 1 / 1 | other.check is due because its recorded outcome failed; its source bytes need not change. |
| history-unknown-tools | 3 / 2 | The current toolchain is unknown, so every configured target and test remains selected. |

Unlike the explicit retry scenario, these modes do not manually supply the
failed name to the planner. The host passes its identified earlier record,
source observations and current toolchain identity. The shared service finds
missing and nonpassing checks. Older scoped records that did not cover a test
cannot exclude it.

A host must retain the actual baseline observation and close its attempt with
after observations. A missing baseline, changed configuration, failed runner
or interrupted attempt widens work. An outstanding test removed from the
current registration is investigated rather than quietly dropped.

The example uses independent known test exit observations. Imported JUnit
results without individual exit codes require matching aggregate process
evidence in the qualification record. The failed-runner case remains
conservative even when a report contains some passing rows.

Read [scoped qualification](../../docs/guides/scoped-qualification.html#plan-next-attempt)
for the ownership and evidence limits. These APIs support a host application;
they do not create an unattended build service on their own.


## Reopen a synthetic saved attempt

The saved-pass, saved-failed and saved-unknown-tools modes have the same selections
as the corresponding history modes. They first format the report, retain its digest
and destroy the original attempt. UmiReleaseQualificationPlanFromReport then checks
the bytes and reconstructs evidence only for planning. No old execution is resumed.

```powershell
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" saved-pass
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" saved-failed
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" saved-unknown-tools
```

Each prints that the report was reopened after original attempt destruction. A real
host retains the receipt independently and decides which evidence origins it trusts.
A matching hash is integrity against that receipt, not authenticated execution.
The source baseline is still supplied separately. This example keeps observations
in memory and does not implement persistent observation storage. If that baseline
is unavailable after a restart, pass NULL and accept the full selection.

The reader refuses malformed or unsupported fields, inconsistent counts, incorrect
plan identities and unknown reason bits. Report length excludes the string terminator.
On refusal neither the plan nor its explanation is changed. A new reader accepts old
supported reason values and the new required-test flags; an older SDK may refuse the
new flags and needs a matching SDK update. Keep the earlier report for investigation.


## Reopen the source baseline too

The restored-pass, restored-failed and restored-edited modes encode the earlier
source observation, destroy its original allocation, and decode it against the
same configured inventory and separately retained digest. They then reopen the
qualification report after destroying that original attempt. Both inputs to the
next planning call are independently owned reconstructed historical data.

```powershell
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" restored-pass
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" restored-failed
& ".\build\observed-impact-consumer\umicom-observed-impact.exe" restored-edited
```

Expected selections are respectively 0 targets / 0 tests, 1 target / 1 test, and
2 targets / 1 test. The edited scenario selects core, view and view.check from
fresh current bytes even though the historical checks passed synthetically.

See source_observation_archive.h and ExampleRestoreBaseline for ownership rules.
The encoded baseline contains paths, sizes and hashes, not old source contents.
Its paths can be private workstation data. Use a private host-selected location
and retain receipts independently. These teaching modes operate on memory buffers;
host storage, retention, fresh metadata and scheduling are separate responsibilities.
