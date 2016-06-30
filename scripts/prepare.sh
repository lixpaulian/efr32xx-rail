#! /bin/bash
set -euo pipefail
IFS=$'\n\t'

# Archives are to be downloaded from:
# http://www.silabs.com/Support%20Documents/RegisteredDocs/SiliconLabs-RAIL-1.0.0.0-GA.exe

RELEASE_VERSION="1.2.0.0"

FAMILY="EFM32FG FlexGecko"
GITHUB_PROJECT="lixpaulian/efr32fg-rail"
ARCHIVE_NAME="SiliconLabs-RAIL-${RELEASE_VERSION}-GA.exe"
# ARCHIVE_URL="http://www.silabs.com/Support%20Documents/RegisteredDocs/${ARCHIVE_NAME}"
ARCHIVE_LOCATION="/Volumes/C/SiliconLabs/SiliconLabsRAIL/${RELEASE_VERSION}-GA"

# we have only an .exe file that we must unpack using windows :-( hopefully Silicon Labs
# will provide also a zip or tar file sometimes...
# LOCAL_ARCHIVE_FILE="/tmp/xpacks/${ARCHIVE_NAME}"

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

# if [ ! -f "${LOCAL_ARCHIVE_FILE}" ]
# then
#  mkdir -p $(dirname ${LOCAL_ARCHIVE_FILE})
#  curl -o "${LOCAL_ARCHIVE_FILE}" -L "${ARCHIVE_URL}"
# fi

# echo "Manually unpack '${ARCHIVE_NAME}'..."
# unzip -q "${LOCAL_ARCHIVE_FILE}"

cp -R "${ARCHIVE_LOCATION}/"* .

echo "Saving the precompiled library..."
mkdir lib
cp autogen/librail_release/librail_efr32_gcc_release.a lib/librail_efr32.a

echo "Removing unnecessary files..."

rm -rf \
apps \
apps-bin \
autogen \
docs \
hal \
meta-inf \
stack.* \
*.dat \
*.exe \
*.html \
utils

echo "Creating README.md..."
cat <<EOF >README.md
# ${FAMILY} CMSIS

This project, available from [GitHub](https://github.com/${GITHUB_PROJECT}),
includes the RAIL precompiled library and all the header and source files
required to start writing applications for the EFR32FG Flex Gecko family.

## Version

* v${RELEASE_VERSION}

## Documentation

The latest RAIL documentation is available from Silicon Labs at
http://www.silabs.com/products/wireless/proprietary/Pages/getting-started-with-flex-gecko.aspx.


## Original files

The original files are available in the \`originals\` branch.

These files were extracted from \`${ARCHIVE_NAME}\`.

To save space, the following folders/files were removed:

* apps
* apps-bin
* docs
* hal
* meta-inf
* stack.*
* *.dat
* *.exe
* *.html
* utils

EOF

echo
echo Check if ok and when ready, issue: \`git commit -m ${ARCHIVE_NAME}\`


