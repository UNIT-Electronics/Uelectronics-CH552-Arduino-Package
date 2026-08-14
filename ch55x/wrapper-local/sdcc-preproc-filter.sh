#!/bin/sh
# sdcc-preproc-filter.sh
#
# Strips "-MMD" and "-MF <file>" from the arguments before invoking SDCC.
#
# Arduino CLI 1.3+ (bundled with Arduino IDE 2.3.x) always appends
# "-MMD -MF <sketch>.libsdetect.d" to recipe.preproc.macros so it can cache
# header discovery. That is safe for GCC, which only honors the *last*
# -MF on the command line. SDCC's preprocessor (sdcpp) instead derives a
# default dependency filename from "-o" the moment it sees -MMD, and since
# this recipe preprocesses to /dev/null (object_file used during library
# discovery), it tries to open /dev/null.d and aborts with:
#   sdcpp: fatal error: opening dependency file /dev/null.d: Permission denied
# before it ever reaches the later, valid -MF override.
#
# We do not need the generated dependency file here (this recipe only
# preprocesses the merged sketch to feed prototype generation), so drop
# every -MMD/-MF pair -- ours and whatever Arduino CLI appended -- and run
# SDCC normally. Written in POSIX sh (no arrays) so it also runs under the
# BusyBox ash used on Windows.
#
# Usage: sdcc-preproc-filter.sh <path-to-sdcc> <sdcc args...>

sdcc="$1"
shift

count=$#
i=0
while [ "$i" -lt "$count" ]; do
    arg="$1"
    shift
    i=$((i + 1))
    case "$arg" in
        -MMD)
            continue
            ;;
        -MF)
            # also drop the dependency filename that follows
            shift
            i=$((i + 1))
            continue
            ;;
    esac
    set -- "$@" "$arg"
done

exec "$sdcc" "$@"
