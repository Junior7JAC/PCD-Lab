/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.2
 * Programul de mai jos implementeaza un shell simplu care primeste un username si o parola,
 * si executa comenzi shell.
 */

#include <stdio.h>//input output
#include <stdlib.h>// exit statuses
#include <string.h>//String operations
#include <unistd.h>//fork(), execvp()
#include <sys/wait.h>// pt wait()


// implementarea unei functii custom system folosind fork si execvp
int custom_system(const char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        //duplica string-ul command ca sa-l fac non-const
        char *cmd = strdup(command);
        char *argv[] = {"/bin/sh", "-c", cmd, NULL};

        execvp(argv[0], argv);
        perror("execvp failed");
        free(cmd); // free string-ul duplicat
        exit(EXIT_FAILURE);
    } else {
        //procesul parinte: asteapta pt child sa termine
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

// functia de login
int login(const char *username, const char *password) {
    FILE *file = fopen("/tmp/credentials.txt", "r");
    if (file == NULL) {
        perror("Unable to open the credentials file");
        return 0;
    }

    char file_username[256], file_password[256];
    while (fscanf(file, "%255s %255s", file_username, file_password) == 2) {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            fclose(file);
            return 1; // login cu succes
        }
    }

    fclose(file);
    return 0; // login fara succes
}

// functia de shell
void shell() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("Login Required\n");
    char username[256], password[256];
    printf("Username: ");
    fgets(username, sizeof(username), stdin); //numele
    username[strcspn(username, "\n")] = 0; // sterge caracterul newline
    printf("Password: ");
    fgets(password, sizeof(password), stdin); //parola
    password[strcspn(password, "\n")] = 0; // sterge caracterul newline

    if (!login(username, password)) {
        printf("Login failed!\n");
        return; // iese daca esueaza loginul
    }

    printf("Welcome %s! You are now logged in.\n", username);

    while (1) {
        printf("> "); // prompt pt input
        read = getline(&line, &len, stdin); // citeste o linie de input
        if (read == -1) {
            free(line);
            if (feof(stdin)) {
                break; // end of file
            } else {
                perror("getline failed");
                continue;
            }
        }
        if (strcmp(line, "exit\n") == 0) { //conditia de exit
            free(line);
            break;
        }
        custom_system(line); // executa comanda
    }
    free(line);
}

// main
int main() {
    shell();
    return 0;
}
/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc shell.c -o shell

RULARE ->./shell
Login Required
Username: junior
Password: costel2002
Welcome junior! You are now logged in.
> pwd
/home/junior/Desktop/Tema3/Partea2
> ^C
junior@junior-pc:~/Desktop/Tema3/Partea2$


 ./shell
Login Required
Username: costel
Password: aaaa
Login failed!


*/
