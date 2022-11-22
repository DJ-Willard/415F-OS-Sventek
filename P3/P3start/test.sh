#!  /bin/bash

TestNum=$1;

echo "====================================="
echo "Running tests on  ./psbv${TestNum}"
echo "-------------------------------------"

function expect_result() {
    # Args
    program=$1
    expect=$2
    echo "${program} ${expect}"
}

case $TestNum in
    1)  echo "Starting psbv1"
        ./psbv1&
        echo "-------------------------------------"
        echo "Running non-encrypted test"
        echo "-------------------------------------"
        ./echoclient < .psbrc
        echo "-------------------------------------"
        echo "Running encrypted test"
        echo "-------------------------------------"
        ./echoclient -e < .psbrc
        echo "-------------------------------------"
        echo "Cleanup"
        echo "-------------------------------------"
        kill $(pgrep psbv1)
        ;;

    *)
        echo "Invalid test number selected, select 1, 2, 3, or 4"
        ;;
esac