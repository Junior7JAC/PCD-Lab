/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.2
 * Programul de mai jos implementeaza un shell simplu care primeste un username si o parola,
 * si executa comenzi shell sau mai multe despartite de un delimiter, in cazul acesta ";".
 */

#include <stdio.h>//input output
#include <stdlib.h>// exit statuses
#include <string.h>//String operations
#include <unistd.h>//fork(), execvp()
#include <sys/wait.h>// pt wait()

// Adapted custom system function to execute multiple commands separated by ";"
// implementarea functiei custom system ca sa execute comenzi multiple separate prin ";"
int adapted_custom_system(char * const commands) {
    int status = 0;
    char *command = strtok(commands, ";");

    while (command != NULL) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            return -1;
        } else if (pid == 0) {
            // in procesul copil
            char *cmd = strdup(command);
            char *argv[] = {"/bin/sh", "-c", cmd, NULL};

            execvp(argv[0], argv);
            perror("execvp failed");
            free(cmd); // free string-ul duplicat
            exit(EXIT_FAILURE);
        } else {
            //procesul parinte: asteapta pt child sa termine
            waitpid(pid, &status, 0);
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                // daca o comanda esueaza, oprim executia restului si return -1
                return -1;
            }
        }
        command = strtok(NULL, ";");
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : status;
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

        // sterge caracterul newline
        char *newline = strchr(line, '\n');
        if (newline) *newline = 0;

        if (strcmp(line, "exit") == 0) { //conditia de exit
            free(line);
            break;
        }

        int result = adapted_custom_system(line); //executa comenda/comenzile
        if (result == -1) {
            printf("An error occurred while executing the commands.\n");
        }
    }
    free(line);
}

// main
int main() {
    shell();
    return 0;
}

/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc shell2.c -o shell2

RULARE ->./shell2
Login Required
Username: junior
Password: costel2002
Welcome junior! You are now logged in.
> pwd; ls; cal; echo "e ok"
/home/junior/Desktop/Tema3/Partea2_2
a.out  shell2  shell2.c
    Martie 2024
Du Lu Ma Mi Jo Vi Sb
                1  2
 3  4  5  6  7  8  9
10 11 12 13 14 15 16
17 18 19 20 21 22 23
24 25 26 27 28 29 30
31
e ok
> ^C
junior@junior-pc:~/Desktop/Tema3/Partea2_2$


./shell2
Login Required
Username: costel
Password: aaaa
Login failed!


*/
