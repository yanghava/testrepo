for i in {1..65}; do
    name=$(kill -l $i 2>/dev/null)
    echo "$i: $name"
done
