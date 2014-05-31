/*
 * Simple test client from http://savetheions.com/2010/01/16/quickly-using-openssl-in-c/
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Simple structure to keep track of the handle, and
// of what needs to be freed later.
typedef struct {
	int socket;
	SSL *ssl_handle;
	SSL_CTX *ssl_context;
} connection;

// For this example, we'll be testing on www.openssl.org
#define SERVER  "www.openssl.org"
#define HOST "www.openssl.org"
#define PORT 443

// Establish a regular tcp connection
int tcp_connect()
{
	struct hostent *host = gethostbyname(SERVER);
	if (!host) {
		return -1;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Socket");
		return -1;
	}

	struct sockaddr_in server = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr = *((struct in_addr *)host->h_addr),
		.sin_zero = { 0 }
	};

	int error = connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr));
	if (error == -1) {
		close(sock);
		perror("Connect");
		return -1;
	}

	return sock;
}

// Establish a connection using an SSL layer
connection *ssl_connect(void)
{
	connection *c = calloc(1, sizeof(connection));

	c->socket = tcp_connect();
	if (c->socket < 0) {
		free(c);
		return NULL;
	}
	
	// New context saying we are a client, and using SSL 2 or 3
	c->ssl_context = SSL_CTX_new(SSLv23_client_method());
	if (c->ssl_context == NULL) {
		ERR_print_errors_fp(stderr);
	}

	// Create an SSL struct for the connection
	c->ssl_handle = SSL_new(c->ssl_context);
	if (c->ssl_handle == NULL) {
		ERR_print_errors_fp(stderr);
	}

	// Connect the SSL struct to our connection
	if (!SSL_set_fd(c->ssl_handle, c->socket)) {
		ERR_print_errors_fp(stderr);
	}

	// Initiate SSL handshake
	if (SSL_connect(c->ssl_handle) != 1) {
		ERR_print_errors_fp(stderr);
	}

	return c;
}

// Disconnect & free connection struct
void ssl_disconnect(connection * c)
{
	if (c->socket >= 0) {
		close(c->socket);
	}
	if (c->ssl_handle) {
		SSL_shutdown(c->ssl_handle);
		SSL_free(c->ssl_handle);
	}
	if (c->ssl_context) {
		SSL_CTX_free(c->ssl_context);
	}

	free(c);
}

// Read all available text from the connection
char *ssl_read(connection * c)
{
	char *rc = NULL;
	int received, rc_len = 0, off = 0;

	do {
		rc_len += 1024;
		rc = realloc(rc, rc_len);

		received = SSL_read(c->ssl_handle, rc + off, 1024);

		if (received > 0) {
			off += received;
		}

	} while (received == 1024);

	rc[off] = '\0';
	return rc;
}

// Write text to the connection
void ssl_write(connection * c, char *text)
{
	SSL_write(c->ssl_handle, text, strlen(text));
}

// Very basic main: we send GET / and print the response.
int main(int argc, char **argv)
{
	// Register the error strings for libcrypto & libssl
	SSL_load_error_strings();
	// Register the available ciphers and digests
	SSL_library_init();

	connection *c = ssl_connect();
	if (c) {
		ssl_write(c, "GET /\r\nHost: " HOST "\r\n\r\n");

		char *response = ssl_read(c);
		printf("%s\n", response);
		free(response);

		ssl_disconnect(c);
	}

	return 0;
}
