#!/bin/sh
# Create example directory structure.
mkdir Icons
mkdir Icons/Symbols
touch Icons/Symbols/letter.svg
touch Icons/Symbols/warning.svg
mkdir Icons/FileIcons
touch Icons/FileIcons/close-file-08.svg
touch Icons/FileIcons/open-file-03.svg
# Print user info.
find Icons
echo "Done!"

