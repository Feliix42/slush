#include <main.h>

#define MAX_CMD_LENGTH 300

// int main(int argc, char **argv) {
int main(void) {
	printf("Welcome to slush - the stupid & lightly underwhelming shell!\n");

	char* input = calloc(MAX_CMD_LENGTH + 1, sizeof(char));

	while (true) {
		char* user = getenv("USER");

		if (user) {
			printf("[%s: slush] ", user);
		} else {
			printf("[slush] ");
		}

		// get input
		if (!fgets(input, MAX_CMD_LENGTH + 1, stdin))
			break;

		// TODO: Cut the \n that might exists at the end of the input string

		printf("Received: %s\n", input);
	}

	printf("exit\n");
	return 0;
}
