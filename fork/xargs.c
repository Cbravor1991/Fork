#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef NARGS
#define NARGS 4
#endif

void
xargs(char *lineas[], int cantidad_lineas, char *comando)
{
	int contador = 1;
	int contador_argumentos = 1;
	char *argumentos[NARGS + 2] = { comando, NULL };
	int status;


	while (contador <= (cantidad_lineas)) {
		argumentos[contador_argumentos] = lineas[contador];
		contador_argumentos++;
		contador++;

		if ((contador_argumentos) == (NARGS + 1) ||
		    contador == cantidad_lineas + 1) {
			int tenedor = fork();
			if (tenedor > 0) {
				waitpid(tenedor, &status, 0);
				contador_argumentos = 1;
				for (int j = 1; j < NARGS + 2; j++) {
					free(argumentos[j]);
					argumentos[j] = NULL;
				}
			}

			if (tenedor < 0) {
				perror("fork");
				_exit(-1);
			}

			if (tenedor == 0) {
				int ejecucion_execvp =
				        execvp(comando, argumentos);
				if (ejecucion_execvp < 1) {
					perror("execvp");
					_exit(-1);
				}
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Se necesita un argumento.\n");
		_exit(-1);
	}

	size_t n = 0;
	ssize_t linea;
	char *lineas[100] = { argv[1], NULL };
	int cantidad_lineas = 0;

	while ((linea = getline(&lineas[cantidad_lineas + 1], &n, stdin) != -1)) {
		int len = strlen(lineas[cantidad_lineas + 1]);

		if (lineas[cantidad_lineas + 1][len - 1] == '\n') {
			lineas[cantidad_lineas + 1][len - 1] = '\0';
		}
		cantidad_lineas++;
	}

	xargs(lineas, cantidad_lineas, argv[1]);

	return 0;
}
