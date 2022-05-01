#!/bin/bash
virusFile=$1
countriesFile=$2
numLines=$3
duplicates=$4
countries=()
viruses=()
ids=()
RANGE=9999
FLOOR=2
chars=ABCDEFGHIJKLMOPQRSTUVWXYZ
declare -A records

if [ ! -r $virusFile ]
then
  echo "virusFile does not exist"
  exit 3
fi

if [ ! -r $countriesFile ]
then
  echo "countiesFile does not exist"
  exit 3
fi

if [ $numLines -lt 0 ]
then
  echo "number of lines invalid"
  exit 3
fi

if [[ $duplicates -eq 0 ]] && [[ $numLines -gt 9999 ]]
then
  duplicates=2
  echo "cannot create more than 10000 records without duplicates"
fi
vlines=0
while read line; #read virus file
do
  vlines=$(($vlines+1))
  viruses+=($line)
done < $virusFile

clines=0
while read line;#read countries file
do
  clines=$(($clines+1))
  countries+=($line)
done < $countriesFile

lines=$numLines
if [[ $duplicates -ne 0 ]] #how many will be duplicates
then
  numLines=$(($numLines-($numLines/5)))
fi

i=0
touch inputfile.txt
>inputfile.txt

while [ $i -lt "$numLines" ]
do
  number=$RANDOM
  new=$(($number%$RANGE))
  flag=0

  for j in ${ids[@]};
  do
    if [ "$new" -eq "$j" ] #check if a same id already exists
    then
      flag=1
    fi
  done

  if [ "$flag" -eq 0 ] #if a same id does not exist
  then
    ids+=($new) #add new id to the array
    i=$(($i+1))
    number=$RANDOM
    first=$(($number%12))  #how many characters the first name has
    while [ "$first" -le $FLOOR ]
    do
      number=$RANDOM
      first=$(($number%12))
    done
    number=$RANDOM
    last=$(($number%12)) #how many characters the last name has
    while [ "$last" -le $FLOOR ]
    do
      number=$RANDOM
      last=$(($number%12))
    done
    number=$RANDOM
    c=$(($number%$clines)) #choose country
    number=$RANDOM
    age=$(($number%120)) #choose age
    while [ "$age" -le 0 ]
    do
      number=$RANDOM
      age=$(($number%120))
    done
    number=$RANDOM
    v=$(($number%$vlines)) #choose virus

    #firstname=$(cat /dev/urandom | tr -dc 'A-Z' | fold -w $first | head -n 1)
    #lastname=$(cat /dev/urandom | tr -dc 'A-Z' | fold -w $last | head -n 1)

    firstname="${chars:RANDOM%${#chars}:$first}"
    lastname="${chars:RANDOM%${#chars}:$last}"

    echo  -n $new $firstname $lastname ${countries[$c]} $age ${viruses[$v]} >> inputfile.txt
    records[$i]="$new $firstname $lastname ${countries[$c]} $age"

    number=$RANDOM
    a=$(($number%2))
    if [ "$a" -eq 1 ] #choose answer
    then
      number=$RANDOM
      day=$(($number%31)) #choose day of date
      while [ "$day" -le 0 ]
      do
        number=$RANDOM
        day=$(($number%31))
      done
      number=$RANDOM
      month=$(($number%13)) #choose month of date
      while [ "$month" -le 0 ]
      do
        number=$RANDOM
        month=$(($number%13))
      done
      number=$RANDOM
      year=$(($number%2022)) #choose year of date
      while [ "$year" -le 1940 ]
      do
        number=$RANDOM
        year=$(($number%2022))
      done
      echo " YES" $day"-"$month"-"$year>>inputfile.txt
    else
      echo " NO">>inputfile.txt
    fi
  fi

done


if [[ "$lines" -ne "$numLines" ]] #make the duplicates
then
  n=$(($lines-$numLines))
  j=0
  while [ $j -lt "$n" ]
  do
    number=$RANDOM
    r=$(($number%$numLines))
    echo -n ${records[$r]} >>inputfile.txt #choose a record to duplicate
    j=$(($j+1))
    number=$RANDOM
    v=$(($number%$vlines)) #choose virus
    number=$RANDOM
    a=$(($number%2))
    if [ "$a" -eq 1 ] #choose answer
    then
      number=$RANDOM
      day=$(($number%31)) #choose day of date
      while [ "$day" -le 0 ]
      do
        number=$RANDOM
        day=$(($number%31))
      done
      number=$RANDOM
      month=$(($number%13)) #choose month of date
      while [ "$month" -le 0 ]
      do
        number=$RANDOM
        month=$(($number%13))
      done
      number=$RANDOM
      year=$(($number%2022)) #choose year of date
      while [ "$year" -le 1940 ]
      do
        number=$RANDOM
        year=$(($number%2022))
      done
      echo " "${viruses[$v]} "YES" $day"-"$month"-"$year>>inputfile.txt
    else
      echo  " "${viruses[$v]} "NO">>inputfile.txt
    fi
  done
fi
