#include "malloc.h"

void *reallocarray(void *p, size_t nmemb, size_t size) {
  return realloc(p, nmemb * size);
}