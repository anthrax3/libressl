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

// For this example, we'll be testing on openssl.org
#define SERVER  "www.openssl.org"
#define PORT 443

// Establish a regular tcp connection
int tcp_connect()
{
	int error, handle;
	struct hostent *host;
	struct sockaddr_in server;

	host = gethostbyname(SERVER);
	handle = socket(AF_INET, SOCK_STREAM, 0);
	if (handle == -1) {
		perror("Socket");
		handle = 0;
	} else {
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr = *((struct in_addr *)host->h_addr);
		bzero(&(server.sin_zero), 8);

		error = connect(handle, (struct sockaddr *)&server, sizeof(struct sockaddr));
		if (error == -1) {
			perror("Connect");
			handle = 0;
		}
	}

	return handle;
}

// Establish a connection using an SSL layer
connection *ssl_connect(void)
{
	connection *c;

	c = malloc(sizeof(connection));
	c->ssl_handle = NULL;
	c->ssl_context = NULL;

	c->socket = tcp_connect();
	if (c->socket) {
		// Register the error strings for libcrypto & libssl
		SSL_load_error_strings();
		// Register the available ciphers and digests
		SSL_library_init();

		// New context saying we are a client, and using SSL 2 or 3
		c->ssl_context = SSL_CTX_new(SSLv23_client_method());
		if (c->ssl_context == NULL)
			ERR_print_errors_fp(stderr);

		// Create an SSL struct for the connection
		c->ssl_handle = SSL_new(c->ssl_context);
		if (c->ssl_handle == NULL)
			ERR_print_errors_fp(stderr);

		// Connect the SSL struct to our connection
		if (!SSL_set_fd(c->ssl_handle, c->socket))
			ERR_print_errors_fp(stderr);

		// Initiate SSL handshake
		if (SSL_connect(c->ssl_handle) != 1)
			ERR_print_errors_fp(stderr);
	} else {
		perror("Connect failed");
	}

	return c;
}

// Disconnect & free connection struct
void ssl_disconnect(connection * c)
{
	if (c->socket)
		close(c->socket);
	if (c->ssl_handle) {
		SSL_shutdown(c->ssl_handle);
		SSL_free(c->ssl_handle);
	}
	if (c->ssl_context)
		SSL_CTX_free(c->ssl_context);

	free(c);
}

// Read all available text from the connection
char *ssl_read(connection * c)
{
	const int readSize = 1024;
	char *rc = NULL;
	int received, count = 0;
	char buffer[1025];

	if (c) {
		while (1) {
			if (!rc)
				rc = malloc(readSize * sizeof(char) + 1);
			else
				rc = realloc(rc, (count + 1) * readSize * sizeof(char) + 1);

			received = SSL_read(c->ssl_handle, buffer, readSize);
			buffer[received] = '\0';

			if (received > 0)
				strcat(rc, buffer);

			if (received < readSize)
				break;
			count++;
		}
	}

	return rc;
}

// Write text to the connection
void ssl_write(connection * c, char *text)
{
	if (c)
		SSL_write(c->ssl_handle, text, strlen(text));
}

// Very basic main: we send GET / and print the response.
int main(int argc, char **argv)
{
	connection *c;
	char *response;

	c = ssl_connect();

	ssl_write(c, "GET /\r\n\r\n");
	response = ssl_read(c);

	printf("%s\n", response);

	ssl_disconnect(c);
	free(response);

	return 0;
}
