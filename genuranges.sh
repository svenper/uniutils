#!/bin/sh

echo 'struct cr Range_Table []={'
sed -E -e '/^$/d' -e '/^\#/d' -e 's/; /,"/g' -e 's/$/"},/g' -e 's/\.\./,0x/g' -e 's/^/\{0x/g' "${1}"
echo '};'
