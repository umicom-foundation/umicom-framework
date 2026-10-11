# Umicom Framework — C23 Native FAT16 Disposable-Disk Qualification

Author: Sammy Hegab, Umicom Foundation. Licence: MIT.

## Integration point and purpose

This delivery extends `Umicom::vm_manager` and the native `umicom` command, rather than adding another PowerShell test runner. The live Kernel remains a separate repository, with its own freestanding C23/Assembly, QEMU test definitions and C23 FAT16 image verifier. Umicom OS and Umicom Studio are consumers of Framework services, not owners of duplicate QMP, process or disk-lifetime implementations.

The new, explicit developer command is:

```text
umicom qemu kernel-disk --qemu ABSOLUTE_EXE --firmware ABSOLUTE_ELF
  --directory ABSOLUTE_DIR --fixture ABSOLUTE_RAW
  --scratch-directory NEW_ABSOLUTE_DIR --verifier ABSOLUTE_C23_EXE
  --mode stage|finish --slot 7
```

It exercises the known synthetic 8-MiB FAT16 fixture used by the Kernel tests. It is **not** a general utility for writing a user's disk, an installer, a filesystem recovery procedure or a guest scheduler test. The fixture is accepted only if its file length is 8,388,608 bytes and its SHA-256 matches `bd866d6ae337527f3a8b4609f31969d60185e025f8525726f573d905a2f50938`.

## Canonical local source layout

```text
C:\umicom\
  Umicom-Applications\
    framework\                       (Framework source, Git submodule)
      cmake\                          (existing command and VM composition)
      include\umicom\vm_manager\     (new public C23 contract)
      src\vm_manager\                (new C23 scratch-disk service and policy)
      tools\umicom\src\              (new native command, no PS script)
      tests\vm_manager\              (new deterministic C23 tests)
      docs\vm_manager\               (this guide)
  umicom-kernel\                      (independent Kernel repository)
  umicomOS\                           (independent OS distribution repository)
```

The delivery is installed by extracting the ZIP at `C:\umicom`. Existing or newly created files must be reviewed before merging. No files in the Kernel or OS trees are replaced in this batch.

## Safe boot policy

Normal `umicom qemu plan|review|run` uses `snapshot=on` for ordinary disk inputs. That source remains unchanged. The new C23 service executes the following explicitly bounded acceptance journey:

1. Refuse any existing output directory or non-absolute input. Confirm QEMU, firmware, verifier and the working directory are present.
2. Read the entire *known synthetic* source through Framework's checked file service. Verify the pinned SHA-256 and byte length.
3. Create a new output directory and copy the fixture to `umicom-kernel-owned-fat16.raw`, using `UmiSetupFileCopyChecked`, then verify the copy's complete digest. The source is never passed as QEMU's writable disk.
4. Build the **existing** VM boot plan for the Kernel target. On the internally owned argument vector only, replace `snapshot=on` with explicit `readonly=off,cache=writeback` for the **copied** disk; use modern VirtIO with `write-cache=on` and 512-byte logical/physical sectors. No default VM boot-policy change is made.
5. Use `UmiVmBootReview` and `UmiVmBootStart` for input fingerprinting, QMP handshake, paused start and managed child lifetime. Resume and exercise the real shell via the existing `UmiVmConsoleJourneyRunNative` C23 service.
6. After Stage, or after Finish and explicit lease release, stop **only the owned process**. This is deliberately not described as a graceful Kernel shutdown.
7. Execute the existing independent Kernel C23 `umicom-fat16-guest-commit-evidence` against `BEFORE.raw AFTER.raw stage|finish`. Its success must include a complete-image comparison, `classification=verified` and `repair_performed=false`. The Framework does not replace the Kernel's disk-format or allocation audit.
8. Start a second owned QEMU session on the same stopped disk **read-only**, without a snapshot and without an implicit update. In staged mode the normal read-only Kernel mount must reject an unclean volume; in finished mode a read-only mount must succeed.
9. Hash the entire copied disk again and refuse any change during the reader reboot. Hash the original source again and require exact identity to its pre-run hash. Leave the copied disk in place for evidence.

Only the snapshot-to-owned-copy transformation is new. No private raw QMP parser, process tree management or filesystem-repair implementation is introduced.

## Specific guest commands

The caller supplies `--slot 7` only if `disks` reports that slot. The new journey confirms its `block.slot=7 base=` line before any mutation command.

For a **staged** journey:

```text
disks
fatcommitopen 7 0
fatstage /FRAG.BIN 511 "Umicom ordered update"
fatcommitinfo
```

The accepted Stage has completed the ordered persistence barriers, but the FAT flags remain dirty. The process is then stopped, and the next QEMU run attempts `mountdisk 7 0` read-only. Expected evidence includes `disk.media=unclean-volume`.

For a **finished** journey, the same opening sequence continues:

```text
fatcommit
fatcommitclose
```

A fresh read-only guest then expects `disk.mount=ok` and `disk.mount-state=mounted`. The independent host verifier requires exactly the expected 21 data bytes, unchanged other bytes, restored clean FAT headers and a fully compared disk image.

Neither journey cuts a WRITE/FLUSH *in flight*. The next fault-injection milestone must address those narrower points separately; do not interpret a successful staged guest as simulated power-loss correctness during incomplete DMA or a backend power failure.

## Windows integration without new scripts

Build the Framework native tool in your existing consolidated Applications tree, and build the Kernel's existing standalone verifier and normal firmware in the independent Kernel directory. The following lines are terminal commands, not files to add to the repositories.

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
cmake --preset windows-ucrt64-all-debug -DBUILD_TESTING=ON -DUMICOM_BUILD_NATIVE_TOOL=ON
cmake --build --preset windows-ucrt64-all-debug --target umicom umicom-kernel-disk-plan-tests umicom-kernel-disk-qualification-tests --parallel 2
ctest --preset windows-ucrt64-all-debug -R 'framework[.]command[.]qemu[.]kernel_disk_' --output-on-failure --no-tests=error

Set-Location 'C:\umicom\umicom-kernel'
cmake -S .\tools\fat16-recovery-audit -B .\build\fat16-recovery-audit -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build .\build\fat16-recovery-audit --parallel 2
ctest --test-dir .\build\fat16-recovery-audit --output-on-failure --no-tests=error
cmake --preset riscv64-clang-debug
cmake --build --preset riscv64-clang-debug --parallel 2
ctest --preset riscv64-clang-debug --output-on-failure --no-tests=error
```

Now choose a **new** output directory that you have not used in earlier tests:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$umicom = '.\build\windows-ucrt64-all-debug\bin\umicom.exe'
& $umicom qemu kernel-disk --help
& $umicom qemu kernel-disk `
  --qemu 'C:\msys64\ucrt64\bin\qemu-system-riscv64.exe' `
  --firmware 'C:\umicom\umicom-kernel\build\riscv64-clang-debug\bin\umicom-system.elf' `
  --directory 'C:\umicom\Umicom-Applications' `
  --fixture 'C:\umicom\umicom-kernel\tests\disk_inspection\fixture.raw' `
  --scratch-directory 'C:\umicom\Umicom-Applications\build\kernel-disk-stage-001' `
  --verifier 'C:\umicom\umicom-kernel\build\fat16-recovery-audit\umicom-fat16-guest-commit-evidence.exe' `
  --mode stage --slot 7
```

If that run succeeds, use **another new scratch directory** and `--mode finish` to qualify clean finalisation. Do **not** point `--scratch-directory` at the original fixture or an existing disk directory. The command refuses an already existing output directory. Normal Framework/Kernel tests remain independent; a successful isolated native test or clean build is not a substitute for the actual QEMU journey.

## Engineering limits and future work

- The C23 coordination and QEMU argument policy are portable source, but this package was host-qualified under GCC and Clang with deterministic VM/file-process substitutes. A full consolidated Windows Framework build and actual QEMU runs remain mandatory before accepting guest durability evidence.
- The exact synthetic disk fixture and `umicom-fat16-guest-commit-evidence` are intentionally required. The tool does not claim to understand arbitrary FAT16 volumes or to recover production data.
- The output directory is chosen by a trusted local developer. It should be private and not subject to concurrent modification or symlink attacks. Review fingerprints are integrity observations, not authenticated leases protecting hostile host races.
- An interrupted guest at an arbitrary instruction or an unexpected storage error can leave any portion of a physical sector changed. No automatic retry, repair or rollback is implemented by this batch.
- `umicom qemu kernel-console`, `kernel-qualify`, and existing VM Manager commands remain intact; no Kernel code depends on hosted Framework services.
- Umicom OS and Applications can consume the shared service through the C23 API; updating their Framework gitlinks is a separate, evidence-gated repository operation.
