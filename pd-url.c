#include "pd-url.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct pd_url {
	char * link;
	int added;
	int date;
	unsigned long hash;
};

struct pd_univec * pd_url_pages(int max_num_page) {
  int i;
  struct pd_univec * urls = pd_univec_new();
  for (i = 0; i < max_num_page; ++i) {
    char * p = pd_url_get_addr(i);
    pd_url_process_page(urls, pd_url_get(p));
  }
  return urls;
}

// GET method from the address
char * pd_url_get(const char * url) {
  CURL * curl;
  CURLcode res;

  char * buf;
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pd_url_curl_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
	      curl_easy_strerror(res));
    }
  }
  curl_easy_cleanup(curl);
  return buf;
}

size_t pd_url_curl_callback(void * contents, size_t size, size_t nmemb, void * userp) {
  size_t realsize = size * nmemb;
  char **buf = (char **)userp;
  *buf = (char *)malloc(realsize + 1);
  if (!*buf) {
    error("failed at malloc.");
    return 0;
  }
  memcpy(*buf, contents, realsize);
  (*buf)[realsize] = 0;

  return realsize;
}

char * pd_url_get_addr(int p) {
  int blen = strlen(BASE);
  int digit = (p > 9) ? 2 : 1;
  char * s = (char *)malloc(strlen(blen) + digit + 1);
  if (s) {
    error("failed at malloc.");
    return 0;
  }
  memcpy(s, BASE, blen);
  
}



void pd_url_read_pb(const char *filename, struct pbc_slice *slice) {
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

void pd_url_write_pb(const char *filename, struct pbc_slice *slice) {
	int b;
	if (!slice)
		return;
	FILE *f = fopen(filename, "wb");
	b = fwrite(slice->buffer, 1, slice->len, f);
	if (b != slice->len) {
		error("Failed at writing to file");
	}
	fclose(f);
}
	
struct pd_univec * pd_url_rmessage(struct pbc_env * env, struct pbc_slice * slice) {
	int url_n, i;
	struct pd_univec *vec = pd_univec_new();
	struct pbc_rmessage * m = pbc_rmessage_new(env, "url.Urls", slice);
	if (!m) {
		error("Failed at reading message");
		return NULL;
	}

	// get the total number of url records,
	// and then iteratre over them.
	url_n = pbc_rmessage_size(m, "Url");
	for (i = 0; i < url_n; ++i) {
		struct pd_url * pu = (struct pd_url *)malloc(sizeof(*pu));
		struct pbc_rmessage *u = pbc_rmessage_message(m, "Url", i);
		pu->link = (char *)pbc_rmessage_string(u, "link", i, NULL);
		pu->added = pbc_rmessage_integer(u, "added", i, NULL);
		pu->date = pbc_rmessage_integer(u, "date", i, NULL);
		pu->hash = pbc_rmessage_integer(u, "hash", i, NULL);
		pd_univec_add(vec, pu, pd_url_cmp);
		pbc_rmessage_delete(u);
	}
	pbc_rmessage_delete(m);

	return vec;
}

// compare two url
int pd_url_cmp(void *url1, void *url2) {
	struct pd_url *u1 = (struct pd_url *)url1;
	struct pd_url *u2 = (struct pd_url *)url2;
	return strcmp(u1->link, u2->link);
}
