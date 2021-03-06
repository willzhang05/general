#!/bin/bash
# ===============================================================================
#	Author: Andrew Grimshaw
#	Date:   August 1, 2018
#	Arg 1 is the name of the SLURM file to translate
# ===============================================================================



# Steps
# 1	(SKIP FOR NOW) Check if they have credentials
#		If not, acquire them using a nonce methode
#			
# 2	Call toJSDL, get directory path
# 3	Create GFFS directory path
#		grid mkdir /tmp/CCC/institution/user_id/guid
# 4	If there is a copy in file
#		copy each stage-in file.
# 5 	Call qsub on /resources/CCC/queues/RegularQueue, save ticket
# 6	while (qstat on ticket returns not FINISHED and not ERROR) sleep 60 seconds
# 7 	if there is a copy out file
#		copy each stage out file
# 8	qcomplete ticket
# 9	clean out temp staging directory with rm -rf
# 10	exit 0


# NOTES
# Need to prepend and append stderr from the programs we call locally
# Would like to be able to handle user defined "SCRATCH" dirs


INST_SCRATH="/tmp/CCC/UVA"

function checkCredentials(){
	echo "entering checkCredentials - it is a NO-OP right now"
}

function callToJSDL()
{
	echo "entering callToJSDL $SLURM_FILE"
	JOBDIR=`$GENII_INSTALL_DIR/tojsdl $SLURM_FILE`

	if [[ $? -eq 1 ]] ; then
		echo "No infile specified. Please specify an infile."
		exit 1
	fi
	echo "JOBDIR is $JOBDIR"
}

function createDirPath() 
{
	echo "entering createDirPath"
#	check if it exists ... and basename's up to /tmp/CCC/instituion/person
	WHO=`whoami`
	MKPATH=$INST_SCRATH/$WHO/$JOBDIR
	echo "path is $MKPATH"
	fastgrid mkdir $MKPATH

}


function copyIn() {

	echo "entering copy in "
	#cat ${SLURM_BASE}.stagein
	cat ${SLURM_BASE}.stagein | while read line ;
		do
			echo $line
			fastgrid $line
		done
}

function copyOut() {

	echo "entering copy out "
	#cat ${SLURM_BASE}.stageout
	cat ${SLURM_BASE}.stageout | while read line ;
		do
			echo $line
			fastgrid $line
		done
}


function cleanUp() {

	echo "entering cleanUp"
	# Make more robust checking to prevent accidential deletion
	if [[ $MKPATH == "" || $MKPATH == "/" ]] ; then
		return
	fi
	fastgrid rm -r $MKPATH
	fastgrid qcomplete /resources/CCC/queues/RegularQueue $TICKET
}

function submitAndWait() {

	echo "entering submitandwait"
	RETSTR=`fastgrid qsub /resources/CCC/queues/RegularQueue local:${PWD}/${SLURM_BASE}.jsdl`
	if [[ $? -ne 0 ]] ; then
		echo "ERROR: Failed to submit job to /resources/CCC/queues/RegularQueue. Cleaning up and terminating."
		cleanUp
		exit 1;	
	fi
	TICKET=`echo $RETSTR|cut -b 34-69`
	#echo "RETSTR is $RETSTR"
	#echo "Ticket is $TICKET"
	while [ `fastgrid qstat /resources/CCC/queues/RegularQueue $TICKET | grep $TICKET | awk '{ print $8 }'` != "FINISHED" ]; 
	do
		echo "Still waiting"
		sleep 10
	done

}

if [[ "$SLURM_JOB_PARTITION" = "CCC" ]] ; then
	exit 0;
fi

export SLURM_JOB_DIR=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "WorkDir" | cut -d '=' -f 2- )
export SLURM_JOB_COMMAND=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "Command" | cut -d '=' -f 2- )
export SLURM_JOB_STDIN=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "StdIn" | cut -d '=' -f 2- )
export SLURM_JOB_STDERR=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "StdErr" | cut -d '=' -f 2- )
export SLURM_JOB_STDOUT=$( /usr/bin/scontrol show job $SLURM_JOB_ID | grep -i "StdOut" | cut -d '=' -f 2- )

SLURM_FILE=$SLURM_JOB_DIR/$SLURM_JOB_NAME 
SLURM_BASE=${SLURM_FILE%.*}
echo "slurm_base is $SLURM_BASE"
pushd $SLURM_JOB_DIR
callToJSDL $SLURM_FILE
createDirPath
copyIn
submitAndWait
copyOut
cleanUp
cp $SLURM_JOB_STDOUT ${SLURM_BASE}.stdout
cp $SLURM_JOB_STDERR ${SLURM_BASE}.stderr
popd
