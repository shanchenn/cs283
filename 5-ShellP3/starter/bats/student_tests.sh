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

@test "Check single pipe: ls | wc -l" {
    run ./dsh <<EOF
ls | wc -l
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ -n "$output" ]
}

@test "Check multiple pipes: ls | grep dsh | wc -l" {
    run ./dsh <<EOF
ls | grep dsh | wc -l
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check built-in cd command" {
    run ./dsh <<EOF
cd /
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == "/"* ]]
}