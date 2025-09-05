#!/usr/bin/env bash
#
# install-udev-rules.sh
#
# Copy udev rules from repo into /etc/udev/rules.d/
# Must be run with root privileges (sudo)

set -euo pipefail

# Require root
if [[ "$EUID" -ne 0 ]]; then
  echo "Error: This script must be run with root privileges. Try 'sudo ./setup.sh'"
  exit 1
fi

# Path to rules file in your repo
SRC_RULES="udev/60-spacemouse.rules"

# Destination path
DST_RULES="/etc/udev/rules.d/60-spacemouse.rules"

if [[ ! -f "$SRC_RULES" ]]; then
  echo "Error: $SRC_RULES not found."
  exit 1
fi

echo -e "This script is going to install udev rules for the supported SpaceMouse devices to allow driver access without root, and to disable Linux generic mouse driver interference."
echo "Rules will be copied to $DST_RULES"
echo -e "\nDo you wish to continue? (Y/n) "
read -r response
if [[ "$response" =~ ^[Nn]$ ]]; then
  echo "Aborting."
  exit 0
fi

echo "Copying udev rules..."
cp "$SRC_RULES" "$DST_RULES"
chmod 644 "$DST_RULES"

echo "Reloading udev rules..."
udevadm control --reload-rules
udevadm trigger

echo "Done. Rules installed to $DST_RULES"
