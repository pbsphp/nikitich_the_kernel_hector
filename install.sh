#!/bin/sh

DEVICE_NAME="nikitich"


# Load module

echo "Loading module..."

insmod nikitich.ko || {
    echo "Fail!"
    exit 1
}


# Get device major number

echo "Getting major number..."

major=`sed -n "s;\([0-9]\+\) $DEVICE_NAME;\1;p" /proc/devices`

if [ -z "$major" ]; then
    echo "Cannot find _ in /proc/devices. Fail!"
    exit 1
fi


# Create /dev/DEVICE

echo "Creating /dev/$DEVICE_NAME..."

mknod /dev/$DEVICE_NAME c $major 0 || {
    echo "Cannot create /dev/$DEVICE_NAME. Fail!"
    exit 1
}


echo "Done!"
