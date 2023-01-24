lcm=1
while read -r n
do
	if [ "$n" == "" ];
	then
		continue
	fi
	x=$(echo $n | rev)
	a=$x
	b=$lcm
	while [ $b -ne 0 ]; do
		r=$((a % b))
		a=$b
		b=$r
	done
	lcm=$(((x / a) * lcm))
done < $1
echo $lcm