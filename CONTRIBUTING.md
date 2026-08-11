# Git Commits

## Semantic Commit Messages

Follow https://www.conventionalcommits.org/en/v1.0.0/

Some more requirements for commit body:
* describe made changes using phrases understandable by managers and acceptable in bug tracking
* if commit solve some trouble then commit message MUST contain note about problematic, choosen solution and reason for that.

Additional type(s):
* plan - task tracking activity

Add "Signed-off-by" trailer in commit message. Use option "-s" or "--signedoff" of "git commit" command.

## Sign Commit

Follow [GitHub commit signing documentation](https://docs.github.com/en/authentication/managing-commit-signature-verification/signing-commits) to configure that in your development environment.

Sign all your commits for current project.

| :blue_book: | Reason(s): |
|---|:---|
| | Try avoid "Software supply chain attack" for source code |

## Git Config

Possible variants to force Git commit signing by developers.

Local Git repository setting:
```
git config commit.gpgsign true
```

User git commit alias with signing and adding "Signed-off-by":

```
# part of ~/.gitconfig
...
[alias]
    ...
    ci = commit --signoff --gpg-sign
    ...
```

# Folder Structure

| File/Folder | Purpose |
|:---|:---|
| README.md | index file, contains short note, news and links to other interesting things in project |
| CONTRIBUTING.md | development process aggreements |
| TODO.md | task tracking |
| Makefile | build docs, unit tests, sketch and other artifacts |
| docs/\*.md | manuals |
| docs/\*.{png,jpg,jpeg} | pictures for manuals  |
| docs/\*.xcf | pictures sources (only for not flatten images) |
| proposals/\*.md | proposals |
| proposals/\*.{png,jpg,jpeg} | pictures for manuals  |
| proposals/\*.xcf | pictures sources (only for not flatten images) |
| src/ | source code |
| deps/ | dependencies needed for build |
| deps/\*.tar.zst | dependency libraries and SDKs |
| deps/patches/\*.patch | patches for dependencies to use |

# Documentation Style Guide

## Reasons

We are making decisions using our knowledge and context (available resources, time, requirements, etc.).

We are trying to make optimal decisions.

But everything is changing - requirements, resources and so on. And made things may become wrong and outdated.

If we know the reasons of made decisions then we'll be able to recognize bad parts of proposals and code and adapt them to updated context.

Make information about Reasons a part of documentation.

Example with single reason:

```
| :blue_book: | Reason(s): |
|---|:---|
| | Alone reason to do that |
```

Example with list of reasons to do something:
```
| :blue_book: | Reason(s): |
|---|:---|
| 1 | 1st reason to do that |
| 2 | 2nd reason to do that |
...
| N | Nth reason to do that |
```

## High Level Design (HLD)

High level proposals which does not refer to concrete libraries, frameworks, etc.

HLD SHOULD be as much abstract as possible. Use technologies instead of exist implementations.

For example, instead of MySQL, PostgreSQL, SQLite use SQL DB.

| :blue_book: | Reason(s): |
|---|:---|
| | Avoid mind locking Developers on single solution. Allow to find more optimal alternatives. |

HLD MUST be usable for making Low Level Design.

## Low Level Design (LLD)

Proposing HLD parts with all needed technical details.

It SHOULD be possible to split LLD into tasks and give them to Developers.

# Development Environment

## Dependencies

```
sudo apt install \
    git \
    git-lfs \
    # for Markdown to HTML rendering
    pandoc
```
