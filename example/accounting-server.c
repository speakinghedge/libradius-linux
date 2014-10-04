/*
 * example RADIUS accounting server
 *
 * just dump the message and respond with an empty accounting response message
 *
 * !!! only messages from local clients are accepted !!!
 *
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <radlib.h>
#include <sys/socket.h>
#include <sys/select.h>

#define RAD_SRV_SOCKET	1813	/* accounting server (auth uses 1812) */
#define RAD_SRV_SECRET	"1234"

volatile int srv_run;

void sig_handler(int sig) {
	srv_run = 0;
	printf("shutdown server\n");
	fflush(stdout);
}

int _dump_message(struct rad_handle *rad_h) {

	int attr_type = 0;
	void *data = NULL;
	size_t data_len = 0;
	uint32_t vendor = 0;
	int c = 0;
	int i = 0;

	printf("----dump message----\n");

	while((attr_type = rad_get_attr(rad_h, (const void **) &data, &data_len)) > 0) {

		printf("attr pos: %d\n", c++);
		if (attr_type == RAD_VENDOR_SPECIFIC) {
			printf("attr-type: 26 (vendor specific)\n");
     		attr_type = rad_get_vendor_attr(&vendor, &data, &data_len);
     		printf("vendor-id: %d\n", vendor);
     		printf("vendor-attr-type: %d\n", attr_type);
		} else {
			printf("attr-type: %d\n", attr_type);
		}
		printf("attr-data-len: %zu\n", data_len);
		printf("attr-data:\n");
		for (i = 0; i < data_len; i++) {
			printf("%02x%c", ((unsigned char*)data)[i] & 0xff, !((i+1) % 16)?'\n':' ');
		}
		if ((i - 1) % 15) {
			printf("\n");
		}
	}
}

int main() {

	struct rad_handle *rad_h = NULL;
	int srv_sock = -1;
	int opt = 1;
	struct sockaddr_in sin;

	fd_set readfds, conffds;
	struct timeval tv, tv_conf;
	
	signal(SIGINT, &sig_handler);

	if ((srv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		fprintf(stderr, "failed to create socket: %s\n", strerror(errno));
		return -1;
	}

	if (setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		fprintf(stderr, "failed to set socket option SO_REUSEADDR: %s\n", strerror(errno));
		close(srv_sock);
		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(RAD_SRV_SOCKET);
	if (bind(srv_sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1) {
		fprintf(stderr, "failed to bind socket: %s\n", strerror(errno));
		close(srv_sock);
		return -1;
	}

    if (!(rad_h = rad_server_open(srv_sock))) {
    	fprintf(stderr, "failed to open server.\n");
		close(srv_sock);
		return -1;	
    }

    if (rad_add_server(rad_h, "127.0.0.1", 0, RAD_SRV_SECRET, 5, 5) < 0) {
    	fprintf(stderr, "failed to add server: %s\n", rad_strerror(rad_h));
		close(srv_sock);
		return -1;	
    }

	tv_conf.tv_sec = 1;
	tv_conf.tv_usec = 0;
	FD_ZERO(&conffds);
	FD_SET(srv_sock, &conffds);

	printf("ctrl+c to terminate server\n");

    srv_run = 1;
    while(srv_run) {
    	tv = tv_conf;
    	readfds = conffds;

    	switch(select(srv_sock + 1, &readfds, NULL, NULL, &tv)) {
    		case 0:
    			break;
    		case 1:
    			if (rad_receive_request(rad_h) < 0) {
    				fprintf(stderr, "invalid request received: %s\n", rad_strerror(rad_h));
    			} else {
    				_dump_message(rad_h);
    			}
    			rad_create_response(rad_h, RAD_ACCOUNTING_RESPONSE);
    			rad_send_response(rad_h);

    			break;
    		default:
    			if (srv_run) {
    				fprintf(stderr, "something bad happened: %s\n", strerror(errno));
    				close(srv_sock);
    				return -1;
    			}
    	}
	}
	rad_close(rad_h);
    close(srv_sock);

	return 0;
}
