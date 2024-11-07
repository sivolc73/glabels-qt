#!/bin/bash


# qt6-base-dev for QT core
# qt6-tools-dev for linguist packages
# qt6-svg-dev for SVG on Ubuntu
# libqt6svg6-dev for SVG on Pop!_OS 



distribution=""
version=""
is_root=false

helpFunction() {
   echo "====================================================="
   echo "gLabels automatic compiler, installer and uninstaller"
   echo
   echo "This script is currently working with:"
   echo " - Ubuntu 22.04 LTS"
   echo " - Pop!_OS 22.04 LTS"
   echo
   echo "Usage: $0 [-d | -b | -i | -u | -h ]"
   echo
   echo "\t-d Install dependencies"
   echo "\t-b Build gLabels"
   echo "\t-i (root) Build and install gLabels"
   echo "\t-u (root) Uninstall gLabels"
   echo "\t-h Display this help menu"
   echo "====================================================="
   exit 1 # Exit script after printing help
}

supportedcheck() {
   # Combine distribution and version into a single string
   local distro_version="$1 $2"

   # Check if the distribution and version are in the supported list
   if [ "$distro_version" = "Pop!_OS 22.04 LTS" ] ||
      [ "$distro_version" = "Ubuntu 22.04 LTS" ]; then
      return 0 # Supported
   else
      return 1 # Not supported
   fi
}

if [ -f /etc/os-release ]; then
   # Source the os-release file to extract variables
   . /etc/os-release

   # Set the distribution and version variables
   distribution="$NAME"
   version="$VERSION"

   # Optionally, print them out
   echo "Running $distribution $version"
else
   echo "Could not determine the distribution, assuming Ubuntu 22.04 LTS."
   distribution="Ubuntu"
   version="22.04 LTS"
fi

if supportedcheck "$distribution" "$version"; then
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

while getopts "dbiuh" opt; do
   case ${opt} in
   d)
      if [ "$is_root" = false ]; then
         echo "You need to run this script as root to install dependencies"
         exit 1
      fi
      echo
      echo "====================================================="
      echo "Installing cmake and g++..."
      echo "====================================================="
      apt install cmake g++
      echo
      echo "====================================================="
      echo "Installing dependencies for $distribution..."
      echo "====================================================="
      if [ "$distribution" = "Pop!_OS" ]; then
      sudo apt install qt6-base-dev qt6-tools-dev libqt6svg6-dev
      else
      apt install qt6-base-dev qt6-tools-dev qt6-svg-dev
      fi

      ;;
   b)
      echo "Building gLabels..."
      echo
      rm -r build
      mkdir build
      cd build
      cmake ..
      make
      ;;
   i)
      if [ "$is_root" = false ]; then
         echo "You need to run this script as root to install gLabels"
         exit 1
      fi
      echo "Installing gLabels..."
      echo
      if [ -f "build/glabels/glabels-qt" ]; then
         echo "Installing gLabels..."
         cd build
         make install
      else
         echo "You need to build it first!"
      fi
      ;;
   u)
      if [ "$is_root" = false ]; then
         echo "You need to run this script as root to uninstall gLabels"
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
