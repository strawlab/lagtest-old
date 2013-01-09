#!/bin/bash -x
set -e

# upload main site
boto-rsync --delete --grant public-read _site/ s3://lagtest.org/
