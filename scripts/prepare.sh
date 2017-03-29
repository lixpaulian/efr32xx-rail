#! /bin/bash
set -euo pipefail
IFS=$'\n\t'

# Archives are to be copied from the Simplicity Studio installation directory:
# /Applications/Simplicity Studio/Contents/Eclipse/developer/stacks/flex

RELEASE_VERSION="1.0.1.0"

FAMILY="EFR32FG FlexGecko"
GITHUB_PROJECT="lixpaulian/efr32fg-rail"
ARCHIVE_NAME="SiliconLabs Simplicity Studio Installation"
ARCHIVE_LOCATION="/Applications/Simplicity Studio.app/Contents/Eclipse/developer/stacks/flex/v${RELEASE_VERSION}"


echo "Cleaning previous files..."
for f in *
do
  if [ "${f}" == "scripts" ]
  then
    :
  else
    rm -rf "${f}"
  fi
done

# Copy files from the Simplicity Studio installation

cp -R "${ARCHIVE_LOCATION}/"* .

echo "Saving the precompiled library..."
mkdir lib
cp protocol/flex_1.0/connect/plugins/libraries/libphy-rail-efr32xg1-rtos-library-gcc.a lib/librail_efr32.a

echo "Removing unnecessary files..."

rm -rf \
hardware \
meta \
protocol \
*.dat \
*.exe \
util

echo "Creating README.md..."
cat <<EOF >README.md

This project, available from [GitHub](https://github.com/${GITHUB_PROJECT}),
includes the RAIL precompiled library and all the header and source files
required to start writing applications for the EFR32FG Flex Gecko family.

## Version

* Flex Lib v${RELEASE_VERSION}

## Documentation

The latest RAIL documentation is available from Silicon Labs at
http://www.silabs.com/products/wireless/proprietary/Pages/getting-started-with-flex-gecko.aspx.


## Original files

The original files are available in the \`originals\` branch.

These files were extracted from the \`${ARCHIVE_NAME}\`.

To save space, the following folders/files were removed:

* hardware
* meta
* *.dat
* *.exe
* *.html
* utils

EOF

echo
echo Check if ok and when ready, issue: \`git commit -m ${ARCHIVE_NAME}\`


