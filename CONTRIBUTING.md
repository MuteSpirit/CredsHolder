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

| :blue_book: | Reason: |
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
| :blue_book: | Reason: |
|---|:---|
| | Alone reason to do that |
```

Example with list of reasons to do something:
```
| :blue_book: | Reasons: |
|---|:---|
| 1 | 1st reason to do that |
| 2 | 2nd reason to do that |
...
| N | Nth reason to do that |
```

## Schemas

Use [PlantUML](https://www.plantuml.org) to generate pictures with schemas. It's enough powerful for most of purposes.

| :blue_book: | Reasons: |
|---|:---|
| 1 | PlantUML files are text one. They may be added under source control |
| 2 | Text files maintaining is much easier then graph ones |

PlantUML files (\*.puml) MUST be rendered into PNG/JPEG with Makefile target.

| :blue_book: | Reason: |
|---|:---|
| | In most cases only simple way will be used by Developers during long development period. |

For one time usage pictures you may use any paint application.

## High Level Design (HLD)

High level proposals which does not refer to concrete libraries, frameworks, etc.

HLD SHOULD be as much abstract as possible. 

Use technologies instead of exist implementations. For example, instead of MySQL, PostgreSQL, SQLite use SQL DB.

| :blue_book: | Reason: |
|---|:---|
| | Avoid Designer/Developer mind lock on single known solution. Allow to find more optimal alternatives. |

HLD MUST be usable for making Low Level Design.

Approximate document structure is

| Section | Purpose |
|---|---|
| Abbreviations | <br/>Abbreviations and Terms used in this document |
| Requirements | <br/>User Stories, requirements, history context<br/>and another details needed to understand what we're designing.<br/>Try to figure out Actors to know requiremnt groups. That will help to formulate non-fragile requirements. |
| Open Questions | <br/>List of questions to clarify the details of requirements and etc.<br/>Move answered questions into separate section instead of erase them.<br/>Keep both Questions and Answers in text.<br/> |
| Entities | <br/>To be able to use [TRIZ](https://en.wikipedia.org/wiki/TRIZ) principal on invention we need to know elements of system, it's subsystem and suprasystem. |
| Proposals | <br/>Ideas how requirements can be met.<br/>Add advantages and disadvantages for each variant to be able compare them |
| Rejected Proposals | <br/>During designing you can have ideas good for a first glance but rejected after deeper analysis.<br/>Let list them too to avoid repeating the history and lost time for the same wrong ideas by reviewers and other designers.<br/>Please add the reason of idea rejection. |

Always add picture/schema for each proposal variant describing the core of idea. Without that you'll lost reviewers attention after couple of paragraphs.

During the years in IT we collect ready solutions and tricks in our mind and harry to use them in designs. But in many case the optimal solution is located in knowledge area unknown for us.

Usage of "TRIZ Ideal Result" formulation, "TRIZ System Operator" and "TRIZ Principals to Resolve Contradictions" will allow you cross borders of your mind. ! Without any drugs !

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
