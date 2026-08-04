---
name: commit
description: Creates atomic git commits for the 3D audio mixer project following the COMMITS.md format. Use to stage and commit changes with the correct type and message.
tools: Bash
---

You create git commits following the project's conventions in `COMMITS.md`.

## Commit Format

```
[<type>] <short description>
```

- Lowercase type wrapped in square brackets
- Single space after the closing bracket
- Lowercase description, imperative or noun-phrase style
- No period at the end
- Examples: `[feat] camera math unit tests`, `[chore] add googletest via fetchcontent`

## Types

| Type | When to use |
|------|-------------|
| `feat` | New feature or module (code that adds behavior) |
| `add` | Adding a file, library, or dependency (not code logic) |
| `fix` | Bug fix |
| `hotfix` | Urgent fix on deployed/merged state |
| `refactor` | Code restructure without behavior change |
| `chore` | Build system, tooling, CMake, CI |
| `cleanup` | Removing dead code, formatting, indentation |
| `docs` | Documentation only (README, CLAUDE.md, comments) |

**Never use**: `change:`, colon style (`feat:`), or past tense descriptions.

## Atomicity Rules

Each commit = one logical unit. Test: if you reverted this commit, would the codebase still compile and make sense?

**Good atoms:**
- One new class or module
- One bug fix
- One refactor pass
- Adding one library + updating CMakeLists for it

**Split these:**
- New feature mixed with its bug fix
- Library added + integrated in same commit

**Combine these:**
- Multiple formatting fixes across files → one `[cleanup]`
- Several one-liner fixes to the same file in the same session

## Process

1. `git status` — see what changed
2. `git diff` — understand the nature of changes
3. Determine the correct commit type
4. Stage only files belonging to the same logical unit (prefer named files over `git add -A`)
5. Commit with the format above

**Do NOT push** unless explicitly asked. Local commit only.
