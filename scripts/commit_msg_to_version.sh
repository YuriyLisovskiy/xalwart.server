#!/usr/bin/env bash

set -e

COMMIT_MSG=$(git log -1 --pretty=%B)

if [[ "${COMMIT_MSG}" == *"[MAJOR]"* ]]; then
	echo -n "major"
elif [[ "${COMMIT_MSG}" == *"[MINOR]"* ]]; then
	echo -n "minor"
else
	echo -n "patch"
fi
