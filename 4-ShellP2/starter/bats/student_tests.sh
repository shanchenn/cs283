#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check pwd outputs the correct directory" {
    run ./dsh <<EOF
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == "$(pwd)"* ]]
}

@test "Check cd changes the directory" {
    mkdir -p testdir
    run ./dsh <<EOF
cd testdir
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"testdir"* ]]
    rmdir testdir
}

@test "Check if exit command works" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}
