#!/bin/bash
declare -A countries
#check arguments
if [ $# -ne 3 ]
then
	echo "not the correct number of arguments"
  exit 3
fi

inputfile=$1
inputdir=$2
numfiles=$3

if [ ! -r $inputfile ]
then
  echo "inputFile does not exist"
  exit 3
fi

if [ $numfiles -lt 0 ]
then
  echo "number of files invalid"
  exit 3
fi

if [[ -d $inputdir ]]
then
    echo "directory already exists"
    exit 3
fi

mkdir $inputdir

i=0
while read -r id first last country age virus answer date _;
do
	i=$(($i+1))
	if [[ ! -d $inputdir/$country ]]
	then
	    mkdir $inputdir/$country
			j=1
			while [ $j -le $numfiles ]
			do
				file=$country-$j."txt"
				touch $inputdir/$country/$file
				j=$(($j+1))
			done
			countries[$country]=1
	fi

	file=$country-${countries[$country]}."txt"
	echo  -e $id $first $last $country $age $virus $answer $date >>$inputdir/$country/$file
	if [[ ${countries[$country]} -eq $numfiles ]]
	then
		countries[$country]=1
	else
		countries[$country]=$((${countries[$country]}+1))
	fi
done < $inputfile
