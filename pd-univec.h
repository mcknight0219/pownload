#ifndef _PD_UNIVEC_H
#define _PD_UNIVEC_H

#define PD_UNIVEC_DEFAULT_SIZE 16
struct pd_univec {
  int size;
  int reserved;
  void *data[1];
};

pd_univec * pd_univec_new();
int pd_univec_add(pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2));
int pd_univec_expand(pd_univec *vec);
int pd_univec_size(pd_univec *vec);
int pd_univec_exist(pd_univec *vec, void *elem, int (*cmp)(void *elem1, void *elem2));
int pd_univec_free(pd_univec *vec);

#endif
