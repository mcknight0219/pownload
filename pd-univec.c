#include "pd-univec.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct pd_univec * pd_univec_new() {
  struct pd_univec *uv = (struct pd_univec *)malloc(sizeof(*uv));
  if (!uv)
    return 0;
  uv->data = malloc(sizeof(void *) * PD_UNIVEC_DEFAULT_SIZE); 
 
  if (!uv->data)
    return 0;
  uv->reserved = PD_UNIVEC_DEFAULT_SIZE;
  uv->size = 0;
  return uv;
}

int pd_univec_add(struct pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2)) {
  int res;
  int exist;
  if (vec->size + 1 == vec->reserved) {
    res = pd_univec_expand(vec);
    if (res == -1) {
      fprintf(stderr, "Failed to expand vector.");
      return -1;
    }
    printf("Expanding the vector\n");
  }

  // use provided comparision function
  if (!pd_univec_exist(vec, elem, cmp)) {
    memcpy((vec->data + (vec->size * sizeof(void *))), &elem, sizeof(void *));
    vec->size++;
  }
  return 0;
}

int pd_univec_expand(struct pd_univec * vec) {
  int new_rsv = vec->reserved << 1;
  void * new_data;
  if (NULL == (new_data = realloc(vec->data, new_rsv * sizeof(void *)))) {
    fprintf(stderr, "Failed to realloc.");
    return -1;
  }
	
  vec->reserved = new_rsv;
  vec->data = new_data;
  return 0;
}
  

int pd_univec_size(struct pd_univec * vec) {
  return vec->size;
}


int pd_univec_exist(struct pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2)) {
  int i;
  for (i = 0; i < vec->size; ++i) {
    if (!cmp(elem, (void *)PD_UNIVEC_AT(vec, i)))
      return 1;
  }
  return 0;
}

// free all the data as well
void pd_univec_free(struct pd_univec *vec) {
  int i;
  if (!vec)
    return;
  // TODO free the memory hold by vec->data
  for (i = 0; i < vec->size; ++i) {
    
  }
  free(vec);
  return;
}
