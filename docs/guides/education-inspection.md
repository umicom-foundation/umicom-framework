# Using the education inspection SDK

The Framework inspection library reads educational metadata from files you
choose. It can describe executable headers, static-library members, ELF symbols
and load segments, page permissions, and recorded teaching IPC events. It does
not execute a binary, extract an archive, load a kernel, or connect to another
process.

Applications link the CMake target `Umicom::education_inspection`.
The same implementation is used by the standalone education command. Existing
C API names and report structures remain available.

## Choose the inspector

| Input or task | Public header | Main file operation |
| --- | --- | --- |
| ELF, PE or ordinary COFF header | `umicom/education/binary_inspect.h` | `UmiEducationBinaryInspectFile` |
| Ordinary Unix ar static library | `umicom/education/archive_inspect.h` | `UmiEducationArchiveInspectFile` |
| ELF symbol and relocation metadata | `umicom/education/elf_symbols.h` | `UmiEducationElfInspectFile` |
| ELF loadable-segment ranges | `umicom/education/elf_load_plan.h` | `UmiEducationElfLoadPlanInspectFile` |
| ELF page-permission review | `umicom/education/memory_audit.h` | `UmiEducationMemoryAuditFile` |
| Educational IPC trace | `umicom/education/ipc_trace.h` | `UmiEducationIpcTraceInspectFile` |

An ELF file is a common executable or object format on Linux and in the Umicom
Kernel work. PE is the Windows executable format. COFF is also used for object
files. A file's format does not imply that it can run on the current computer.

## Link an application

1. Install the Framework SDK using your chosen build and installation prefix.
   The main Framework build exports the inspection component with its headers.
   A build of only the standalone education CLI does not create a complete
   Framework package configuration.

2. Request the component in your application's CMake file:

```cmake
cmake_minimum_required(VERSION 3.24)
project(MyInspectionTool LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS education_inspection)
add_executable(my-inspector main.c)
target_link_libraries(my-inspector PRIVATE Umicom::education_inspection)
set_target_properties(my-inspector PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
```

3. Include the header for the operation you need. Initialise a report, call
   the inspection function and check its status before displaying its fields:

```c
#include <umicom/education/binary_inspect.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fputs("Pass the path of one binary to inspect.\n", stderr);
        return 2;
    }

    /* The caller owns this report. Read it only after this inspection succeeds,
     * so a retained result cannot be mistaken for the newly selected file. */
    UmiEducationBinaryHeader report = {0};
    const UmiStatus status = UmiEducationBinaryInspectFile(argv[1], &report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Inspection failed with status %d.\n", (int)status);
        return 1;
    }
    printf("Format: %s\n", UmiEducationBinaryFormatText(report.format));
    return 0;
}
```

The binary-file operation preserves the previous output on failure, including
a file-close failure. This protects application state; it does not make an
earlier result evidence about a different file.

## Build the installed example

The SDK installs a complete example under
`share/umicom-framework/examples/education_inspection`.
The following assumes your chosen SDK prefix is `C:\umicom-sdk`;
substitute your actual prefix. Use a separate build directory for the example
and stop if any command fails.

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S "C:\umicom-sdk\share\umicom-framework\examples\education_inspection" `
      -B "C:\umicom\inspection-example-build" `
      -G Ninja `
      -DCMAKE_C_COMPILER="C:\msys64\ucrt64\bin\gcc.exe" `
      -DCMAKE_PREFIX_PATH="C:\umicom-sdk"

cmake --build "C:\umicom\inspection-example-build" --parallel 2

& "C:\umicom\inspection-example-build\umicom-education-inspect-example.exe" `
    binary "C:\umicom\inspection-example-build\umicom-education-inspect-example.exe"
```

For a native Windows build, successful output begins with `Format: PE`.
A native Linux build normally reports `Format: ELF`.
The example accepts six operations followed by an explicit path:

```text
binary FILE
archive FILE
symbols FILE
load-plan FILE
memory-audit FILE
ipc-trace FILE
```

The `memory-audit` operation returns a nonzero exit code if it finds
writable executable pages or unknown permission flags, even if the file parsed
successfully. This is a conservative educational review, not security approval.

## Coverage and limits

Normal Framework builds with `BUILD_TESTING=ON` and the standalone
education test project use the same parser regressions. They include the
binary-file fault cases, malformed input, bounded reads, spaced paths and
the existing format fixtures. The native installed-consumer check builds this
component into a private test prefix, copies that prefix, and checks that
the consumer uses the copied headers and library. Its build-tree
`education-inspection-sdk/inspection-sdk.log` retains command output, including
warnings from successful builds. This focused check does not qualify the
complete Framework package or the application GUI. It is not registered for
cross-compilation, where an executable built for another device cannot
necessarily run on the build computer.

The inspection library itself uses the C runtime without GTK, network
connections or Kernel calls. A full SDK package may also discover dependencies
needed by other enabled Framework components.

Keep these input limits in mind:

- Binary headers use at most the first 64 KiB. A PE header beyond that prefix
  returns a capacity error.
- Archive inspection scans ordinary ar containers; thin archives are unsupported.
- ELF symbol inspection caps file size at 32 MiB and bounds table traversal.
- ELF load planning reads at most a 1 MiB prefix and bounds segment counts.
- IPC trace files are a bounded educational format, not live Kernel messages.
- Some file readers use the host's `long` seek range. File path encoding
  follows the underlying C file interface.
- Concurrent changes to an input are not a stable file snapshot. An accepted
  header or plan does not validate every byte of an executable.

If CMake reports that the component is missing, check that your prefix contains
an installation of Framework with this component and that you selected that
prefix rather than an older global SDK. If compilation succeeds but file
inspection fails, check the returned status, file format, path permissions and
the relevant size limits. Keep parser failures separate from application launch
or Kernel boot problems.

## Inspect classroom scheduling and driver records

The same installed example also accepts `schedule-trace`, `timer-trace`,
`interrupt-trace` and `evidence-trace`, each followed by a teaching-file path.
These formats model task, timer, interrupt-source and driver ownership. They
are explicitly bounded classroom records, not captures of a running Kernel.

1. Use a teaching file produced for the matching format; a filename extension
   alone does not identify valid contents.
2. Pass the file to the matching operation. The example reports the number of
   accepted records and the ownership count remaining at the end.
3. If parsing or closing the input fails, retain the previous report and show
   the returned error. The file APIs publish a replacement only on success.
4. For driver evidence, explain that the SHA-256 chain detects inconsistent
   record bytes. It is not a signature or proof that a driver recovered. Anyone
   able to rewrite the whole teaching file can also recompute its hashes.

The shared SDK and standalone education command use the same implementations.
Neither changes machine interrupts, repairs devices or executes the input.

## Use the native learning panel

In Studio, open the learning practicum from Guided Learning. The same panel is
also available to applications that embed Framework education.

1. Expand **Inspect a teaching trace**.
2. Choose Scheduler, Timer, Interrupt or Driver evidence to match the producer
   of your teaching file.
3. Browse to the file or enter its absolute path, then choose **Inspect selected
   file**. Picking a filename alone does not start inspection.
4. Read the report, which identifies its input file. A failed read preserves
   the previous labelled report and displays a separate error.
5. To ignore a pending result, choose **Discard pending result**. An accepted
   read may still finish in the background; the report will not replace your
   previous result. Changing the selected path or format also discards a result
   that no longer matches that selection.

The native panel uses bounded regular-file reads and UTF-8 Windows paths. It
rejects relative paths and final-component links; parent-directory aliases are
not confined. File reading and parsing happen away from the GTK UI thread.
Closing the panel retires its controls and discards late results. None of these
actions updates saved lesson progress, authenticates a trace producer, runs a
student program, or controls real hardware.

## Inspect device requests and completion

1. In the teaching inspector, choose **Device I/O trace**.
2. Select a teaching file produced in the documented UMIDIO1 format and press **Inspect selected file**.
3. Compare cancellation requests with acknowledged cancellations. A request to cancel
   is intent; a recorded completion determines whether the simulated operation succeeded,
   failed or was cancelled.
4. Check **Outstanding** and **Ready to reap**. A valid partial trace may still have
   unfinished requests. Inspection does not operate a device or recover Kernel state.
5. Use **Save displayed text** to retain the labelled report in a new text file.
   See [saving displayed reports](saving-displayed-reports.html) for paths and failure handling.

The installed SDK exposes **umicom/education/device_io_trace.h** through
**Umicom::education_inspection**. Its example accepts **device-io-trace FILE**.
