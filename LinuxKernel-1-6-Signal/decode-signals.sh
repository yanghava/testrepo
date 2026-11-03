#!/usr/bin/env bash

pid=${1:?Usage: $0 PID}
status="/proc/$pid/status"

[[ -r "$status" ]] || {
    printf 'Cannot read %s\n' "$status" >&2
    exit 1
}

decode_mask() {
    local hex=$1 digit value bit name
    local sig=1 found=0

    # Start at the rightmost hex digit: its lowest bit is signal 1.
    while [[ -n $hex ]]; do
        digit=${hex: -1}
        hex=${hex%?}
        value=$((16#$digit))

        for bit in 0 1 2 3; do
            if (( value & (1 << bit) )); then
                name=$(builtin kill -l "$sig" 2>/dev/null)
                printf ' %s(%d)' "${name:-UNKNOWN}" "$sig"
                found=1
            fi
            ((sig += 1))
        done
    done

    if (( ! found )); then
        printf ' (none)'
    fi
    printf '\n'
}

while read -r field mask rest; do
    case "$field" in
        SigCgt:|SigIgn:|SigBlk:|SigPnd:|ShdPnd:)
            printf '%-7s %s ->' "$field" "$mask"
            decode_mask "$mask"
            ;;
    esac
done < "$status"