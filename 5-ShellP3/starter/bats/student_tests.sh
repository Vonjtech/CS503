#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Single command - ls runs without errors" {
  run ./dsh <<EOF
ls
exit
EOF
  [ "$status" -eq 0 ]
}


@test "Piped command - echo piped to grep" {
  run ./dsh <<EOF
echo hello | grep hello
exit
EOF
  # Check that the output contains "hello"
  [[ "$output" == *"hello"* ]]
  [ "$status" -eq 0 ]
}


@test "Multiple piped commands: echo split by tr and filtered by grep" {
  run ./dsh <<EOF
echo hello world | tr " " "\n" | grep world
exit
EOF
  [[ "$output" == *"world"* ]]
  [ "$status" -eq 0 ]
}


@test "Invalid command returns error" {
  run ./dsh <<EOF
foobar
exit
EOF
  # The output should contain an error message
  [[ "$output" == *"error"* ]]
  [ "$status" -eq 0 ]
}


@test "Extra whitespace in pipe commands is handled" {
  run ./dsh <<EOF
echo hello    |    grep hello
exit
EOF
  # Should still output hello despite extra spaces
  [[ "$output" == *"hello"* ]]
  [ "$status" -eq 0 ]
}


@test "Triple piped command: sorting pipeline" {
  run ./dsh <<EOF
echo c b a | tr " " "\n" | sort | head -n 1
exit
EOF
  # After sorting, the first line should be a
  [[ "$output" == *"a"* ]]
  [ "$status" -eq 0 ]
}


@test "Built-in exit command terminates the shell" {
  run ./dsh <<EOF
exit
EOF
  # Shell loop returns OK on exit
  [ "$status" -eq 0 ]
}