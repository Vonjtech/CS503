1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My shell tracks child PIDs and calls waitpid() on each to ensure all processes finish before showing the prompt. Without it, zombie processes would accumulate and waste system resources.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Unused pipe ends must be closed after dup2() to avoid resource leaks and ensure EOF is detected properly. Leaving them open can cause processes to hang.


3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd must be built-in because it changes the shell's working directory. If external, it would only change the directory in a child process, not the shell itself.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Use dynamic memory allocation instead of a fixed array. This adds complexity but allows handling any number of pipes efficiently without wasted memory.
