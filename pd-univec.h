#ifndef _PD_UNIVEC_H
#define _PD_UNIVEC_H

#define PD_UNIVEC_DEFAULT_SIZE 16
#define PD_UNIVEC_AT(v,i)							\
  (*(long *)(v->data + i * sizeof(void *)))


struct pd_univec {
  int size;
  int reserved;
  char * data;
};

struct pd_univec * pd_univec_new();
int pd_univec_add(struct pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2));
int pd_univec_expand(struct pd_univec *vec);
int pd_univec_size(struct pd_univec *vec);
int pd_univec_exist(struct pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2));
void pd_univec_free(struct pd_univec *vec);


#endif
