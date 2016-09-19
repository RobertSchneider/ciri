# read.sh <file-to-read> [name-of-voice]
#!/bin/bash

textToRead=$(cat $1)

IFS=$'\n'
while read rep; do
	        IFS=" "
		        repArray=( $rep )
			        textToRead=${textToRead//${repArray[0]}//${repArray[1]}}
			done < replacements.txt

			if [ -z $2 ]; then
				        echo "$textToRead" | say
				else
					        echo "$textToRead" | say -v $2
					fi
