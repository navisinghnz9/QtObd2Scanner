#!/bin/bash

create_control_file() {
echo "[INFO] :: Creating DEBIAN/control file..."
  cat > "$PACKAGE_NAME/DEBIAN/control" <<EOL
Package: $PACKAGE_NAME
Version: 1.0.0
Architecture: amd64
Maintainer: Navi Singh <navisinghnz9@gmail.com>
Depends: python3, qt5-qmake, libqt5widgets5
Section: utils
Priority: optional
Description: A Qt C++ application to perform OBD2 operations and logging
EOL
  echo "[INFO] :: DEBIAN/control file created."
}

PACKAGE_NAME="qtobd2scanner"
QT_APP_BUILD_DIR="../build-QtObd2Scanner-Desktop-Debug"
RELEASE_DIR="../Release"


# creating the root directory for your package and
# subdirs for bins, libs, app and startup scripts
echo "[INFO] :: Creating package directory..."
mkdir -p $PACKAGE_NAME/DEBIAN
mkdir -p $PACKAGE_NAME/usr/bin
mkdir -p $PACKAGE_NAME/usr/lib/python3/dist-packages
mkdir -p $PACKAGE_NAME/etc/init.d
mkdir -p $PACKAGE_NAME/usr/share/applications

# Inform the user
echo "========================================[ $PACKAGE_NAME ] ======================================"
echo "Directory structure for .deb package '$PACKAGE_NAME' created successfully."
echo
echo "  - Qt app binary:          $PACKAGE_NAME/usr/bin/"
echo "  - Python backend server:  $PACKAGE_NAME/usr/lib/python3/dist-packages/"
echo "  - Startup script:         $PACKAGE_NAME/etc/init.d/"
echo "  - Desktop entry:          $PACKAGE_NAME/usr/share/applications/"
echo
echo "================================================================================================"

# generating the DEBIAN/control file
create_control_file

cp $QT_APP_BUILD_DIR/QtObd2Scanner $PACKAGE_NAME/usr/bin
dpkg-deb --build $PACKAGE_NAME $RELEASE_DIR
