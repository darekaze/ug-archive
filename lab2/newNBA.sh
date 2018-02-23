# ! /bin/bash
# Initialize associative array
declare -A east
declare -A west
declare -A name
declare -A hwin
declare -A hlose
declare -A rwin
declare -A rlose
declare -A pct

# Read name and area
row=0
while read -r -a line; do
    name[${line[0]}]=${line[1]}
    if [ "${line[3]}" == "Eastern" ]; then
        east[${line[0]}]=0
    elif [ "${line[3]}" == "Western" ]; then
        west[${line[0]}]=0
    fi
    ((row++))
done < "$1"

# Initialize arrays to 0
for x in "${!name[@]}"; do
    rwin["${x}"]=0
    rlose["${x}"]=0
    hwin["${x}"]=0
    hlose["${x}"]=0
    pct["${x}"]=0
done

# Read all data and record win and lose
row=0
for f in $2*; do
    # echo "File -> $f"
    while read -r -a line; do
        if [ ${line[1]} -gt ${line[3]} ]; then
            (('rwin["${line[0]}"]'++))
            (('hlose["${line[2]}"]'++))
        elif [ ${line[1]} -lt ${line[3]} ]; then
            (('rlose["${line[0]}"]'++))
            (('hwin["${line[2]}"]'++))
        fi
    done < "$f"
done

# Calculate pct
for x in "${!pct[@]}"; do
    total=$(( rwin[$x]+hwin[$x]+rlose[$x]+hlose[$x] ))
    if [ $total -eq 0 ]; then
        continue
    else
        pct["${x}"]=$(( (rwin[$x]+hwin[$x])*1000 / $total ))
        # echo $x ' - ' ${pct[${x}]}
    fi
done

# Calculate net win
for x in "${!west[@]}"; do
    west[$x]=$(bc <<< "scale=1; (${rwin[$x]}+${hwin[$x]}-${rlose[$x]}-${hlose[$x]}) / 2")
done

for x in "${!east[@]}"; do
    east[$x]=$(bc <<< "scale=1; (${rwin[$x]}+${hwin[$x]}-${rlose[$x]}-${hlose[$x]}) / 2")
done

# Get max net-win of western conference
max=0
for n in "${!west[@]}"; do
    if (( $(echo "${west[$n]} > $max" | bc -l) )); then
        max=${west[$n]}
    fi
done

# store in temp file
for x in "${!west[@]}"; do
    win=$((rwin[$x]+hwin[$x]))
    lose=$((rlose[$x]+hlose[$x]))

    echo ${name[$x]} $win $lose \
        ${pct[$x]} $(bc <<< "scale=1; $max-(${west[$x]})") ${hwin[$x]}-${hlose[$x]} ${rwin[$x]}-${rlose[$x]} 
done | sort -n -k5 > .Wstat

# Get max net-win from eastern conference
max=0
for n in "${!east[@]}"; do
    if (( $(echo "${east[$n]} > $max" | bc -l) )); then
        max=${east[$n]}
    fi
done

# store in temp file
for x in "${!east[@]}"; do
    win=$((rwin[$x]+hwin[$x]))
    lose=$((rlose[$x]+hlose[$x]))

    echo ${name[$x]} $win $lose \
        ${pct[$x]} $(bc <<< "scale=1; $max-(${east[$x]})") ${hwin[$x]}-${hlose[$x]} ${rwin[$x]}-${rlose[$x]}
done | sort -n -k5 > .Estat

# Print out data with format
format="%-4s %-15s %-3s %-3s %-4s %-4s %-7s %-7s\n"

rank=1
printf "\nWestern Conference\n"
printf "$format" "Rank" "Team" "W" "L" "PCT" "GB" "HOME" "ROAD"
while read -r -a line; do 
    printf "$format" "$rank" "${line[0]}" "${line[1]}" "${line[2]}" \
    "${line[3]}" "${line[4]}" "${line[5]}" "${line[6]}"
    ((rank++))
done < .Wstat 
rank=1
printf "\nEastern Conference\n"
printf "$format" "Rank" "Team" "W" "L" "PCT" "GB" "HOME" "ROAD"
while read -r -a line; do 
    printf "$format" "$rank" "${line[0]}" "${line[1]}" "${line[2]}" \
    "${line[3]}" "${line[4]}" "${line[5]}" "${line[6]}"
    ((rank++))
done < .Estat

# Cleanup
rm -f .Estat
rm -f .Wstat