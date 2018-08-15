_ViFi file management tool_

# Usage

> ViFi is still considered beta, make sure you have a working backup.

## Command Line and Workflow

Using ViFi to change entries in a directory is straightforward. We just start
the vifi script with the directory path as argument:

    <joe@work:~> vifi path/to/directory

No other options yet. The entries in the directory are scanned recursively and
stored with their relative paths in a text file. The vifi script then opens the
text file with the editor found in the `$EDITOR` environment variable.  Please
make sure that this shell variable is set to your favourite editor.

When the editor is closed, ViFi will examine the changes made to the text file
and interpret them as filesystem operations. The user is asked to acknowledge
the operations (type 'y' for yes) before they are carried out. Otherwise the
changes can be revised in the text editor (type 'r' for revise) or aborted
(type 'n' for no).

By default ViFi creates a hidden temporary directory named `.ViFi` in the
scanned directory. It is used to store the text files, and as a temporary space
for files and directories that are moved or copied around.
The filesystem operations should be read as

* `<---` moves the entry out to temporary space, `<===` copies it.
* `--->` moves the entry in from temporary space, `===>` copies it.
* `[x]` means the entry will be deleted.
* `[*]` means a new intermediate directory will be created.

Going through a temporary directory for all filesystem operations avoids naming
conflicts, for example when swapping two files. It also ensures a well defined
state of directory copies given the semantics (see below).

> There are good reasons for having the `.ViFi` directory close to the changed
> files, and not in place like `/tmp`. Moving files around will cause unnecessary
> data copying when crossing filesystem barriers (different mounts).

Hidden files and directories are not shown in the text file, but they are
certainly included as part of the content when their parent directories are
moved or copied. It is also possible to call vifi directly on a hidden
directory, like:

    <joe@work:~> vifi .config

## Editing Basics

Before we get deeper into the details we should revise what basic operations
are expressible when editing the ViFi text file.
So this is what we can tell ViFi to do with our files and directories:

* _Move_ and rename an entry, by changing the filesystem path in a line.
* _Copy_ and rename an entry, by copying the line and changing its path.
* _Create_ new directories implicitly by introducing them in a changed path.
* _Delete_ an entry by removing its line.

You may have noticed the hexadecimal id at the beginning of each line.  It
identifies the original entry, independent of the filesystem path, and allows
ViFi to track the changes made to the text file. Therefore it is crucial that
each line begins with a valid hexadecimal id and a tab as separator between id
and path. My recommendation is to always treat the entries as complete lines in
your editor, e.g. copy lines with `yy` and delete them with `dd` in vi.

Beware that some editor settings may replace tabs with spaces, which will not
work with ViFi. Please adhere to the following rules for text editing:

* Every line starts with a valid hex id found in the original text file.
* The hex id is followed by a tab as separator.
* The path consists of all characters between separator and line ending.
* Directory and file names in the path are separated by forward slashes `/`.
* Except slash and line ending, all characters in the path are used unchanged.

> Even though ViFi is quite tolerant with special characters, other programs
> may have problems or require escaping in filesystem paths. Further
> restrictions on the possible directory and file names are imposed by the
> underlying filesystem. These are not checked by ViFi. See
> https://en.wikipedia.org/wiki/Filename#Reserved_characters_and_words

For logical reasons each path in the text file must be unique.

## Directories

Directories are seen by ViFi as entities including all files and subdirectories
they contain. This means that copies of a directory are made recursively and
will reproduce the content of the original directory.

> Defining the semantics that way has some benefits. We can treat copied
> directories exactly like moved directories. And hidden files and
> subdirectories are preserved, even if they are not visible.

More specific, a copy of a directory will reproduce the content of the original
directory at the time when it is copied out to temporary space.

Let's use an icon directory for example.

    # ViFi@/tmp/Build/Icons
    01      FileIcons
    02      FileIcons/close-file-08.svg
    03      FileIcons/open-file-03.svg
    04      Symbols
    05      Symbols/letter.svg
    06      Symbols/warning.svg

We create two copies of the `FileIcons` directory named `actions` and `menu`.
Then we remove the original.

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    01      icons/menu
    02      FileIcons/close-file-08.svg
    03      FileIcons/open-file-03.svg
    04      Symbols
    05      Symbols/letter.svg
    06      Symbols/warning.svg

Both copies are placed in a newly created `icons` directory and implicitly
contain `open-file-03.svg` and `close-file-08.svg`. If we clean up the file
names of those two icons in the original directory

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    01      icons/menu
    02      FileIcons/close-file.svg
    03      FileIcons/open-file.svg
    04      Symbols
    05      Symbols/letter.svg
    06      Symbols/warning.svg

we get cleaned up icon names in both copies of the directory. This is because
the icon file renames happen before the original directory is moved out to
temporary space.

> ViFi has no internal notion of files and directories, since they are treated
> equally. Beware that if you accidentally misuse a regular file as a directory
> and move entries "into" it, ViFi will choke on that. Even though that never
> happened to me (yet), I agree that ViFi needs a plausibility check for this
> situation, which is planned already.

## Internal First

As a general rule, _internal_ changes in a directory are carried out before all
other changes. This includes removal of files. _Internal_ here means any change
that does not involve files or directories from outside.

To revisit the icon example, say we only want `close-file.svg` in the `menu`
copy of the directory, but not in the `actions` copy. We can do so by moving
`close-file.svg` out of the original into the `menu` directory.

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    01      icons/menu
    02      icons/menu/close-file.svg
    03      FileIcons/open-file.svg
    04      Symbols
    05      Symbols/letter.svg
    06      Symbols/warning.svg

Why does it work? ViFi will first move out `close-file.svg` as an internal
change, before copying the `FileIcons` directory. After that. `close-file.svg`
is moved back in from temporary space, into the `menu` directory copy.

> The internal first rule helps to define reasonable semantics for the implicit
> moving and copying of directories. On the downside, some changes are
> prohibited, like making a backup copy of a directory and then removing files
> from the original directory. This cannot be done in one go.

## Nested Directories

To further stretch the icon example, what if we wanted to use `Symbols` as our
base `icons` directory instead of creating a new one? We would just rename it
accordingly.

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    01      icons/menu
    02      icons/menu/close-file.svg
    03      FileIcons/open-file.svg
    04      icons
    05      Symbols/letter.svg
    06      Symbols/warning.svg

The former `Symbols` will be our `icons` directory now, containing the
`letter.svg` and `warning.svg` files as before. ViFi is certainly clever enough
to postpone copying the `actions` and `menu` directories until `icons` is in
place. Easy, right?

For the finishing touch we want to move the `letter.svg` into the `actions`
directory, named as `send-mail.svg`. Given that `actions` will be in the same
`icons` base directory afterwards, we are tempted to do the following:

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    01      icons/menu
    02      icons/menu/close-file.svg
    03      FileIcons/open-file.svg
    04      icons
    05      Symbols/actions/send-mail.svg
    06      Symbols/warning.svg

_This will not work!_

Here is the catch: As an _internal_ change of the `Symbols` directory, ViFi
will first create a new `actions` subdirectory and put `send-mail.svg` there.
But after renaming the `Symbols` directory to `icons`, this subdirectory will
be "overwritten" when copying the former `FileIcons` directory into the same
path, namely `icons/actions`. Thus we would lose the `send-mail.svg` file!

To do what we want we have to change the complete path for the `send-mail.svg`.

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    01      icons/menu
    02      icons/menu/close-file.svg
    03      FileIcons/open-file.svg
    04      icons
    05      icons/actions/send-mail.svg
    06      Symbols/warning.svg

This way, ViFi will move the original `letter.svg` out first, and only move it
back in as `send-mail.svg` after the target directory `actions` is copied. As
seen here, getting the order right can be tricky some times. Although in
practice these situations are not that common.

As a precaution for complicated and nested changes, my advice is to
* Check the operations that ViFi wants to carry out, especially the delete
  operations `[x] <---`.
* Do the changes in multiple steps. You can clearly define the order and you
  can see the complete result of the former changes.
* Write out all changes explicitly.

For our icon example making all changes explicit would mean:

    # ViFi@/tmp/Build/Icons
    01      icons/actions
    03      icons/actions/open-file.svg
    05      icons/actions/send-mail.svg
    01      icons/menu
    02      icons/menu/close-file.svg
    03      icons/menu/open-file.svg
    04      icons
    06      icons/warning.svg

The explicit changes tell ViFi to completely decompose the directory structure,
and puzzle it back together in the desired way. Apart from using more
filesystem operations (and more explicit writing), there are no detrimental
effects.


# Background Theory

The ViFi algorithm has to match lines in the changed text file to lines in the
original, identified by the hex id at the beginning of the line. File
operations are derived from this matching. In general, each line in the
original has zero (deleted), one (moved), or multiple (copied) corresponding
lines in the changed text file.

## Pivot

For each line in the changed file we define the pivot level as the directory
level where the changed line differs from the corresponding line in the
original file.
Given an original line of

    04	www/docs/public/index.html

we get the following pivot levels for changed lines

    04	www/docs/public/index.html      -> pivot MAX (unchanged)
    04	www/docs/public/index.php       -> pivot 4
    04	www/docs/index.html             -> pivot 3
    04	www/docs/private/index.html     -> pivot 3
    04	www/overview.html               -> pivot 2
    04	tmp/docs/public/index.html      -> pivot 1

Changes _internal_ to a directory are executed first, before any changes to the
directory itself or its parent directories can take place. By definition, the
pivot level is the level of the (parent) directory to which the path changes
count as _internal_. It determines a coarse order in which changes are applied,
starting with the higher pivot levels.

There is a special case where we have to deviate from the definition above.
Obviously we can only move an entry into a directory that already exists. Thus
for nested changes, the content of a newly created directory must have a pivot
level smaller or equal to its parent directories. Otherwise the pivot level
order would contradict the order of filesystem operations.

As an example when we change 

    01	www
    02	www/docs
    03	www/docs/index.html
    04	www/public
    05	www/public/about.html

to

    01	www                            -> pivot MAX (unchanged)
    02	www/docs                       -> pivot MAX (unchanged)
    04	www/docs/public                -> pivot 2
    05	www/docs/public/about.html     -> pivot 2
    03	www/docs/public/index.html     -> pivot 2

Notice that while `index.html` would be at pivot level 3 just from its path
changes, it inherits the lower pivot level 2 from the `public` directory. This
_pivot inheritance_ is transitive as it extends to subdirectories.

## Out and In

As stated before, ViFi translates the text changes into a sequence of
filesystem operations. The sequence is composed of the following operations:

* _Copy_ a file or directory _out_ to temporary space.
* _Move_ a file or directory _out_ to temporary space.
* _Delete_ a file or directory.
* _Copy_ a file or directory _in_ from temporary space.
* _Move_ a file or directory _in_ from temporary space.
* _Create_ a new directory.

Given the _internal first_ rule, ViFi naturally orders the filesystem
operations by directory level, from deep within the tree to the root level. For
each level, the operations are processed in different phases.

1. _Copy out_ items that stem from the current directory level.
2. _Move out_ or _delete_ items that stem from the current directory level.
3. _Copy_ or _move in_ items with pivot level matching the current directory
   level, and _create_ new directories. Order by target directory level.

At the end of processing the previous level, directories from the current level
are supposed to have all internal changes completed. Phase 1 and 2 are meant to
preserve their state in temporary space before phase 3 could potentially alter
them. Phase 2 also prevents any namespace conflicts in the filesystem with
items that are introduced in phase 3.

Phase 3 will then add all items that are missing to complete the current level.
Our definition of pivot levels implies that this includes only changes that
count as internal at the current level, external changes are left for later.
Since the items that are added may create directory structures of arbitrary
depth, we have to order them by increasing target directory level.

In brief, this algorithm processes the internal changes of each level, one
after another, from the innermost directory level down to the root. When the
root level is finished, we are done.

## Conclusion

ViFi has a sound theory and a practical algorithm according to the semantics
that we defined. What is debatable is the semantics and how it affects
usability. The most important decisions on semantics were

* Treat directories as objects including their content, for all operations.
* Changes internal to a directory are processed first.
* A directory is copied or moved with all its internal changes applied.

This choice has a number of advantages - we avoid ambiguities and problems with
recursion, changes are composable, and simple cases like renaming a directory
with internal changes can be done with very little effort. But it also imposes
some limitations on what changes can be expressed by altering the text file.

* Make a backup copy of a directory and change the original.
* Leave a file or directory in place but move away its parent directory.

A major reason for this is that we implicitly include the content of a
directory when it is moved or copied. We could require explicitly changing and
copying every path in the text file instead, but this would severely cripple
usability.


# Troubleshooting

"We apologise for the inconvenience" - did I already tell you to do regular
backups?

If you read this, most likely one of the following happened:

## ViFi was interrupted or unexpectedly stopped

Good news! Your files should be intact, unless there was a problem with the
underlying filesystem. What is missing from the original files can be found in
the temporary `.ViFi` directory. The file and directory names correspond to the
hex ids in the text files. You will also find the text files with ids and paths
there, so you should be able to identify and recover all your files.

> Beware that recursive copies of files and directories may be incomplete.

Regarding ViFi unexpectedly stopping, throwing errors, or crashing, please
consider to contact me at dev@submerge.ch and keep the text files for
reference.

## ViFi did not what you wanted it to do

ViFi finished processing your changes, but the outcome is different from what
you expected? Please double check the actions that ViFi asked you to confirm,
if that part is still accessible in your shell output. Unfortunately all
temporary files will be already deleted at this point. Also the text files will
be gone, so it is not possible to examine the changes you made.

> For an important file that was accidentally deleted, there is some chance it
> can be recovered from disk. Stop using the disk and search the internet for
> file recovery in combination with your filesystem.
> And have a backup ready the next time.

If you come to the conclusion that ViFi did not correctly follow the actions it
proposed, or interprets text file changes differently from what is described in
the manual, please contact me at dev@submerge.ch - preferably with some
evidence or an example.

