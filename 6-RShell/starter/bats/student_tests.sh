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


# Location of the compiled dsh executable
DSH_EXEC="./dsh"

# Server settings
SERVER_IP="127.0.0.1"
SERVER_PORT="5678"

# Start the server in the background
setup() {
    # Kill any existing dsh server instances
    pkill -f "$DSH_EXEC -s" || true

    # Start the server in the background
    $DSH_EXEC -s -i 0.0.0.0 -p $SERVER_PORT > server_output.log 2>&1 &
    sleep 1  # Give the server some time to start
}

# Stop the server after tests
teardown() {
    pkill -f "$DSH_EXEC -s" || true
}

@test "Basic command execution" {
    run $DSH_EXEC -c -i $SERVER_IP -p $SERVER_PORT <<EOF
ls
exit
EOF
    echo "$output"
    [[ "$output" =~ "dsh4> exit" || "$output" =~ "client exited: getting next connection..." ]]
}



@test "Pipeline command execution" {
    run $DSH_EXEC -c -i $SERVER_IP -p $SERVER_PORT <<EOF
echo "hello world" | tr a-z A-Z
exit
EOF
    echo "$output"
    [[ "$output" =~ "HELLO WORLD" ]]
}

@test "Invalid command handling" {
    run $DSH_EXEC -c -i $SERVER_IP -p $SERVER_PORT <<EOF
invalidcmd
exit
EOF
    echo "$output"
    [[ "$output" =~ "execvp failed" ]]  # Expected failure message
}


@test "Multi-threaded support test" {
    # Start multi-threaded server
    pkill -f "$DSH_EXEC -s" || true
    $DSH_EXEC -s -i 0.0.0.0 -p $SERVER_PORT -x > server_output.log 2>&1 &
    sleep 1

    # Run two clients simultaneously
    $DSH_EXEC -c -i $SERVER_IP -p $SERVER_PORT <<EOF > client1_output.log 2>&1 &
ls
exit
EOF

    $DSH_EXEC -c -i $SERVER_IP -p $SERVER_PORT <<EOF > client2_output.log 2>&1 &
whoami
exit
EOF

    sleep 2  # Give clients time to execute

    # Validate both clients received expected output
    run cat client1_output.log
    echo "$output"
    [[ "$output" =~ "dsh4>" ]]

    run cat client2_output.log
    echo "$output"
    [[ "$output" =~ "dsh4>" ]]
    rm client1_output.log
    rm client2_output.log
    rm server_output.log
}
