Can you think of why we use fork/execvp instead of just calling execvp directly? What value do you think the fork provides?

Answer:  In order to keep the parent shell process running while the command runs in the child process, we use fork() before execvp() to establish a new process.



It would be impossible to return control to the shell after command execution if we ran execvp() directly because it would replace the shell process itself.
When using fork(), the parent process waits for the child process to finish while the child process carries out the command. By doing this, a responsive shell that can process several commands in succession is maintained.


What happens if the fork() system call fails? How does your implementation handle this scenario?

Answer:  The system is unable to start a new process because of resource exhaustion . if fork() fails.



In my implementation, I use perror("Fork failed") to output an error message and return ERR_EXEC_CMD to signal failure if fork() returns -1.
pid_t pid = fork();
if (pid < 0) {
perror("Fork failed");
return ERR_EXEC_CMD;
}


How does execvp() find the command to execute? What system environment variable plays a role in this process?

Answer:  In the directories specified in the PATH environment variable, execvp() looks for the command.



execvp() searches /bin, /usr/bin, and other directories listed in PATH when a command such as ls is entered.
To verify the PATH variable, use:
echo $PATH
execvp() gives an error  if it is unable to locate the command in PATH.


What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

Answer:  The parent process pauses execution until the child process is finished by using wait().



Without wait(), the shell wouldn't wait for the last command to finish before prompting the user for the next one.
Child processes turn into zombies until the system cleans them up if wait() is not called.
Usage example:
int status;
waitpid(pid, &status, 0);
Process table overflow may result from background processes building up.
When executing commands, the shell may not act in a sequential manner.


In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

Answer:  The exit code of a terminated child process is extracted using WEXITSTATUS(status).



This is helpful for determining whether a command was successful or unsuccessful.
A command that fails, for instance, can return 127 (command not found) or another error code.
Enables script error handling (echo $? in Bash).
Aids shells in deciding whether to handle failures, log, or try again.


Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

Answer:  My implementation correctly preserves spaces inside quotes while treating quoted text as a single argument. For example, if the user inputs echo "hello world", it should be parsed as cmd->argv = ["echo", "hello world"], rather than splitting "hello" and "world" into separate arguments. This handling is necessary because, without it, commands like echo "hello world" would incorrectly interpret "hello" and "world" as distinct arguments, leading to unexpected behavior. Ensuring proper parsing of quoted strings allows commands to function as they would in a real shell, maintaining accuracy and consistency in command execution.



What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

Answer:  In this assignment, I made several significant changes to the parsing logic compared to the previous one. One of the major modifications was switching from a command list (command_list_t) to a single cmd_buff_t, as this assignment only requires handling a single command at a time without support for multiple commands separated by pipes. Additionally, I improved the handling of quoted strings to ensure that spaces inside quotes are preserved while still correctly parsing arguments. To achieve this, I refactored the build_cmd_buff() function, enhancing its ability to detect and process quoted input properly instead of simply splitting on spaces.



During the implementation, I encountered some challenges, particularly in ensuring that quotes were removed while maintaining spaces within quoted text. Another difficulty was modifying exec_built_in_cmd() to correctly handle different cases, ensuring that built-in commands functioned as expected. These refinements ultimately improved the overall robustness and accuracy of the shell's command processing.

For this quesiton, you need to do some research on Linux signals. You can use this google search to get started.



What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

Answer:  Signals are software interrupts used to communicate between processes.
They notify processes about events (e.g., termination, stopping, user interrupts).
Unlike other IPC methods (pipes, sockets), signals are asynchronous .



Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

Answer:  1. SIGKILL (kill -9 )
Instantaneously stops a process with force.Cannot be ignored or caught.



SIGTERM (kill )
Asks a process to stop gracefully.This can be handled by processes, which will clean up before leaving.


SIGINT (Ctrl + C)
Sent when a user uses the keyboard to halt a process.Frequently used to halt ongoing applications, such as ping.




What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

Answer: Similar to pressing Ctrl + Z, SIGSTOP pauses a process.



It can't be blocked, ignored, or caught.
SIGCONT is the only means to restart a stopped process:
kill -CONT
