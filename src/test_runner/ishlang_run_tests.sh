#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage"
    echo ""
    echo "    $0 <ishlang> <ishlang_test_files> <tests>"
    echo ""
    echo "            ishlang := ishlang binary"
    echo " ishlang_test_files := ishlang_test_files binary"
    echo "              tests := tests/scenarios folder"
    echo ""
    echo "Error: Invalid arguments"
    exit 1
fi

ishlang=${1}
ishlang_test_files=${2}
tests=${3}

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
        echo "${scenario}: OK"
    else
        echo "${scenario}: Failed"
        echo ""
        echo "${diff_result}"
        exit 1
    fi

    rm ${code_file} ${expect_file} ${out_file}
done
