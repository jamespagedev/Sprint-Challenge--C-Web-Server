#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lib.h"

#define BUFSIZE 4096 // max number of bytes we can get at once

/**
 * Struct to hold all three pieces of a URL
 */
typedef struct urlinfo_t {
  char *hostname;
  char *port;
  char *path;
} urlinfo_t;

/**
 * Tokenize the given URL into hostname, path, and port.
 *
 * url: The input URL to parse.
 *
 * Store hostname, path, and port in a urlinfo_t struct and return the struct.
*/
urlinfo_t *parse_url(char *url)
{
  // copy the input URL so as not to mutate the original
  char *hostname = strdup(url);
  char *port;
  char *path;

  printf("hostname_before = %s\n", hostname);

  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));

  // We can parse the input URL by doing the following:

  /* Hint:
  You can use the `strchr` function to look for specific characters in a string.
  You can also use the `strstr` function to look for specific substrings in a string.
  */
  // 1. Use strchr to find the first slash in the URL (this is assuming there is no http:// or https:// in the URL).
  path = strchr(hostname, '/');
  printf("path1 = %s\n", path);
  // 2. Set the path pointer to 1 character after the spot returned by strchr.
  path++;
  printf("path2 = %s\n", path);
  // 3. Overwrite the slash with a '\0' so that we are no longer considering anything after the slash.
  *(path - 1) = '\0';
  printf("hostname_after_path_removal = %s\n", hostname);
  // 4. Use strchr to find the first colon in the URL.
  port = strchr(hostname, ':');
  printf("port1 = %s\n", port);
  // 5. Set the port pointer to 1 character after the spot returned by strchr.
  port++;
  printf("port2 = %s\n", port);
  // 6. Overwrite the colon with a '\0' so that we are just left with the hostname.
  *(port - 1) = '\0';
  printf("hostname_after_port_removal = %s\n\n", hostname);

  ///////////////////
  // IMPLEMENT ME! //
  ///////////////////
  urlinfo->hostname = hostname;
  urlinfo->port = port;
  urlinfo->path = path;

  return urlinfo;
}

/**
 * Constructs and sends an HTTP request
 *
 * fd:       The file descriptor of the connection.
 * hostname: The hostname string.
 * port:     The port string.
 * path:     The path string.
 *
 * Return the value from the send() function.
*/
int send_request(int fd, char *hostname, char *port, char *path)
{
  const int max_request_size = 16384;
  char request[max_request_size];

  ///////////////////
  // IMPLEMENT ME! //
  ///////////////////
  /* Construct the HTTP request.
  Just like in the web server, use sprintf in order to construct the request from the hostname, port, and path.
  Requests should look like the following:

  GET /path HTTP/1.1
  Host: hostname:port
  Connection: close

  The connection should be closed, otherwise some servers will simply hang and not return a response,
  since they're expecting more data from our client.
  */
  int request_length = sprintf(request,
          "GET /%s HTTP/1.1\n"    // /path
          "Host: %s:%s\n"         // hostname:port
          "Connection: close\n"   // close or keep-alive
          "\n",                   // closes the header in the response
          path,
          hostname,
          port
          );

  // Send the request string down the socket.
  //    Hopefully that's pretty self-explanatory.
  int rv = send(fd, request, request_length, 0);

  if (rv < 0)
  {
    perror("send");
    exit(1);
  }

  return rv;
}

int main(int argc, char *argv[])
{
  int sockfd, numbytes;
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }
  char *url = argv[1];

  // 1. Parse the input URL
  /*
    Implement the parse_url() function, which receives the input URL and tokenizes it into hostname, port, and
    path strings. Assign each of these to the appropriate field in the urlinfo_t struct and return it from the
    parse_url() function.
  */
  urlinfo_t *urlinfo = parse_url(url);
  printf("urlinfo->hostname = %s\n", urlinfo->hostname);
  printf("urlinfo->port = %s\n", urlinfo->port);
  printf("urlinfo->path = %s\n", urlinfo->path);
  printf("url = %s\n\n", url); // ensure the original url string is still in tact and not modified
  // 2. Initialize a socket by calling the `get_socket` function from lib.c
  // utilize sockfd initialized for you above
  sockfd = get_socket(urlinfo->hostname, urlinfo->port);
  // 3. Call `send_request` to construct the request and send it
  send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);
  // 4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
  /*
    Receive the response from the server and print it to stdout.
    The main hurdle that needs to be overcome when receiving data from a server is that we have no idea how large of
    a response we're going to get back. So to overcome this, we'll just keep calling `recv`, which will return back data
    from the server up to a maximum specified byte length on each iteration. We'll just continue doing this in a loop
    until `recv` returns back no more data from the server:
    https://pubs.opengroup.org/onlinepubs/009695399/functions/recv.html
    https://stackoverflow.com/questions/30667176/string-used-in-recv-does-not-printf-as-it-should
    https://stackoverflow.com/questions/3074824/reading-buffer-from-socket
  */
  while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) {
    // print the data we got back to stdout
    printf("%s\n", buf);
  }
  // 5. Clean up any allocated memory and open file descriptors.
  /*
  Don't forget to free any allocated memory and close any open file descriptors.
  */
  free(urlinfo->path);
  urlinfo->path = NULL;
  free(urlinfo->port);
  urlinfo->port = NULL;
  free(urlinfo->hostname);
  urlinfo->hostname = NULL;
  free(urlinfo);
  urlinfo = NULL;

  close(sockfd);
  sockfd = 0;

  ///////////////////
  // IMPLEMENT ME! //
  ///////////////////

  return 0;
}
