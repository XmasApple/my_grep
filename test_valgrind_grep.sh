#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
RESULT=0
DIFF_RES=""

declare -a tests=(
"s test_0_grep.txt VAR"
"for main.c main.h Makefile VAR"
"for main.c VAR"
"-e for -e ^int main.c main.h Makefile VAR"
"-e for -e ^int main.c VAR"
"-e regex -e ^print main.c VAR -f test_ptrn_grep.txt"
"-e while -e void main.c Makefile VAR -f test_ptrn_grep.txt"
)


testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    valgrind -v -q --leak-check=full --log-file="test_my_grep.log" ./my_grep $t
    leak=$(grep -A100000 leaks test_my_grep.log)
    (( COUNTER++ ))
    if [[ $leak == *"no leaks are possible"* ]]
    then
      (( SUCCESS++ ))
        echo -e "\e[31m$FAIL\e[0m/\e[32m$SUCCESS\e[0m/$COUNTER \e[32msuccess\e[0m grep $t"
    else
      (( FAIL++ ))
        echo -e "\e[31m$FAIL\e[0m/\e[32m$SUCCESS\e[0m/$COUNTER \e[31mfail\e[0m grep $t"
#        echo "$leak"
    fi
    rm test_my_grep.log
}

# 1 параметр
for var1 in v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# 2 сдвоенных параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                testing $i
            done
        fi
    done
done

# 3 строенных параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        for var3 in v c l n h o
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1$var2$var3"
                    testing $i
                done
            fi
        done
    done
done

echo -e "\e[31mFAIL: $FAIL\e[0m"
echo -e "\e[32mSUCCESS: $SUCCESS\e[0m"
echo "ALL: $COUNTER"
RATE=$(( SUCCESS * 100 / COUNTER ))
echo "SUCCESS RATE: $RATE%"
