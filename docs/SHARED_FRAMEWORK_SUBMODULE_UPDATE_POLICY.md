# Umicom Framework — Shared-Repository Submodule Update Policy

Author / project lead: Sammy Hegab  
Organisation: Umicom Foundation

## Correct local directory map

```text
C:\umicom\
  Umicom-Applications\
    framework\                 -> umicom-foundation/umicom-framework
    applications\studio\
    applications\trader\
    applications\bank\
    applications\tms\
    ... (other thin modules)
  umicom-kernel\               -> independent native Kernel (no Framework submodule)
  umicomOS\
    framework\                 -> the SAME umicom-foundation/umicom-framework
    boot\
    desktop\
    image\
    ...
```

The canonical remote Umicom Framework has **one source owner**. A Git submodule
is a separately checked-out, pinned revision of that same repository; copying
source files into each consumer's `framework/` would create divergent edits.

## Standard order: modify once, verify, commit, push, then pull other checkouts

1. Merge the complete supplied Framework source files into
   `C:\umicom\Umicom-Applications\framework` with Beyond Compare. Review
   differences; never use a mirror/delete operation against a partial batch.
2. Build and test Framework using the parent Umicom Applications configuration.
3. At `Umicom-Applications\framework`, verify `git status`, make sure `main`
   is checked out, then run `git add -A`, `git diff --cached --check`,
   `git commit -m "..."`, and `git push origin main`.
4. At `Umicom-Applications`, build/test the pinned integration, stage the
   **Framework submodule gitlink** with `git add -A`, check the staged diff,
   commit and push the parent repository.
5. At `umicomOS\framework`, ensure the submodule worktree is clean, switch to
   `main` if necessary, then `git pull --ff-only origin main`. This is the
   same Framework source, so **do not merge copies of source files there**.
6. At `umicomOS`, configure/build/test the OS host targets, stage the changed
   Framework gitlink with `git add -A`, commit and push the OS parent repo.
7. When independently cloned standalone app repositories (Studio IDE, Trader,
   Bank, TMS, etc.) also contain their OWN root `framework/` submodule,
   explicitly update each **only if it exists locally**, test, then commit
   its parent gitlink. Thin application modules under
   `Umicom-Applications\applications\` do NOT each carry a separate
   Framework checkout; they use the single parent Framework target.
8. `umicom-kernel` is an independent native operating-system Kernel with no
   direct Framework dependency. There is NO Framework submodule to pull there.

## Important Git rules

- `git pull` on a detached submodule HEAD may fail because no current branch
  tracks `origin/main`. Check `git branch --show-current`, then use
  `git switch main` (only when the submodule is clean) before
  `git pull --ff-only origin main`.
- Do not use `git reset --hard`, `git clean -fd`, force push or silent conflict
  resolution. Preserve local edits and the accepted source history.
- A submodule pointer is stored in the **parent repository**. Pulling Framework
  inside OS does not update its remote parent pointer until `umicomOS` is
  committed and pushed.
- Check the Framework SHA in both local checkouts after synchronising. A
  mismatch indicates that at least one checkout remains on a different
  revision and is NOT automatically fixed by updating another repository.
- A clean test run is a prerequisite for publishing a changed integration pin.
- Check `git diff --cached --stat` before every `git commit`: `git add -A` stages
  all working-tree changes in the current repository, including unrelated edits.

The full batch-specific Windows commands, including OS build, target selection,
Git commit messages and optional standalone product repositories, are in the
associated Batch R02 merge guide.
