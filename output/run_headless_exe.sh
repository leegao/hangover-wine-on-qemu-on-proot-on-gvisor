#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOTFS="${SCRIPT_DIR}/debian-arm64-rootfs"
PROOT="${SCRIPT_DIR}/proot-qemu/proot"
QEMU="${SCRIPT_DIR}/proot-qemu/qemu-aarch64-static"
PROOT_TMP="${SCRIPT_DIR}/proot_tmp"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <path-to-x86_64.exe> [args...]" >&2
    exit 1
fi

EXE_ARG="$1"
shift

mkdir -p "${PROOT_TMP}"
chmod 755 "${ROOTFS}/host-rootfs" "${ROOTFS}/etc/ld.so.preload" 2>/dev/null || true
mkdir -p "${ROOTFS}/host-rootfs"
touch "${ROOTFS}/etc/ld.so.preload"

if [[ "${EXE_ARG}" == "${ROOTFS}"/* ]]; then
    GUEST_EXE="${EXE_ARG#${ROOTFS}}"
elif [ -f "${EXE_ARG}" ]; then
    BASENAME="$(basename "${EXE_ARG}")"
    cp -f "${EXE_ARG}" "${ROOTFS}/tmp/${BASENAME}"
    GUEST_EXE="/tmp/${BASENAME}"
elif [ -f "${ROOTFS}${EXE_ARG}" ]; then
    GUEST_EXE="${EXE_ARG}"
else
    GUEST_EXE="${EXE_ARG}"
fi

pkill -9 wineserver 2>/dev/null || true

set +e
PROOT_NO_SECCOMP=1 PROOT_TMP_DIR="${PROOT_TMP}" \
    "${PROOT}" \
    -r "${ROOTFS}" \
    -q "${QEMU}" \
    -b /dev -b /proc -b /sys \
    -w /root \
    /usr/bin/env -i \
    HOME=/root \
    USER=root \
    PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
    WINEDEBUG="${WINEDEBUG:--all}" \
    /usr/bin/wine "${GUEST_EXE}" "$@"
RC=$?
set -e

pkill -9 wineserver 2>/dev/null || true
chmod 755 "${ROOTFS}/host-rootfs" "${ROOTFS}/etc/ld.so.preload" 2>/dev/null || true
exit ${RC}
