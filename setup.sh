#!/usr/bin/env bash

set -euo pipefail

if [[ "$EUID" -ne 0 ]]; then
  echo "Error: This script must be run with root privileges. Try 'sudo ./setup.sh'"
  exit 1
fi

SRC_RULES="udev/60-spacemouse.rules"
DST_RULES="/etc/udev/rules.d/60-spacemouse.rules"
GROUP="hidraw"
USER="${SUDO_USER:-$USER}"

if [[ ! -f "$SRC_RULES" ]]; then
  echo "Error: $SRC_RULES not found."
  exit 1
fi

echo -e "This script is going to create $GROUP group, add user $USER to it and install udev rules."
echo -e "It will allow the driver to access SpaceMouse devices without root, and disable Linux generic mouse driver interference."
echo "Rules will be copied to $DST_RULES"
echo -e "\nDo you wish to continue? (Y/n) "
read -r response
if [[ "$response" =~ ^[Nn]$ ]]; then
  echo "Aborting."
  exit 0
fi

if ! getent group "$GROUP" > /dev/null 2>&1; then
  echo "Creating group $GROUP..."
  groupadd "$GROUP"
else
  echo "Group $GROUP already exists."
fi

if id -nG "$USER" | grep -qw "$GROUP"; then
  echo "User $USER is already in group $GROUP."
else
  echo "Adding user $USER to group $GROUP..."
  usermod -aG "$GROUP" "$USER"
fi

echo "Copying udev rules..."
cp "$SRC_RULES" "$DST_RULES"
chmod 644 "$DST_RULES"

echo "Reloading udev rules..."
udevadm control --reload-rules
udevadm trigger

echo "Done."
echo "You may need to log out and back in for group changes to take effect."
