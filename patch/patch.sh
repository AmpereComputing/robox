#!/bin/bash
if [ "$BASH" != "/bin/bash" ] && [ "$BASH" != "/usr/bin/bash" ]
then
    bash "$0" "$@"
	exit $?
fi
#转码补丁
ex_patch=$(ls |grep android-7.1.1_.*.patch)
echo "*--patching ${ex_patch}"
patch -p1 < ${ex_patch}
# 获取补丁列表
patches=($(ls |grep "^[0-9]\{4\}.*.$"|sort -n))
for patch in ${patches[*]}
do
    echo "*--patching ${patch}"
    patch -p1 < ${patch}
    [ $? -ne 0 ] && error "patch ${patch} failed"
done
exit 0


