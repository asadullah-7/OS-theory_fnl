#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2, pid3;

    // Create first and second pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        return 1;
    }

    // ================== CHILD 1 -> ls / -R ==================
    pid1 = fork();
    if (pid1 == 0) {
        // Redirect output of ls to pipe1
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("ls", "ls", "/", "-R", NULL);
        perror("execlp failed");
        exit(1);
    }

    // ================== CHILD 2 -> grep "std" ==================
    pid2 = fork();
    if (pid2 == 0) {
        // Input from pipe1, output to pipe2
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("grep", "grep", "std", NULL);
        perror("execlp failed");
        exit(1);
    }

    // ================== CHILD 3 -> more ==================
    pid3 = fork();
    if (pid3 == 0) {
        // Input from pipe2
        dup2(pipe2[0], STDIN_FILENO);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("more", "more", NULL);
        perror("execlp failed");
        exit(1);
    }

    // ================== Parent Process ==================
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // Wait for all children
//    wait(NULL);
//    wait(NULL);
//    wait(NULL);

    return 0;
}

