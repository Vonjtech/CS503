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

@test "Remote: Piped command (ls | grep dshlib.c)" {
    run ./dsh -c <<EOF
ls | grep dshlib.c
exit
EOF
    [ "$status" -eq 0 ]
    # Expect the output to contain "dshlib.c"
    [[ "$output" == *"dshlib.c"* ]]
}

@test "Remote: Piped command (echo hello world | tr \" \" \"\n\")" {
    run ./dsh -c <<EOF
echo hello world | tr " " "\n"
exit
EOF
    [ "$status" -eq 0 ]
    # Check that output contains both "hello" and "world"
    [[ "$output" == *"hello"* ]]
    [[ "$output" == *"world"* ]]
}

@test "Remote: Invalid command (invalidooo)" {
    run ./dsh -c <<EOF
invalidoooo
exit
EOF
    [ "$status" -eq 0 ]
    # Expect an error message; adjust the string as needed.
    [[ "$output" == *"error"* ]] || [[ "$output" == *"not found"* ]]
}

@test "Remote: Built-in cd command" {
    run ./dsh -c <<EOF
cd ..
ls
exit
EOF
    [ "$status" -eq 0 ]
    # Check that the output from ls reflects the directory change.
    [[ "$output" == *"starter"* ]] || [[ "$output" == *"Assignments"* ]]
}

@test "Remote: Built-in stop-server command" {
    run ./dsh -c <<EOF
stop-server
EOF
    [ "$status" -eq 0 ]
    # Expect the output to indicate the server is stopping.
    [[ "$output" == *"Server stopping"* ]]
}

@test "Remote: Multiple sequential commands" {
    run ./dsh -c <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
    # Verify that output is non-empty and contains a known file
    [ -n "$output" ]
}

@test "Remote: Empty input" {
    run ./dsh -c <<EOF
<ENTER>
exit
EOF
    [ "$status" -eq 0 ]
    # For empty input, output should be empty or just the prompt; adjust if needed
 trimmed=$(echo "$output" | tr -d '[:space:]') [ -z "$trimmed" ]
 }
