#include <stdint.h>
#include <stddef.h>
#include <errno.h>

static const uint8_t decode_table[256] = {
	65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 62, 65, 65, 65, 63,
	52, 53, 54, 55, 56, 57, 58, 59,	60, 61, 65, 65, 65, 64, 65, 65,
	65,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22,	23, 24, 25, 65, 65, 65, 65, 65,
	65, 26, 27, 28, 29, 30, 31, 32,	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,	49, 50, 51, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65,
	65, 65, 65, 65, 65, 65, 65, 65,	65, 65, 65, 65, 65, 65, 65, 65	
};

static size_t decode_block(const uint8_t *in, uint8_t *out) {
	uint32_t buffer = 0;
	uint8_t read = 0;
	size_t i;
	for(i = 0; i < 4; i++) {
		uint8_t ui;
		switch(ui = decode_table[in[i]]) {
			case 65 /* invalid character */:
				for(; i < 4; i++) {
					buffer = buffer << 6 | 0;
				}
				break;
			case 64 /* padding */:
				read++;
				buffer = buffer << 6 | 0;
				for(i += 1; i < 4; i++) {
					if(ui = decode_table[in[i]] == 64) {
						read++;
					}
					buffer = buffer << 6 | 0;
				}
				break;
			default:
				read++;
				buffer = buffer << 6 | ui;
		}
	}
	for(i = 0; i < 3; i++) {
		size_t shift = 16 - 8 * i;
		out[i] = buffer >> shift | 0;
	}
	return read;
}

int b64_decode(const uint8_t *in, const size_t inlen, uint8_t *out, const size_t outlen) {
	int size = 0;
	size_t i;
	for(i = 0; i < inlen; i += 4) {
		uint8_t result[3];
		size_t bytes;
		if((bytes = decode_block(in + i, result)) != 4) {
			errno = EINVAL;
			return -1;
		} else if(size + bytes > outlen) {
			errno = EOVERFLOW;
			return -1;
		} else {
			size_t j;
			for(j = 0; j < 3; j++) {
				out[size++] = result[j];
			}
		}
	}
	return size;
}

static const uint8_t encode_table[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

static size_t encode_block(const uint8_t *in, const size_t inlen, uint8_t *out) {
	uint8_t sextet;

	// first six bits of first byte
	sextet = (in[0] >> 2) | 0x0;
	out[0] = encode_table[sextet];

	// last two bits of first byte
	sextet = (in[0] << 4) & 0x30;
	if(inlen > 1) {
		// first four bits of second byte
		sextet = sextet | (in[1] >> 4);
	}
	out[1] = encode_table[sextet];

	if(inlen > 1) {
		// last four bits of second byte
		sextet = (in[1] << 2) & 0x3C;
		if(inlen > 2) {
			// first two bits of third byte
			sextet = sextet | (in[2] >> 6);
		}
		out[2] = encode_table[sextet];
	} else {
		out[2] = '=';
	}

	if(inlen > 2) {
		// last six bits of third byte
		sextet =  in[2] & 0x3F;
		out[3] = encode_table[sextet];
	} else {
		out[3] = '=';
	}
	return 3;
}

int b64_encode(const uint8_t *in, const size_t inlen, uint8_t *out, const size_t outlen) {
	int size = 0;
	size_t i;
	for(i = 0; i < inlen; i += 3) {
		uint8_t block[3];
		size_t j;
		for(j = 0; j < 3; j++) {
			if(inlen > i + j) {
				block[j] = in[i + j];
			} else {
				break;
			}
		}
		uint8_t result[4];
		size_t bytes;
		if((bytes = encode_block(block, j, result)) != 3) {
			errno = EINVAL;
			return -1;
		} else if(size + bytes > outlen) {
			errno = EOVERFLOW;
			return -1;
		} else {
			size_t j;
			for(j = 0; j < 4; j++) {
				out[size++] = result[j];
			}
		}
	}
	return size;
}	
