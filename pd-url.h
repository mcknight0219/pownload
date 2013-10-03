#ifndef _PD_URL_H
#define _PD_URL_H
#include "pd-univec.h"
#include "pbc.h"

struct pd_url;

struct pd_univec * pd_url_pages(int max_num_page = -1);
char *pd_url_get(const char *url);
int pd_url_insert(struct url *u, struct pd_univec *vec);

// a group of APIs
void pd_url_read_pb(const char * filename, struct pbc_slice * slice);
void pd_url_write_pb(const char * filename, struct pbc_slice * slice);
struct pd_univec * pd_url_rmessage(struct pbc_env * env, struct pbc_slice * slice);
void pd_url_wmessage(struct pbc_env * env, struct pbc_slice * slice);

int pd_url_cmp(void *url1, void *url2);
#endif
