/**
 * Nume si prenume: Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 4
 * <Includeti aici un scurt rezumat al temei>
 * Am creat un program in care sa transferam un numar de biti dintr-un fisier in altul.
 * Am tratat primele 3 cerinte din tema, si anume functiile de myRead si myWrite, thread-urile joinable, si
 * functionalitatea de traversare a unui director.
 * Am tratat urmatoarele situatii limita care pot aparea inbcontextul programului de mai jos :
 * -- numar de argumente incorect
 * -- valori incorecte pentru argumentele date (nu sunt numerice, valoarea nu se conformeaza constrangerilor problemei)
 */

#include <unistd.h> // pentru fork
#include <stdlib.h> // pentru malloc
#include <stdio.h> // pentru printf, scanf
#include <string.h> // pentru strcpy, strcat
#include <pthread.h>// pentru thread-uri
#include <fcntl.h>     // pentru O_RDONLY, O_WRONLY

// functie pentru citirea unui numar de bytes dintr-un fisier
size_t myRead(int fd, char *buffer, size_t size) {
    size_t bytesRead, totalRead = 0;
    // citeste din fisier pana cand nu mai sunt bytes de citit
    while ((bytesRead = read(fd, buffer + totalRead, size - totalRead)) > 0) {
        buffer[bytesRead] = '\0'; // null-terminate string-ul
        totalRead += bytesRead; // incrementam numarul total de bytes cititi
    }
    return totalRead; // returnam numarul total de bytes cititi
}

// functie pentru scrierea unui numar de bytes intr-un fisier
size_t myWrite(int fd, char *buffer, size_t size) {
    size_t totalWritten = 0;
    ssize_t bytesWritten;
    while (totalWritten < size) { // cat timp nu am scris toate bytes-urile
        bytesWritten = write(fd, buffer + totalWritten, size - totalWritten); // scriem in fisier
        if (bytesWritten == -1) {
            perror("Error writing to file");
            return totalWritten; // returneaza numarul de bytes scrisi pana acum
        }
        totalWritten += bytesWritten;
    }
    return totalWritten; // returnea numarul total de bytes scrisi
}

//struct din lab
typedef struct inFile{
    char * filePath;
    char * buffer;
    size_t size;
}inFile_t;

//struct din lab
typedef struct outFile{
    char * filePath;
    char * buffer;
    size_t size;
}outFile_t;

// functie pentru a citi un numar de bytes dintr-un fisier
void *inThread(void *arg) {
    inFile_t *file = (inFile_t *)arg;
    int fd = open(file->filePath, O_RDONLY); // deschide fisierul pentru citire
    if (fd == -1) {
        perror("Error opening file for reading");
        return NULL;
    }
    file->size = myRead(fd, file->buffer, file->size); // citeste din fisier
    printf("Read (bytes): %zu\n", file->size); // scrie numarul de bytes cititi
    close(fd); // inchide fisierul
    return arg; // returneaza argumentul primit
}

// functie pentru a scrie un numar de bytes intr-un fisier
void *outThread(void *arg) {
    outFile_t *file = (outFile_t *)arg;
    int fd = open(file->filePath, O_WRONLY | O_CREAT, 0666); // deschide fisierul pentru scriere
    if (fd == -1) {
        perror("Error opening file for writing");
        return NULL;
    }
    file->size = myWrite(fd, file->buffer, file->size); // scrie in fisier
    printf("Written (bytes): %zu\n", file->size); // scrie numarul de bytes scrise
    close(fd); // inchide fisierul
    return arg; // returneaza argumentul primit
}

int main(int argc, char *argv[]) {
    char *buffer = (char *)malloc(2048); // alocam 2048 bytes pentru buffer
    size_t bytesToRead = 2048; // numarul de bytes pe care vrem sa-i citim
    inFile_t inFile = {.filePath = "input.txt", .buffer = buffer, .size = bytesToRead}; // fisierul de intrare
    outFile_t outFile = {.filePath = "output.txt", .buffer = NULL, .size = 0}; // fisierul de iesire

    pthread_t thr1, thr2; // declaram thread-urile
    pthread_create(&thr1, NULL, inThread, &inFile); // cream thread-ul pentru citire
    pthread_join(thr1, NULL); // asteptam ca thread-ul sa se termine

    outFile.buffer = inFile.buffer; // copiem buffer-ul din fisierul de intrare in cel de iesire
    outFile.size = inFile.size; // copiem numarul de bytes cititi in cel de iesire
    pthread_create(&thr2, NULL, outThread, &outFile); // cream thread-ul pentru scriere
    pthread_join(thr2, NULL); // asteptam ca thread-ul sa se termine
    return 0;
}
/*

junior@junior-pc:~/Desktop/Tema4$ gcc app1.c -o app1 --------> COMPILARE
junior@junior-pc:~/Desktop/Tema4$ ./app1 --------> RUN
Read (bytes): 126
Written (bytes): 126


*/