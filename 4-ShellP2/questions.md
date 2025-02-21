1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  fork creates a new process while execvp just replaces the current process. fork allows the shell to run while the command is still executing

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  if fork fails, a stderr is printed and the parent process stops.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: it looks for the command in the PATH environment.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() waits for the child to finish processing and without it the parent wouldn't know when the child finishes.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: it gets the exit status of the child process, it is important because it tracks if the command ran or not.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: it handles it like one argument but my implementation does not work successfully. this is needed because the different arguments need to stay as one to read each command.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: I attempted to handle quoted arguments together instead. There were a lot of challenges in refactoring the code.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  signals are used to notify processes, they differ because they are about notifying rather than changing data.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: forces a process to stop
    SIGTERM: requests a process to stop
    SIGINT: interrupts a process

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: SIGSTOP stops a process. it cant be caught or ignored.
