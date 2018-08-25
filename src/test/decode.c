#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "b64.h"

int main(int argc, const char *argv[]) {
	const char *in = argv[1];
	size_t inlen = strlen(in);

	size_t outlen = inlen;
	uint8_t *out;
	if((out = malloc(outlen)) == NULL) {
		char *err = strerror(errno);
		fprintf(stderr, "error: %s\n", err);
		exit(EXIT_FAILURE);
	}

	if(b64_decode(in, inlen, out, outlen) == -1) {
		char *err = strerror(errno);
		fprintf(stderr, "error: %s\n", err);
	} else {
		printf("%s", out);
	}
}
