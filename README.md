_ViFi file management tool_

# Introduction
ViFi lets you manipulate file names and directory trees in your favourite text
editor. This is particularly useful for renaming and copying multiple files, or
changing directory structures, where you can apply the full range of your text
editor skills. ViFi is best explained by walking through an example.

## Starting ViFi
Let's say you get a zip file from a colleague with this years and last years
annual financial report. You unpack the zip file and have a look at the
resulting folder:

    <joe@work:~/Downloads> unzip -q Annual\ Report.zip
    <joe@work:~/Downloads> vifi Annual\ Report

The `vifi` script opens your favourite text editor (`$EDITOR`) with the folder
content presented in text form:

    # ViFi@/home/joe/Downloads/Annual Report
    01      Reports
    06      Reports/2017
    09      Reports/2017/Annual Statement.pdf
    07      Reports/2017/Budget.pdf
    08      Reports/2017/Marketing.pdf
    02      Reports/2018
    05      Reports/2018/Annual Statement.pdf
    03      Reports/2018/Budget.pdf
    04      Reports/2018/Marketing.pdf
    0a      cat.jpg

## Cleanup
You don't even want to know about that cat picture, so you politely get rid of
it by deleting its line in the text file. Also you want to move all documents
out of the year folders, but prefix their file names with the year. You do so
by adjusting the documents paths in the text file and remove the lines with the
now obsolete year folders:

    # ViFi@/home/joe/Downloads/Annual Report
    01      Reports
    09      Reports/2017 Annual Statement.pdf
    07      Reports/2017 Budget.pdf
    08      Reports/2017 Marketing.pdf
    05      Reports/2018 Annual Statement.pdf
    03      Reports/2018 Budget.pdf
    04      Reports/2018 Marketing.pdf

## Splitting a Directory
Finally you decide to split the reports into accounting and marketing base
folders. You rename the `Reports` folder to `Accounting`, but change the base
folder in the path of the marketing documents to `Marketing`:

    # ViFi@/home/joe/Downloads/Annual Report
    01      Accounting
    09      Reports/2017 Annual Statement.pdf
    07      Reports/2017 Budget.pdf
    08      Marketing/2017 Marketing.pdf
    05      Reports/2018 Annual Statement.pdf
    03      Reports/2018 Budget.pdf
    04      Marketing/2018 Marketing.pdf

## Applying the Changes
Now you save the changes in the text file and close the editor. ViFi presents
you with the actions it would take to realize the changes in the text file:

    ".ViFi/03" <--- "Reports/2018/Budget.pdf"
    ".ViFi/04" <--- "Reports/2018/Marketing.pdf"
    ".ViFi/05" <--- "Reports/2018/Annual Statement.pdf"
    ".ViFi/07" <--- "Reports/2017/Budget.pdf"
    ".ViFi/08" <--- "Reports/2017/Marketing.pdf"
    ".ViFi/09" <--- "Reports/2017/Annual Statement.pdf"
    [x] <--- "Reports/2018"
    [x] <--- "Reports/2017"
    ".ViFi/03" ---> "Reports/2018 Budget.pdf"
    ".ViFi/05" ---> "Reports/2018 Annual Statement.pdf"
    ".ViFi/07" ---> "Reports/2017 Budget.pdf"
    ".ViFi/09" ---> "Reports/2017 Annual Statement.pdf"
    ".ViFi/01" <--- "Reports"
    [x] <--- "cat.jpg"
    [*] ---> "Marketing"
    ".ViFi/01" ---> "Accounting"
    ".ViFi/04" ---> "Marketing/2018 Marketing.pdf"
    ".ViFi/08" ---> "Marketing/2017 Marketing.pdf"
    Do you want to execute operations? [y|n]

In short it would
1. Move out all documents into a temporary directory.
2. Delete the now empty year folders.
3. Move the accounting documents back into `Reports`, with new filenames.
4. Delete the cat picture.
5. Rename the now complete `Reports` folder to `Accounting` (via temporary).
6. Create a new `Marketing` folder containing the marketing documents.

Since this is exactly your intent, you acknowledge by typing `y`, enter.

You may want to check the result by starting `vifi` again:

    # ViFi@/home/joe/Downloads/Annual Report
    01      Accounting
    05      Accounting/2017 Annual Statement.pdf
    04      Accounting/2017 Budget.pdf
    02      Accounting/2018 Annual Statement.pdf
    03      Accounting/2018 Budget.pdf
    06      Marketing
    07      Marketing/2017 Marketing.pdf
    08      Marketing/2018 Marketing.pdf

# Usage

## Guidelines

These are some quick guidelines to get you started. Some remarks first:
* You can only work on a single directory (for now).
* Try to understand the actions ViFi asks you to acknowledge.
  You can always abort instead if in doubt.
  * `<---` is moving entries out to a temporary location, `--->` in again.
  * `<===` is copying entries out to temporary location, `===>` in again.
  * `[x]` means deleting an entry.
  * `[*]` means creating a new directory.
* Experiment with files that you can recover from repos, snapshots or backups.

When editing the text file, in most cases it is best to treat entries as
complete lines. The hex id at the beginning of each line identifies the original
file for ViFi. Don't change it.
* Renaming or moving a file is changing its path in the line.
* Copying a file is copying its line and adjusting the new path.
* Deleting a file is deleting its line from the text file.
* Intermediate directories are created as needed when moving or copying files.

Be careful to preserve the hex id and the tab that separates it from the
filesystem path. All other characters are taken as is for the filesystem path,
including spaces, no escaping needed. Except for the first line (the ViFi
header), the order of the entries does not matter.

Directories are seen as entities including all their content. They can be 
manipulated like files in ViFi, copies and deletions are made recursively.
Although possible, it is not necessary to copy or delete all lines of their
content in the text file.

As a general rule all changes internal to a directory are processed before
copying or moving the directory. This allows to do nested changes in one go.

For more information please consult the [user manual](MANUAL.md).

## Recommended Use Cases

_The Good_ (Try it!)
* Working in a shell, using shell based text editors?
* Need a quick overview of a directory, with the ability to make changes?
* Want to change the structure of a directory?
* Repetitive filename changes that would be easy in a text editor?

Then you should definitely consider ViFi.

_The Bad_ (Counterindications)
* Want extended file informations, like file sizes or previews?
* Need interactive changes with immediate (externally visible) effect?

Use something else then.

_The Ugly_ (Inconvenient but possible)
* Exchange files between two distinctively separate directories?
  You probably don't want to run ViFi on a huge parent directory like `/usr`.
* Make changes involving a mounted subdirectory?
  This may cause unnecessary file copying.
* Copy operations are limited by text representation, e.g. make a backup copy
  of a directory and then change the original cannot be done in one go.

I did warn you.

# About

## Characteristics

_Selling points_ (Relax, it's [free and open source](COPYRIGHT.md))
* Works with any editor that can be invoked from the shell.
* Use a full featured text editor, search and replace, regexp, number sequences.
* Lightweight and fast (C++ and Boost).
* Versatile in changing and creating directory structure.

_Limitations_ (Nobody is perfect)
* Only works in a single directory (for now).
* Some filesystem operations are cumbersome in text form.

## Further Information

Still interested? Feel free to contact me at dev@submerge.ch if something
is not covered in the following documents.
* [Installation](INSTALL.md) - Build instructions and platform compatibility.
* [Manual](MANUAL.md) - Complete manual with syntax definitions and semantics.
* [Tasks](TASKS.md) - Open tasks and ideas for improvements.
* [Copyright](COPYRIGHT.md) - Copyright notice (ISC license).

If the links don't work, you can find the markdown files in the root of the
repository.
