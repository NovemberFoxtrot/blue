#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	void **array;
	int *objects;
	int i;
	char *string = "dude";

	array = malloc(1 * sizeof(void *));
	if(array == NULL) {
		printf("malloc fail array");
		exit(1);
	}

	objects = malloc(10 * sizeof(int));
	if(objects == NULL) {
		printf("malloc fail objects");
		exit(1);
	}

	for(i = 0; i < 10; i++) {
		objects[i] = i;
	}

	for(i = 0; i < 10; i++) {
		printf("%d\n", objects[i]);
	}

	array[0] = objects;
	array[1] = string;

	for(i = 0; i < 10; i++) {
		printf("%d\n", ((int *)array[0])[i]);
	}

	printf("%s\n", ((char *)array[1]));

	free(objects);
	free(array);

	return 0;
}
