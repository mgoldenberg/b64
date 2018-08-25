#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "b64.h"

int main(int argc, char **argv) {
	int (*b64_fcn)(const uint8_t *, const size_t, uint8_t *, const size_t);
	int factor;

	char c;
	while((c = getopt(argc, argv, "de")) != -1) {
		switch(c) {
			case 'd': /* decode */
				b64_fcn = &b64_decode;
				factor = 3 / 4;
				break;
			case 'e': /* encode */
				b64_fcn = &b64_encode;
				factor = 4 / 3;
			default:
				abort();
		}
	}
	if(b64_fcn == NULL) {
		b64_fcn = &b64_encode;
	}

	uint8_t *in = NULL;
	size_t size = 0;
	ssize_t nread;
	while((nread = getline((char **)&in, &size, stdin)) != -1) {
		size_t inlen = nread;
		size_t outlen = (inlen * factor) + 1;

		uint8_t *out;
		if((out = (uint8_t *)malloc(sizeof(uint8_t) * outlen)) == NULL) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(b64_fcn(in, inlen, out, outlen) == -1) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("%.*s", outlen, out);
		free(out);
	}
	free(in);
}
