#!/bin/bash

# directory of script
scriptDir=$(cd "$(dirname "$0")"; pwd)

# default mode and user options
MODE=flash
USEROPTIONS=$@

# list available modes
if [ "$1" == "--listMode" ]; then
	echo 
	echo "Available Modes for UniFlash CLI:"
	echo "  * flash [default] - on-chip flash programming"
	echo "  * memory          - export memory to a file"
	echo "  * load            - simple loader (use default options)"
	echo "  * serial          - serial flash programming"
	echo
	
	exit 0
fi

# custom mode
if [ "$#" -ne 0 ]; then
	if [ $1 = "--mode" ]; then
		MODE=$2;
		USEROPTIONS=${@:3}
	fi
fi

# run the command line batch file
if [ "$#" -eq 0 ]; then
	args=(\"$scriptDir\"/ccs_base/DebugServer/bin/DSLite $MODE -c user_files/configs/tms570ls3137.ccxml -l user_files/settings/generated.ufsettings -s VerifyAfterProgramLoad=\"No verification\" )
	echo -e Executing: ${args[@]}
	eval ${args[@]}
else
	echo For more details and examples, please visit https://software-dl.ti.com/ccs/esd/uniflash/docs/latest_qsguide.html#command-line-interface
	echo
	eval "$scriptDir"/ccs_base/DebugServer/bin/DSLite $MODE $USEROPTIONS
fi

rc=$?
if [[ $rc != 0 ]]; then exit $rc; fi
