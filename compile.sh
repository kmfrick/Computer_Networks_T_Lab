#!/bin/bash
find -type f -name "*.java" -execdir javac {} \;
for i in `find -type f -name "*.c"`; do
	NAMENOEXT=$(echo ${i}  | rev | cut -d. -f2- | rev)
	gcc ${i} -o ${NAMENOEXT};
done

