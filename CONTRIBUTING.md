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
