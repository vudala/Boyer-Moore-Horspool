export OMP_PROC_BIND=true
export OMP_DYNAMIC=false

END=30
testar () {
    export OMP_NUM_THREADS=$1
    SUM=0
    for i in $(seq 1 $END);
    do
        start=$(date +%s.%N)
        ./dna
        dur=$(echo "$(date +%s.%N) - $start" | bc)
        SUM=$(echo "$SUM + $dur" | bc)
    done
    echo -n "Average: "
    echo "scale=9; $SUM / $END" | bc
}

echo "1 core"
testar 1

echo "2 cores"
testar 2

echo "4 cores"
testar 4

echo "8 cores"
testar 8