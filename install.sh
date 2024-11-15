#!/bin/bash

# qt6-base-dev for QT core
# qt6-tools-dev for linguist packages
# qt6-svg-dev for SVG on Ubuntu
# libqt6svg6-dev for SVG on Pop!_OS

distribution=""
version=""
versionId=""
is_root=false

helpFunction() {
   echo "======================================================================"
   echo "gLabels automatic compiler, installer and uninstaller"
   echo
   echo "This script is currently working with:"
   echo " - Ubuntu 22.04 LTS"
   echo " - Pop!_OS 22.04 LTS"
   echo
   echo "Usage: $0 [-d | -b | -i | -u | -h ]"
   echo
   echo "\t-d (root)Install dependencies"
   echo "\t-b Build gLabels"
   echo "\t-i (root) Build and install gLabels"
   echo "\t-u (root) Uninstall gLabels"
   echo "\t-h Display this help menu"
   echo "======================================================================"
   exit 1
}

supportedCheck() {
   local distro_version="$1 $2"

   if [ "$distro_version" = "Pop!_OS 22.04" ] ||
      [ "$distro_version" = "Fedora Linux 41" ] ||
      [ "$distro_version" = "Ubuntu 22.04" ]; then
      return 0 # Supported
   else
      return 1 # Not supported
   fi
}

detect_package_manager() {
    if command -v apt >/dev/null 2>&1; then
        echo "apt"
    elif command -v dnf >/dev/null 2>&1; then
        echo "dnf"
    elif command -v yum >/dev/null 2>&1; then
        echo "yum"
    elif command -v pacman >/dev/null 2>&1; then
        echo "pacman"
    elif command -v zypper >/dev/null 2>&1; then
        echo "zypper"
    elif command -v apk >/dev/null 2>&1; then
        echo "apk"
    else
        echo "Unsupported package manager"
        return 1
    fi
}

installDependencies() {
   PACKAGE_MANAGER=$(detect_package_manager)
   echo "Using $PACKAGE_MANAGER"
   echo
   echo "======================================================================"
   echo "Installing cmake and g++..."
   echo "======================================================================"
   if [ "$distribution" = "Fedora Linux" ]; then
      dnf install cmake g++ -y
   else
      apt install cmake g++ -y
   fi
   echo
   echo "======================================================================"
   echo "Installing dependencies for $distribution using $PACKAGE_MANAGER..."
   echo "======================================================================"
   if [ "$distribution" = "Pop!_OS" ]; then
      sudo apt install qt6-base-dev qt6-tools-dev libqt6svg6-dev -y
   elif [ "$distribution" = "Fedora Linux" ]; then
      dnf install qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel -y
   elif [ "$distribution" = "Ubuntu" -a "$versionId" = "22.04" ]; then
      sudo apt install qt6-base-dev qt6-tools-dev libqt6svg6-dev
   else
      if [ "$PACKAGE_MANAGER" = "apt" ]; then
      apt install qt6-base-dev qt6-tools-dev qt6-svg-dev libqt6svg6-dev -y
      elif [ "$PACKAGE_MANAGER" = "dnf" ]; then
      dnf install qt6-qtbase-devel qt6-qtsvg-devel qt6-qttools-devel -y
      fi
   fi
   return 0
}

buildGlabels() {
   echo "======================================================================"
   echo "Building gLabels..."
   echo "======================================================================"
   echo
   rm -r build
   mkdir build
   cd build
   cmake ..
   make
}

installGlabels() {
   echo "======================================================================"
   echo "Installing gLabels..."
   echo "======================================================================"
      echo
      if [ -f "build/glabels/glabels-qt" ]; then
         echo "Installing gLabels..."
         cd build
         make install
      else
         echo "You need to build it first!"
      fi
}



if [ -f /etc/os-release ]; then
   . /etc/os-release

   distribution="$NAME"
   version="$VERSION"
   versionId="$VERSION_ID"
   echo
   echo "Running $distribution $version"
else
   echo "Could not determine the distribution, you are not officially supported"
   distribution="Unknown"
   version="Unknown"
   versionId="Unknown"
fi

if supportedCheck "$distribution" "$versionId"; then
   echo "Your distribution is supported"
   echo
fi

if [ $(id -u) -eq 0 ]; then
   is_root=true
fi

if [ "$#" -eq 0 ]; then
   helpFunction
   exit 1
fi

while getopts "adbiuh" opt; do
   case ${opt} in
   a)
      if [ "$is_root" = false ]; then
         echo "Don't run this as root!"
         exit 1
      fi
      echo "Please enter your root password to launch automatic install"
      sudo echo Thank you

      installDependencies
      buildGlabels
      cd ..
      chown -R "$USER":"$USER" /build
      installGlabels
      echo "Done!"
      ;;
   d)
      if [ "$is_root" = false ]; then
         echo "You need to run this as root to install dependencies"
         exit 1
      fi
      installDependencies
      echo "Done!"
      ;;
   b)
      if [ "$is_root" = true ]; then
         echo "Don't run this as root!"
         exit 1
      fi
      buildGlabels
      echo "Done!"
      ;;
   i)
      if [ "$is_root" = false ]; then
         echo "You need to run this as root to install gLabels"
         exit 1
      fi
      installGlabels
      echo "Done"
      ;;
   u)
      if [ "$is_root" = false ]; then
         echo "You need to run this as root to uninstall gLabels"
         exit 1
      fi
      echo "Uninstalling gLabels..."
      echo
      if [ -f "build/install_manifest.txt" ]; then
         echo "Install manifest found!"
         while IFS= read -r file; do
            if [ -e "$file" ]; then
               echo "Deleting $file"
               rm -r "$file"
            fi
         done <build/install_manifest.txt
      else
         echo "No build manifest found, the uninstaller will try it's best, for better result run this script install first"
         rm /usr/local/bin/glabel-qt
         rm /usr/local/share/applications/glabels-qt.desktop
         rm /usr/local/share/mime/packages/x-glabels-document.mime.xml
         rm /usr/local/share/appdata/glabels-qt.appdata.xml
         rm -rf /usr/local/share/glabels-qt
      fi
      echo "Done!"
      ;;
   h)
      helpFunction
      exit 0
      ;;
   *)
      helpFunction
      exit 1
      ;;
   esac
done
