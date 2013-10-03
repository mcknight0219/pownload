#include "pd-url.h"
#include <stdio.h>
#include <stdlib.h>

struct pd_url {
	char * link;
	int added;
	int date;
	unsigned long hash;
};

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
	fread(slice->buffer, 1, slice_len, f);
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
	struct pbc_rmessage * m = pbc_rmesasge_new(env, "url.Urls", slice);
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
		pu->link = pbc_rmessage_string(u, "link", i, NULL);
		pu->added = pbc_rmessage_integer(u, "added", i, NULL);
		pu->date = pbc_rmessage_integer(u, "date", i, NULL);
		pu->hash = pbc_rmessage_integer(u, "hash", i, NULL);
		vec->add(vec, pu, pd_url_cmp);
		pbc_rmessage_delete(u);
	}
	pbc_rmessage_delete(m);

	return vec;
		
}
	
