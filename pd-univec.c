#include "pd-univec.h"
#include <stdlib.h>
#include <stdio.h>


struct pd_univec * pd_univec_new() {
  struct pd_univec *uv = (struct pd_univec *)malloc(sizeof(*uv) + (PD_UNIVEC_DEFAULT_SIZE-1) * sizeof(void *));
  if (!uv)
    return 0;
  uv->reserved = PD_UNIVEC_DEFAULT_SIZE;
  uv->size = 0;
  return uv;
}

int pd_univec_add(struct pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2)) {
  int res;
  int exist;
  if (vec->size + 1 == vec->reserved) {
    vec = pd_univec_expand(vec);
    if (vec == NULL) {
      fprintf(stderr, "Failed to expand vector.");
      return -1;
    }
		printf("Expanding the vector\n");
  }

  // use provided comparision function
  if (!pd_univec_exist(vec, elem, cmp)) {
    vec->data[vec->size] = elem;
    vec->size++;
  }
  return 0;
}

struct pd_univec * pd_univec_expand(struct pd_univec *vec) {
	int new_rsv = vec->reserved << 1;
	struct pd_univec * new_vec;
	if (NULL == (new_vec = (struct pd_univec *)realloc(vec, sizeof(struct pd_univec) + (new_rsv-1) * sizeof(void *)))) {
		fprintf(stderr, "Failed to realloc.");
		return vec;
	}

	new_vec->reserved = new_rsv;
	return new_vec;
}
  

int pd_univec_size(struct pd_univec * vec) {
  return vec->size;
}


int pd_univec_exist(struct pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2)) {
  int i;
  for (i = 0; i < vec->size; ++i) {
    if (!cmp(elem, vec->data[i]))
      return 1;
  }
  return 0;
}

// free all the data as well
void pd_univec_free(struct pd_univec *vec) {
	int i;
	if (!vec)
		return;
	
	for (i = 0; i < vec->size; ++i) {
		if (vec->data[i]) {
			free(vec->data[i]);
		}
	}
	free(vec);
	return;
}
