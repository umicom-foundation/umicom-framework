# Writing useful Umicom documentation

Umicom documentation serves people learning to program, people using the applications and developers maintaining shared components. A reader should be able to understand a page without knowing the conversation or development batch that produced it.

## Begin with something the reader can do

Choose a clear task: open a project, save a note, add a menu command, inspect a compiler error, or test a simulated account calculation. Give the page a descriptive title, such as **Editing and saving files in Studio**. Explain what the reader needs before starting and what they should have working at the end.

Write in natural, professional English. Assume no previous knowledge when a page is labelled introductory, but do not talk down to the reader. Define a new term where it first becomes useful. For example:

> A working copy is the version of a document that you are editing. It can contain changes that have not yet been saved to the file on disk.

Explain one new idea at a time. A useful example has a purpose in an application: keeping a note in memory, displaying a project panel, calculating a training account total, or responding to a Save command. Avoid personal greetings in example programs and unrelated novelty exercises.

## Keep instructions small and explicit

State the working directory before a command. Label Windows PowerShell and Linux shell examples separately. Explain the normal result and the condition that means the reader should stop. Use ordinary commands rather than a large helper script when a few clear steps will do.

Show `git status` before changing branches or staging files. A detached checkout may contain work not on `main`; it must be examined before switching. Do not teach force-push, forced checkout, recursive deletion or hard reset as routine recovery. Remind the reader that `git add -A` includes deletions and unrelated work in that repository.

For projects with submodules, explain that the parent records child commit IDs. Publish the changed child repositories first, update the native Umicom repository lock, then publish the parent. Do not ask a reader to create an empty commit in an unchanged child repository.

## Teach from code that exists

Check function names, headers, CMake targets and application commands against the source. A proposed API must be labelled as a design proposal rather than shown as a working feature. Prefer complete, buildable examples over fragments that quietly depend on missing helpers.

Keep reusable logic in Framework examples and services. Application examples should show how to compose those services, not teach a second document store, permission system, build engine or database layer. Preserve established public identifiers, lesson IDs, attribution and Master Controller / Slave Controller terminology.

Explain ownership, capacity, return values and error handling alongside C examples. A bad-code example should be clearly marked for inspection and excluded from executable targets. Financial examples must use simulated data and explain their units. They must not encourage live orders or real payments as practice exercises.

## Show the expected behaviour

After a command or interaction, say what the reader should see. A successful build should lead to a specific executable; a search exercise should identify a particular occurrence; an Undo exercise should restore specific text. Include a small change for the reader to make and explain how to check it.

Describe important limitations where they affect the task. For example, a size limit belongs beside the instructions for opening a file, and a missing runtime dependency belongs beside installation instructions. Do not describe unimplemented or untested behaviour as a completed capability.

## Separate teaching from delivery evidence

Public guides explain how to use, build and extend the software. They are not personal status reports. Do not address a repository page to an individual, refer to a private chat, list how long an assistant worked, or fill a tutorial with batch totals and pass-rate claims.

Keep release-specific logs, baseline hashes, failed probes, manifests and validation reports with the release's delivery evidence. A technical test report can be valuable, but it should be clearly separated from the lesson. Stable contributor instructions may explain how to run the tests without claiming results from a particular development session.

Before publishing, check the commands and local links, compile the supplied example, review the page as a new reader, and remove anything that requires private context. A page is ready when another person can follow it without guessing the missing steps.
