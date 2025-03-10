#!/bin/bash

PACKAGE_NAME="qtobd2scanner"

# creating the root directory for your package
mkdir -p $PACKAGE_NAME/DEBIAN

# creating the necessary subdirectories for binaries, libs, app and startup scripts
mkdir -p $PACKAGE_NAME/usr/bin
mkdir -p $PACKAGE_NAME/usr/lib/python3/dist-packages
mkdir -p $PACKAGE_NAME/etc/init.d
mkdir -p $PACKAGE_NAME/usr/share/applications

# Inform the user
echo "========================================[ $PACKAGE_NAME ] ======================================"
echo "Empty directory structure for .deb package '$PACKAGE_NAME' created successfully."
echo "Now, you can add your files to the following locations:"
echo
echo "  - Qt app binary:          $PACKAGE_NAME/usr/bin/"
echo "  - Python backend server:  $PACKAGE_NAME/usr/lib/python3/dist-packages/"
echo "  - Startup script:         $PACKAGE_NAME/etc/init.d/"
echo "  - Desktop entry:          $PACKAGE_NAME/usr/share/applications/"
echo
echo "After adding your files, you can proceed with creating the .deb package."
echo "================================================================================================"
