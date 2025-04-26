#!/bin/bash

verbose=0
ishlang=""
ishlang_test_files=""
tests=""

for arg in $@;
do
    if [ "${arg}" == "-v" ] || [ "${arg}" == "--verbose" ]; then
        verbose=1
    elif [ "${arg}" == "-d" ] || [ "${arg}" == "--dyld" ]; then
        export DYLD_LIBRARY_PATH="/Users/${USER}/bin"
    elif [ "${ishlang}" == "" ]; then
        ishlang=${arg}
    elif [ "${ishlang_test_files}" == "" ]; then
        ishlang_test_files=${arg}
    elif [ "${tests}" == "" ]; then
        tests=${arg}
    fi
done

if [ "${ishlang}" == "" ] || [ "${ishlang_test_files}" == "" ] || [ "${tests}" == "" ]; then
    echo "Usage"
    echo ""
    echo "    $0 [-v|--verbose] [-d|--dyld] <ishlang> <ishlang_test_files> <tests>"
    echo ""
    echo "            ishlang := ishlang binary"
    echo " ishlang_test_files := ishlang_test_files binary"
    echo "              tests := tests/scenarios folder"
    echo ""
    echo "Error: Invalid arguments"
    exit 1
fi

if [ ! -f ${ishlang} ]; then
    echo "Terminating tests ... cannot find ishlang binary ${ishlang}"
    exit 1
fi

if [ ! -f ${ishlang_test_files} ]; then
    echo "Terminating tests ... cannot find ishlang_test_files binary ${ishlang_test_files}"
    exit 1
fi

if [ ! -d ${tests} ]; then
    echo "Terminating tests ... cannot find tests folder ${tests}"
    exit 1
fi

scenarios=$(ls ${tests}/*.test)
if [ "${scenarios}" == "" ]; then
    echo "Terminating tests ... no scenarios "
fi

echo ""
echo "***** Running test scenarios"
echo ""

for scenario in ${scenarios}
do
    ${ishlang_test_files} ${scenario}
    if [ ${?} -ne 0 ]; then
        echo "Terminating tests ... failed to create test files for ${scenario}"
        exit 1
    fi

    code_file=$(echo "${scenario}.code")
    expect_file=$(echo "${scenario}.expect")
    out_file=$(echo "${scenario}.out")

    ${ishlang} -p ${tests} -f ${code_file} > ${out_file}
    if [ ${?} -ne 0 ]; then
        echo "Terminating tests ... failed running ${code_file}"
        exit 1
    fi

    diff_result=$(diff ${out_file} ${expect_file})
    if [ "${diff_result}" == "" ]; then
        if [ ${verbose} -eq 1 ]; then
            echo "${scenario}: OK"
        fi
    else
        echo "${scenario}: Failed"
        echo ""
        echo "${diff_result}"
        exit 1
    fi

    rm ${code_file} ${expect_file} ${out_file}
done

count=$(ls ${tests}/*.test | wc -l)
if [ ${verbose} -eq 1 ]; then
    echo ""
fi
echo "***** Ran ${count} test scenarios"
echo "***** Success"
