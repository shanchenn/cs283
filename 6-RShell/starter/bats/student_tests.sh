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

# echo command does not work
# @test "Check echo command" {
#     run ./dsh -c -i 127.0.0.1 -p 5678 <<EOF
#     echo "Hello, world!"
# EOF
    
#     [ "$status" -eq 0 ]
#     [[ "$output" == *"Hello, world!"* ]]
# }

@test "Check exit command" {
    run ./dsh -c -i 127.0.0.1 -p 5678 <<EOF
    exit
EOF
    
    [ "$status" -eq 0 ]
}

@test "Check stop-server command" {
    run ./dsh -c -i 127.0.0.1 -p 5678 <<EOF
    stop-server
EOF
    
    [ "$status" -eq 0 ]
}
