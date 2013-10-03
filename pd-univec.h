#ifndef _PD_UNIVEC_H
#define _PD_UNIVEC_H

struct pd_univec {
  int size;
  int reserved;
  void *data[1];
};

int pd_univec_new();
int pd_univec_add(void *elem);
int pd_univec_expand();
int pd_univec_size();

#endif
