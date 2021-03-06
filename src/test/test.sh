#!/bin/bash

encode() {
	A=$(echo -n $1 | ../b64 2> /dev/null)
	A_EXIT=$?

	B=$(echo -n $1 | /usr/bin/base64 2> /dev/null)
	B_EXIT=$?

	if [ $A_EXIT != 0 ] && [ $B_EXIT != 0 ]; then
		return 0;
	fi
	if [ $A_EXIT == 0 ] && [ $B_EXIT == 0 ]; then
		if [ "$A" == "$B" ]; then
			return 0;
		fi
	fi

	echo "encode: $1" >&2
	echo $A >&2
	echo $B >&2
	return 1
}

decode() {
	A=$(echo -n $1 | ../b64 -d 2> /dev/null)
	A_EXIT=$?

	B=$(echo -n $1 | /usr/bin/base64 -d 2> /dev/null)
	B_EXIT=$?

	if [ $A_EXIT != 0 ] && [ $B_EXIT != 0 ]; then
		return 0;
	fi
	if [ $A_EXIT == 0 ] && [ $B_EXIT == 0 ]; then
		if [ "$A" == "$B" ]; then
			return 0;
		fi
	fi

	echo "decode: $1" >&2
	echo $A >&2
	echo $B >&2
	return 1
}

if [ $# == 0 ]; then
	ROOT=".."
fi

# build applicatoin
make --quiet --directory=$ROOT all

# run tests on encoder
encode "h"
encode "he"
encode "hel"
encode "hell"
encode "hello"

# run tests on decoder
decode "^"
decode "aA=="
decode "aGU="
decode "aGVs"
decode "aGVsbA=="
decode "aGVsbG8="

# clean up
make --quiet --directory=$ROOT clean
