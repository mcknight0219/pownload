#ifndef _PD-URL_H
#define _PD-URL_H
#include "pd-univec.h"

struct url;
// get urls from retrieved pages
pd_univec *pd_get_pages(int max_num_page = -1);
// spawn a process (wget) to retrieve a page
char *pd_web_get(const char *url);
int pd_url_insert(struct url *u, pd_hash_tbl *ht);
// send a list of urls for the downloading process
// to work on. 
int pd_send(pd_univec *uv);
#endif
