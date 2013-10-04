#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pd-url.h"
#include "pd-univec.h"

#define MAX_PAGE 100
void usage() {
	printf("Usage: pd-discover [--max_page_num=<num>]\n");
}

int main(int argc, char * argv[]) {
	int max_page_num;
	if (argc > 2) {
		usage();
		exit(0);
	}

	if (argc == 1) {
		max_page_num = MAX_PAGE;
	} else {
		if (strncmp(argv[1], "--max_page_num=", strlen("--max_page_num="))) {
				sprintf(stderr, "unknown argument %s\n", argv[1]);
				usage();
				exit(0);
			}
		max_page_num = atoi(strchr(argv[1], '=')+1);
	}

	// first set up pbc
	struct pbc_slice slice;
	pd_url_read_pb(PD_PB_FILENAME, &slice);
	if (slice.buffer == NULL)
		return -1;
	struct pbc_env * env = pbc_new();
	int r = pbc_register(env, &slice);
	if (r) {
		sprintf(stderr, "failed at pbc_register: %s\n", pbc_error(env));
		return 1;
	}
	struct pd_univec * msg_vec = pd_url_rmessage(env, &slice);

  // message is loaded up, let's get started
	pd_url_pages(max_page_num, msg_vec);

	// write back the updated messages
	pd_url_wmessage(env, &slice, msg_vec);
	pd_url_write_pb(PD_PB_FILENAME, &slice);

	// delete resources, though unnecessary
	pbc_delete(env);
	pd_univec_delete(msg_vec);
	return 0;
}
