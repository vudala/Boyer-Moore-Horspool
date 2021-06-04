END=20
for i in $(seq 1 $END);
do
    start=$(date +%s.%N)
    ./dna
    dur=$(echo "$(date +%s.%N) - $start" | bc)
    printf "%.6f\n" $dur
done