#!/bin/bash
FILE=$1
TMP_ASM=temp.s
TMP_EXEC=temp

if [[ $2 == "" ]]; then
	FILE="outp.ll_"
fi
../Debug/ProgLang ./inp > /dev/null
if [[ $1 == "opt" ]]; then
	BC=$FILE.opt.bc
	opt $FILE -O3 -o $BC
	FILE=$BC.ll_
	llvm-dis $BC -o $FILE
fi

llc $FILE -o $TMP_ASM
clang++ $TMP_ASM -o $TMP_EXEC
./$TMP_EXEC
echo Returned: $?
#rm $TMP_ASM $TMP_EXEC
