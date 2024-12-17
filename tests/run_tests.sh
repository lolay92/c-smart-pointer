mkdir -p build/tests
echo "Start running tests...."
echo "-----------------\n"
for i in build/tests/*_tests
do
	if test -f $i
    then
		if ./$i 2>> build/tests/tests.log
        then
			echo "$i PASS"
        else
			echo "ERROR in test $i. Find log here: build/tests/tests.log"
			echo "---------------------"
			tail build/tests/tests.log
			exit 1
		fi
	fi
echo "-----------------\n"
echo "ALL TESTS PASSED!\n"
done