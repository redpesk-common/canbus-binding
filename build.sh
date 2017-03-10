#!/bin/bash

function build {
	echo "ACTION: build"
	source ~/agl/sdk/porter/environment*
	if [ ! -d "$1/$2" ]; then
		echo "INFO: build dir ($1/$2) doesn't exist, created it!"
		mkdir -p "$1/$2"
	fi
	pushd "$1/$2"
	#cd "$1/$2"
	cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$3 $1
	make
	popd
}

function clean {
	echo "ACTION: clean"
	if [ -d "$1/$2" ]; then
		rm -vrf "$1/$2"
	fi
}

function rebuild {
	clean $1 $2
	build $1 $2 $3
}

function printhelp {
	echo "Usage: build.sh <action> <subdir> [config]"
	echo "	action: can be one of the following"
	echo "		build: build this project."
	echo "		rebuild: rebuild this project."
	echo "		clean: clean the previous build."
	echo "		install: install the build result."
	echo "	subdir: the subdir into which the build is done."
	echo "	config: can be Debug or Release. Ignored if the action is 'clean'."
}

function checkparams {
	if [ "$#" -ne "$(($1+1))" ]; then
		echo "ERROR: Wrong number of parameters, expected $1 but got $(($#-1))"
		printhelp
		exit 1
	fi
}

function main {
	CURRENT_DIR=$( dirname "$(readlink -f "$0")" )
	echo "Current script: $CURRENT_DIR"

	if [ "$#" -lt "1" ]; then
		echo "ERROR: At least <action> must be specified!"
		exit 1
	fi

	case "$1" in
		"build")
			checkparams 3 $*
			build $CURRENT_DIR $2 $3
			;;
		"rebuild")
			checkparams 3 $*
			rebuild $CURRENT_DIR $2 $3
			;;
		"clean")
			checkparams 2 $*
			clean $CURRENT_DIR $2
			;;
		"install")
			checkparams 3 $*
			echo "ERROR: Not implemented yet!"
			;;
		*)
			echo "ERROR: Unknown action '$3'!"
			exit 1 
			;;
	esac
}

main $*

