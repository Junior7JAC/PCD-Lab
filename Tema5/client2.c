/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 5
    * Acest program demonstreaza utilizarea socket-urilor pentru a crea un server care poate fi accesat de un client.
    * Serverul poate oferi informatii despre timpul curent, utilizatorul curent, poate executa comenzi shell si poate fi oprit de la distanta.
    * Se foloseste un fisier de log pentru a inregistra activitatile serverului.
    * Se foloseste un fisier de credentiale pentru a valida utilizatorii.
    * Se foloseste un port si un IP prestabilite pentru conexiunea client-server.
    *
    * Am tratarea erorilor si situatiilor limita, precum:
    * -- erori la crearea socket-ului
    * -- erori la bind
    * -- erori la listen
    * -- erori la accept
    * -- erori la citirea de la client
    * -- erori la scrierea catre client
    * -- erori la deschiderea fisierelor de credentiale si de log
 */

#include <stdio.h> // pentru printf și scanf
#include <string.h> // pentru strlen
#include <sys/socket.h> // pentru socket
#include <netinet/in.h> // pentru structura INET
#include <unistd.h> // pentru close
#include <arpa/inet.h> // pentru inet_pton
#include <stdlib.h> // pentru wait

#define SERVER_PORT 12346
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 2048

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

//functia de afisare meniu
void print_menu() {
    printf(">>> 1. Send Message\n");
    printf(">>> 2. Request Time\n");
    printf(">>> 3. Request User\n");
    printf(">>> 4. Execute Command\n");
    printf(">>> 5. Close Connection\n");
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char username[256], password[256];
    int is_authenticated = 0;

    // creare socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Could not create socket");
        return 1;
    }

    // setare structura server_addr pentru serverul cu care se va face conexiunea
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // conectare la server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Could not connect");
        return 1;
    }

    // autentificare
    while (!is_authenticated) {
        printf("Login: ");
        scanf("%255s", username);
        printf("Password: ");
        scanf("%255s", password);

        is_authenticated = login(username, password);
        if (is_authenticated) {
            printf("Autentificare reușită.\n");
        } else {
            printf("Nume de utilizator sau parolă incorecte. Încercați din nou.\n");
        }
    }

    // afisarea meniului
    print_menu();
    int option = 0; //optiunea utilizatorului
    while (1) {
        printf("Select: ");
        scanf("%d", &option);
        getchar(); // curata bufferul de orice caracter extra

        switch (option) {
            case 1:
                printf("Enter message: ");
                fgets(message, BUFFER_SIZE, stdin);
                message[strcspn(message, "\n")] = 0; // delete newline-uld e la final
                break;
            case 2:
                strcpy(message, "REQUEST_TIME");
                break;
            case 3:
                strcpy(message, "REQUEST_USER");
                break;
            case 4:
                //buffer temporar pentru a scapa de "strlen("EXECUTE_COMMAND: ")"
                //am incercat si fara a face unul temporar, dar nu mi-a mers
                char temp[BUFFER_SIZE];
                printf("Enter command: ");
                fgets(temp, BUFFER_SIZE, stdin);
                temp[strcspn(temp, "\n")] = 0; // Remove newline at end
                //formatare pt a citi doar comanda, si nu "EXECUTE_COMMAND: "
                snprintf(message, BUFFER_SIZE, "EXECUTE_COMMAND: %.*s",BUFFER_SIZE - 18, temp);
                break;
            case 5:
                strcpy(message, "CLOSE_CONNECTION");
                break;
            default:
                printf("Invalid option. Please try again.\n");
                continue;
        }

        // trimitere mesaj la server si primire raspuns
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Could not send message");
            continue;
        }

        if (option == 5) {
            close(sock); //daca optiunea 5, inchide socket ul
            break;
        }

        if (recv(sock, buffer, BUFFER_SIZE, 0) < 0) {
            perror("Could not receive response");
            continue;
        }
        printf("::: %s :::\n", buffer);
        memset(buffer, 0, BUFFER_SIZE); //curata buffer ul

        print_menu();
    }

    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * gcc client2.c -o client2 ---> COMPILARE
 * ./client2 ----> RULARE
 *
 * junior@junior-pc:~/Desktop/PCD/Tema5$ ./client2
Login: juni
Password: 123
Autentificare reușită.
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
Select: 1
Enter message: salut
::: salut :::
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
Select: 2
::: Wed Apr  3 15:35:47 2024 :::
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
Select: 3
::: junior :::
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
Select: 4
Enter command: ls
::: a.out
client1
client1.c
client2
client2.c
server1
server1.c
server2
server2.c
server_logs.log
 :::
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
Select: 5
junior@junior-pc:~/Desktop/PCD/Tema5$

 ERORI:

 junior@junior-pc:~/Desktop/PCD/Tema5$ ./client2
Login: juni
Password: abc
Nume de utilizator sau parolă incorecte. Încercați din nou.
Login:


 junior@junior-pc:~/Desktop/PCD/Tema5$ ./client2
Login: juni
Password: 123
Autentificare reușită.
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
 Select: 4
Enter command: abc
::: sh: 1: abc: not found :::
>>> 1. Send Message
>>> 2. Request Time
>>> 3. Request User
>>> 4. Execute Command
>>> 5. Close Connection
Select: 5
junior@junior-pc:~/Desktop/PCD/Tema5$
 */