#!/usr/bin/env bash
# UserPromptSubmit hook: appends every prompt sent to Claude Code to PROMPTS.md.
# Hook input arrives as JSON on stdin; see https://docs.claude.com/en/docs/claude-code/hooks
set -euo pipefail

log="${CLAUDE_PROJECT_DIR:-.}/PROMPTS.md"
# Strip context tags the IDE integration injects (e.g. <ide_opened_file>).
prompt="$(jq -r '.prompt // empty' | perl -0pe 's{<(ide_\w+)>.*?</\1>\s*}{}gs')"
[ -n "$prompt" ] || exit 0

author="$(git -C "${CLAUDE_PROJECT_DIR:-.}" config user.name 2>/dev/null || echo unknown)"

{
  printf '\n## %s · %s\n\n' "$(date '+%Y-%m-%d %H:%M')" "$author"
  printf '%s\n' "$prompt" | sed 's/^/> /'
} >> "$log"
