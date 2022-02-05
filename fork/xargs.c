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
xargs(char *argumentos[], int cantidad_lineas, char *comando)
{
	argumentos[cantidad_lineas + 1] = NULL;
	int status;

	int tenedor = fork();

	if (tenedor < 0) {
		perror("fork");
		_exit(-1);
	}

	if (tenedor > 0) {
		waitpid(tenedor, &status, 0);

		for (int j = 1; j < NARGS + 2; j++) {
			free(argumentos[j]);
			argumentos[j] = NULL;
		}
	}

	if (tenedor == 0) {
		execvp(comando, argumentos);
		perror("execvp");
		_exit(-1);
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
	char *argumentos[NARGS + 2] = { argv[1], NULL };
	int cantidad_lineas = 0;


	while ((linea = getline(&argumentos[cantidad_lineas + 1], &n, stdin) !=
	                -1)) {
		int len = strlen(argumentos[cantidad_lineas + 1]);

		if (argumentos[cantidad_lineas + 1][len - 1] == '\n') {
			argumentos[cantidad_lineas + 1][len - 1] = '\0';
		}
		cantidad_lineas++;


		if ((cantidad_lineas == NARGS)) {
			xargs(argumentos, cantidad_lineas, argv[1]);
			cantidad_lineas = 0;
		}
	}

	if (cantidad_lineas > 0) {
		xargs(argumentos, cantidad_lineas, argv[1]);
	}

	return 0;
}
