#ifndef _PD_URL_H
#define _PD_URL_H

#include <gumbo.h>
#include "pd-univec.h"
#include "pbc.h"

static const char * BASE = "www.gazo-ch.net/bbs/7/";
static const char * IMG_BASE = "http://img.gazo-ch.net/bbs/7/img/";

struct pd_url {
  char * link;
  int added;
  int date;
  int hash;
};
// http request related apis
void   pd_url_pages(int max_num_page, struct pd_univec * vec);
char * pd_url_get(const char *url);
int    pd_url_insert(struct pd_url *u, struct pd_univec *vec);
size_t pd_url_curl_callback(void * contents, size_t size, size_t nmemb, void * userp);
char * pd_url_get_addr(int p);
void   pd_url_parse_html(struct pd_univec * vec, char * html);
void   pd_url_parse_link(struct pd_univec * vec, GumboNode * node);
int    _pd_url_is_imgref(const char * href);
int    _pd_url_date_from_addr(const char *href);
char * _pd_url_short(const char *href);

// pbc related APIs
void   pd_url_read_file (const char * filename, struct pbc_slice * slice);
void   pd_url_write_pb(const char * filename, struct pbc_slice * slice);
void   pd_url_write_db(const char * filename, struct pbc_slice * slice);
void  pd_url_wmessage(struct pbc_env * env, struct pbc_slice * slice, struct pd_univec * vec);
int    pd_url_cmp(void *url1, void *url2);
struct pd_univec * pd_url_rmessage(struct pbc_env * env, struct pbc_slice * slice);

#endif
