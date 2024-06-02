/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 1
 * Programul de mai jos este o aplicatie de gestionare a unei liste de sarcini
 * care permite utilizatorului sa adauge, stearga, listeze sarcinile existente dintr-o lista,
 * sau sa ceara ajutor. Sarcinile sunt salvate intr-un fisier .csv.
 * Pentru ca aplicatia sa ruleze primeste pe linia de comanda --add, --delete, --list, --help
 * pentru comenzile mentionate mai sus.
 * Am tratat urmatoarele erori care pot aparea in contextul
 * programului de mai jos :
 * -- argumente inexistente
 * -- argumente utilizare gresit
 * -- in cazul comenzilor compuse: daca argumentul secundar nu este corespunzator (./app1 -a)
 */
#include <stdio.h> // libraria standard pentru functii I/O
#include <stdlib.h> // libraria standard pentru alocare de memorie, control procese etc.
#include <getopt.h> // libraria pentru parsarea optiunilor in cmd

#define MAX_TASK_LENGTH 100 //lungimea maxima a descrierii sarcinii
#define FILENAME "app1.csv" //shortcut pentru fisierul csv

// struct-ul pentru o sarcina, format din id si descriere
typedef struct {
    int id;
    char description[MAX_TASK_LENGTH];
} Task;

// contor pentru ID-uri
 int task_counter = 0;

 // functie pentru actualizarea contorului id-ului
void read_task_counter() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        Task current_task;
        while (fscanf(file, "%d,%99[^\n]", &current_task.id, current_task.description) == 2) {
            if (current_task.id > task_counter) {
                task_counter = current_task.id;
            }
        }
        fclose(file);
    }
}

// functie pentru adaugarea unei noi sarcini
void add_task(char *description) {
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        perror("Eroare la deschiderea fisierului");
        exit(EXIT_FAILURE);
    }

    Task new_task;
    new_task.id = ++task_counter;
    snprintf(new_task.description, sizeof(new_task.description), "%s", description);

    fprintf(file, "%d,%s\n", new_task.id, new_task.description);

    fclose(file);
}

// functie pentru stergerea unei sarcini. Se foloseste un fisier tmp pentru a rescrie tot din fisierul original si
// a efectua modificari in acesta, avand garantia pastrarii datelor
void delete_task(int id) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        perror("Eroare la deschiderea fisierului");
        exit(EXIT_FAILURE);
    }

    FILE *temp_file = fopen("temp.csv", "w");
    if (temp_file == NULL) {
        perror("Eroare la deschiderea fisierului temporar");
        exit(EXIT_FAILURE);
    }

    Task current_task;
    int deleted = 0;

    while (fscanf(file, "%d,%99[^\n]", &current_task.id, current_task.description) == 2) {
        if (current_task.id != id) {
            fprintf(temp_file, "%d,%s\n", current_task.id, current_task.description);
        } else {
            deleted = 1;
        }
    }

    fclose(file);
    fclose(temp_file);

    if (!deleted) {
        remove("temp.csv"); // stergem fisierul temporar daca nu s-a gasit sarcina cu id-ul specificat
        printf("Nu s-a găsit nicio sarcină cu ID-ul %d.\n", id); // daca spcifici un id care nu este in fisier
        return;
    }

    // se incearca stergerea fisierului original
    if (remove(FILENAME) != 0) {
        perror("Eroare la ștergerea fișierului original");
        exit(EXIT_FAILURE);
    }

    // se incearca redenumirea fisierului temporar
    if (rename("temp.csv", FILENAME) != 0) {
        perror("Eroare la redenumirea fișierului temporar");
        exit(EXIT_FAILURE);
    }

    printf("Sarcina cu ID-ul %d a fost ștearsă.\n", id);
}

// functie pentru listarea sarcinilor existente
void list_tasks() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        perror("Eroare la deschiderea fisierului");
        exit(EXIT_FAILURE);
    }

    Task current_task;
    printf("Lista sarcinilor:\n");
    printf("ID\tDescriere\n");

    while (fscanf(file, "%d,%99[^\n]", &current_task.id, current_task.description) == 2) {
        printf("%d\t%s\n", current_task.id, current_task.description);
    }

    fclose(file);
}

// functie pentru afisarea comenzilor de ajutor (help)
void display_help() {
    printf("Utilizare: app1 [OPTIUNE]\n");
    printf("Aplicatie de gestionare a sarcinilor.\n\n");
    printf("Optiuni:\n");
    printf("  -a, --add DESCRIERE  Adauga o noua sarcina\n");
    printf("  -d, --delete ID      Sterge o sarcina dupa ID\n");
    printf("  -l, --list           Listeaza toate sarcinile\n");
    printf("  -h, --help           Afișează acest mesaj de ajutor\n");
}

int main(int argc, char *argv[]) {
    // definirea operatiilor
    static struct option long_options[] = {
            {"add", required_argument, 0, 'a'},
            {"delete", required_argument, 0, 'd'},
            {"list", no_argument, 0, 'l'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;

    read_task_counter(); // initializeaza task counter-ul cu valoarea corespunzatoare din fisier


    // procesarea operatiilor folosind getopt_long()
    while ((opt = getopt_long(argc, argv, "a:d:lh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                add_task(optarg);
                break;
            case 'd':
                delete_task(atoi(optarg));
                break;
            case 'l':
                list_tasks();
                break;
            case 'h':
                display_help();
                exit(EXIT_SUCCESS);
            case '?':
                // daca se introduce o operatie neidentificata sau lipsesc argumente necesare
                fprintf(stderr, "Utilizare: %s [--add description] [--delete id] [--list] [--help]\n", argv[0]);
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Eroare în procesarea opțiunilor\n");
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

/* Exemple de compilare si rulare a programului

 gcc -o app1 app1.c        --> compilare


./app1 -a aaa
./app1 -l
Lista sarcinilor:
ID      Descriere
1       aaa


./app1 -a
./app1: option requires an argument -- 'a'
Utilizare: ./app1 [--add description] [--delete id] [--list] [--help]


./app1 --delete 1
Sarcina cu ID-ul 1 a fost ștearsă.

./app1 --list
Lista sarcinilor:
ID      Descriere

./app1 -d 7
Nu s-a găsit nicio sarcină cu ID-ul 7.

 */
