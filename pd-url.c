#include "pd-url.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curl/curl.h>
#include <gumbo.h>

extern char * strdup(const char * s);

struct curl_buf {
  char * buf;
  size_t size;
};

void pd_url_pages(int max_num_page, struct pd_univec * vec) {
  int i;
  for (i = 0; i < max_num_page; ++i) {
    char * p = pd_url_get_addr(i);
    pd_url_parse_html(vec, pd_url_get(p));
  }
}

// GET method from the address
char * pd_url_get(const char * url) {
  CURL * curl;
  CURLcode res;

  struct curl_buf chunk;
  chunk.buf = malloc(1);
  chunk.size = 0;
  
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pd_url_curl_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
	      curl_easy_strerror(res));
    }
  }
  curl_easy_cleanup(curl);

  return chunk.buf;
}

size_t pd_url_curl_callback(void * contents, size_t size, size_t nmemb, void * userp) {
  size_t realsize = size * nmemb;
  struct curl_buf * cb = (struct curl_buf *)userp;
  cb->buf = realloc(cb->buf, cb->size + realsize + 1);
  
  if (!cb->buf) {
    printf("failed at malloc.");
    return 0;
  }
  memcpy(&(cb->buf[cb->size]), contents, realsize);
  cb->size += realsize;
  cb->buf[cb->size] = 0;

  return realsize;
}

char * pd_url_get_addr(int p) {
  if (!p)
    return (char *)BASE;
  int blen = strlen(BASE);
  int digit = (p > 9) ? 2 : 1;
  char * s = (char *)malloc(blen + digit + 1);
  if (!s) {
    printf("failed at malloc.");
    return 0;
  }
  char * d = (char *)malloc(digit + 1);
  sprintf(d, "%d", p);
  memcpy(s, BASE, blen);
  memcpy(s+blen, d, digit);
  s[blen+digit] = 0;
  
  return s;
}

// use Gumbo library to extract image links
void pd_url_parse_html(struct pd_univec * vec , char * html) {
  GumboOutput * output = gumbo_parse(html);
  pd_url_parse_link(vec, output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
}

// recursively fill up vec with links
void pd_url_parse_link(struct pd_univec * vec, GumboNode * node) {
  int i;
  if (node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  GumboAttribute * href;
  if (node->v.element.tag == GUMBO_TAG_A &&
      (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
    if (_pd_url_is_imgref(href->value)) {
      struct pd_url *u = malloc(sizeof(*u));
      char * shorten = _pd_url_short(href->value); 
      u->link = shorten;
      u->added = 0;
      u->date = 0;//_pd_url_date_from_addr(u->link);
      u->hash = 0;
      pd_univec_add(vec, u, pd_url_cmp);
    }
  }

  // TODO, considering the structure of the target page,
  // this is an extremely inefficient way to explore the
  // image link.
  // recusively  search children
  GumboVector * children = &node->v.element.children;
  for (i = 0; i < children->length; ++i) {
    pd_url_parse_link(vec, (GumboNode *)children->data[i]);
  }
}

// image link looks like http://img.gazo-ch.net/bbs/7/img/201309/1325956.jpg
int _pd_url_is_imgref(const char * href) {
  if (!strncmp(href, IMG_BASE, strlen(IMG_BASE)))
    return 1;
  else
    return 0;
}

int _pd_url_date_from_addr(const char * href) {
  int len = strlen(IMG_BASE);
  int date, base = 1;
  char * slash = strchr(href+len, '/');
  if (slash)
    return -1;

  for (int i = slash-href;i >= len;i--) {
    date += (href[i]-'0') * (pow(10, (slash-href)-i));
  }
  return date;
}

// we don't need to store the whole url, just
// the varying parts
char * _pd_url_short(const char * href) {
  size_t lsz = strlen(IMG_BASE);
  size_t sz = strlen(href);
  char *ret = (char *)malloc(sz-lsz+1);
  memcpy(ret, href+lsz, sz-lsz);
  ret[sz-lsz] = 0;
  return ret;
}


void pd_url_read_file(const char *filename, struct pbc_slice * slice) {
  FILE *f =fopen(filename, "rb");
  if (!f) {
    slice->buffer = NULL;
    slice->len = 0;
    return;
  }

  fseek(f, 0, SEEK_END);
  slice->len = ftell(f);
  fseek(f, 0, SEEK_SET);
  slice->buffer = malloc(slice->len);
  fread(slice->buffer, 1, slice->len, f);
  fclose(f);
}

// write the slice into a file for storage
void pd_url_write_db(const char *filename, struct pbc_slice *slice) {
  int b;
  if (!slice)
    return;
  FILE *f = fopen(filename, "wb");
  b = fwrite(slice->buffer, 1, slice->len, f);
  if (b != slice->len) {
    printf("Failed at writing to file");
  }
  fclose(f);
}


	
struct pd_univec * pd_url_rmessage(struct pbc_env * env, struct pbc_slice * slice) {
  int url_n, i;
  struct pd_univec *vec = pd_univec_new();
  struct pbc_rmessage * m = pbc_rmessage_new(env, "Pownload.Urls", slice);
  if (m == NULL) {
    printf("Failed at reading message.\n");
    return NULL;
  }

  // get the total number of url records,
  // and then iteratre over them.
  url_n = pbc_rmessage_size(m, "url");
  for (i = 0; i < url_n; ++i) {
    struct pd_url * pu = (struct pd_url *)malloc(sizeof(*pu));
    struct pbc_rmessage *u = pbc_rmessage_message(m, "url", i);
    pu->link = (char *)pbc_rmessage_string(u, "link", i, NULL);
    pu->added = pbc_rmessage_integer(u, "added", i, NULL);
    pu->date = pbc_rmessage_integer(u, "date", i, NULL);
    pu->hash = pbc_rmessage_integer(u, "hash", i, NULL);
    pd_univec_add(vec, pu, pd_url_cmp);
    //pbc_rmessage_delete(u);
  }
  pbc_rmessage_delete(m);

  return vec;
}

void dump_hex(char *buffer, int sz) {
  int i, j;
  for (i = 0; i < sz; ++i) {
    printf("%02X ", buffer[i]);
    if (i % 16 == 15) {
      for (j = 0; j < 16; ++j) {
	char c = buffer[i/16 * 16+j];
	if (c>=32 && c < 127) {
	  printf("%c",c);
	} else {
	  printf(".");
	}
      }
      printf("\n");
    }
  }
  printf("\n");
}

// write the contents from vector to the message
void pd_url_wmessage(struct pbc_env * env, struct pbc_slice * slice, struct pd_univec * vec) {
  int i, url_n = vec->size;
  struct pbc_wmessage * msg = pbc_wmessage_new(env, "Pownload.Urls");

  struct pbc_wmessage * u;
  printf("Writing %d messages\n", url_n);
  for (i = 0; i < url_n; ++i) {
    u = pbc_wmessage_message(msg, "url");
    const char * s = ((struct pd_url*)(PD_UNIVEC_AT(vec,i)))->link;
    int added = ((struct pd_url*)(PD_UNIVEC_AT(vec,i)))->added;
    int date = ((struct pd_url*)(PD_UNIVEC_AT(vec,i)))->date;
    int hash = ((struct pd_url*)(PD_UNIVEC_AT(vec,i)))->hash;
    
    pbc_wmessage_string (u, "link",  s,  -1);
    pbc_wmessage_integer(u, "added", added,  0);
    pbc_wmessage_integer(u, "date",  date,  0);
    pbc_wmessage_integer(u, "hash",  hash,  0);
    
  }
  pbc_wmessage_buffer(msg, slice);
  //pbc_wmessage_delete(msg);
}



// compare two url
int pd_url_cmp(void *url1, void *url2) {
  struct pd_url *u1 = (struct pd_url *)url1;
  struct pd_url *u2 = (struct pd_url *)url2;
  return strcmp(u1->link, u2->link);
}
