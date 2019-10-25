#!/bin/bash

# Assumes script is run from ishlang/test_runner/
# Assumes make_test_files binary is in ishlang/test_runner/

scenarios=$(ls ../tests/*.test)
if [ "${scenarios}" == "" ]; then
    echo "Terminating tests ... no scenarios "
fi

for scenario in ${scenarios}
do
    ./make_test_files ${scenario}
    if [ ${?} -ne 0 ]; then
        echo "Terminating tests ... failed to create test files for ${scenario}"
        exit 1
    fi

    code_file=$(echo "${scenario}.code")
    expect_file=$(echo "${scenario}.expect")
    out_file=$(echo "${scenario}.out")

    ishlang -f ${code_file} > ${out_file}
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
