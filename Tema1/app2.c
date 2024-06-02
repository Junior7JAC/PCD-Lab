/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 1
 * Acest program are ca scop afisarea sumei si produsului a doua numere de tip real cu precizie dubla (double).
 * Am tratat urmatoarele erori care pot aparea in contextul
 * programului de mai jos:
 * -- argumente incorecte la apelul comenzii
 * -- valori incorecte pentru argumentele date
 */

#include <stdio.h> // libraria standard pentru functii I/O
#include <stdlib.h> // libraria standard pentru alocare de memorie, control procese etc.
#include <getopt.h> // libraria pentru parsarea optiunilor in cmd

// functia pentru comanda help
void print_help(const char *program_name) {
    printf("Utilizare: %s [-sum | -product | -help]\n", program_name);
    printf("  -sum        Calculeaza suma a doua numere\n");
    printf("  -product    Calculeaza produsul a doua numere\n");
    printf("  -help       Afiseaza acest mesaj de ajutor\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int opt;
    double num1, num2;

    // struct-ul pentru operatii
    static struct option long_options[] = {
            {"sum", no_argument, 0, 's'},
            {"product", no_argument, 0, 'p'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
    };

    // procesarea operatiilor
    while ((opt = getopt_long_only(argc, argv, "sph", long_options, NULL)) != -1) {
        switch (opt) {
            case 's':
                // optiunea -s sau -sum
                printf("Introdu primul numar: ");
                scanf("%lf", &num1);
                printf("Introdu al doilea numar: ");
                scanf("%lf", &num2);
                printf("Suma: %.2lf\n", num1 + num2);
                exit(EXIT_SUCCESS);

            case 'p':
                // optiunea -p sau -product
                printf("Introdu primul numar: ");
                scanf("%lf", &num1);
                printf("Introdu al doilea numar: ");
                scanf("%lf", &num2);
                printf("Produs: %.2lf\n", num1 * num2);
                exit(EXIT_SUCCESS);

            case 'h':
                // optiunea -h sau -help
                print_help(argv[0]);

            case '?':
                // optiune nevalida
                fprintf(stderr, "Incearca '%s -help' pentru mai multe informatii.\n", argv[0]);
                exit(EXIT_FAILURE);

            default:
                // alte cazuri neprevazute
                fprintf(stderr, "Eroare neasteptata.\n");
                exit(EXIT_FAILURE);
        }
    }

    // Daca nu s-au furnizat optiuni, se afiseaza un mesaj de ajutor.
    print_help(argv[0]);

    return 0;
}

/* Exemple de compilare si rulare a programului

 gcc -o app2 app2.c        --> compilare


./app2 -sum
Introdu primul numar: 12
Introdu al doilea numar: 23
Suma: 35.00


./app2 -prod
Introdu primul numar: 12
Introdu al doilea numar: -12
Produs: -144.00



ERORI:

./app2 -asda
./app2: unrecognized option '-asda'
Incearca './app2 -help' pentru mai multe informatii.


./app2 -sum
Introdu primul numar: a
Introdu al doilea numar: Suma: 0.00

 */