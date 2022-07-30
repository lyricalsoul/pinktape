// functionality that jakt does not provide (yet)
// most of this cannot be implemented directly without breaking multiplatform support
#pragma once

#include <time.h>
#include <stdlib.h>

// FIXME: using an os-based rand implementation would be much better (although this one might be just enough)
void call_srand() {
    srand((unsigned int) time(NULL));
}

void free_buffer(char *data) {
    free(data);
}

char *generate_random_bytes(size_t size) {
    unsigned char *stream = (unsigned char *) malloc(size);
    size_t i;
    for (i = 0; i < size; i++) {
        stream[i] = rand() % 255;
    }
    return (char *) stream;
}

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// FIXME: this implementation generates some very weird values (also i don't think it does padding). works for pinktape's use case, though
Jakt::Optional<char *> base64_encode(const char *in_s, size_t len) {
    char *out;
    const unsigned char* in = (const unsigned char*)in_s;
    size_t elen;
    size_t i;
    size_t j;
    size_t v;

    if (in == NULL || len == 0)
        return Jakt::NullOptional{};

    elen = len;
    if (len % 3 != 0)
        elen += 3 - (len % 3);
    elen /= 3;
    elen *= 4;
    out = (char*)malloc(elen + 1);
    out[elen] = '\0';

    for (i = 0, j = 0; i < len; i += 3, j += 4) {
        v = in[i];
        v = i + 1 < len ? v << 8 | in[i + 1] : v << 8;
        v = i + 2 < len ? v << 8 | in[i + 2] : v << 8;

        out[j] = b64chars[(v >> 18) & 0x3F];
        out[j + 1] = b64chars[(v >> 12) & 0x3F];
        if (i + 1 < len) {
            out[j + 2] = b64chars[(v >> 6) & 0x3F];
        } else {
            out[j + 2] = '=';
        }
        if (i + 2 < len) {
            out[j + 3] = b64chars[v & 0x3F];
        } else {
            out[j + 3] = '=';
        }
    }

    return out;
}
