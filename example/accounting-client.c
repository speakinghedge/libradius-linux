/*
 * example RADIUS client
 *
 * send a message to a local RADIUS accounting server
 *
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <radlib.h>
#include <sys/socket.h>
#include <sys/select.h>

#define RAD_SRV_SOCKET	1813	/* accounting server (auth uses 1812) */
#define RAD_SRV_SECRET	"1234"
#define RAD_VENDOR_ID	123456

int main() {

	struct rad_handle *rad_h = NULL;
	int rc = 0;
	
	if (!(rad_h = rad_acct_open())) {
		fprintf(stderr, "failed to create radius lib handle\n");
		return -1;	
	}

	if (rad_add_server(rad_h, "127.0.0.1", 0, RAD_SRV_SECRET, 5, 5) < 0) {
    	fprintf(stderr, "failed to add server: %s\n", rad_strerror(rad_h));
		return -1;	
    }

    if (rad_create_request(rad_h, RAD_ACCOUNTING_REQUEST)) {
    	fprintf(stderr, "failed to add server: %s\n", rad_strerror(rad_h));
		return -1;	
    }
	
    rad_put_string(rad_h, RAD_USER_NAME, "abcdef");
    rad_put_int(rad_h, RAD_NAS_PORT, 4223);
    rad_put_vendor_int(rad_h, RAD_VENDOR_ID, 1, 1749);
    rad_put_vendor_string(rad_h, RAD_VENDOR_ID, 2, "ghijklm");

    switch((rc = rad_send_request(rad_h))) {
    case RAD_ACCOUNTING_RESPONSE:
    	printf("server response okay.\n");
    	rc = 0;
    	break;
    case -1:
    	fprintf(stderr, "error while receiving response: %s\n", rad_strerror(rad_h));
    	break;
	default:
		fprintf(stderr, "invalid message type in response: %d\n", rc);
		rc = -1;
    }

    rad_close(rad_h);

    return rc;
}
