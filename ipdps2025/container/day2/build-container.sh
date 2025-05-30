#!/usr/bin/env bash
set -euo pipefail
set -x

./download-sst.sh
docker build --squash -t sstbuilder/ipdps2025-day2 .
