/**
 * Copyright (C) 2010 Geoff Johnson <geoff.jay@gmail.com>
 *
 * This file is part of cldd.
 *
 * cldd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <common.h>

#include "cldd.h"
#include "client.h"
#include "cmdline.h"
#include "conf.h"
#include "daemon.h"
#include "error.h"
#include "server.h"
#include "utils.h"

/* replace later with value taken from configuration file */
#define PID_FILE    "/var/run/cldd.pid"

/* function prototypes */
void signal_handler (int sig);
void * client_manager (void *data);
void * client_thread (void *data);

pthread_t master_thread;
struct options options;

static void
glue_daemonize_init (const struct options *options)
{
    daemonize_init ("root", "root", PID_FILE);
    if (options->kill)
        daemonize_kill ();
}

int
main (int argc, char **argv)
{
    int ret;
    bool success;
    server *s;

    if (argc == 1)
        usage (argv);
    success = parse_cmdline (argc, argv, &options);
    if (!success)
        CLDD_ERROR ("Error while parsing command line arguments\n");

    /* setup signal handling first */
    signal (SIGHUP,  signal_handler);
    signal (SIGINT,  signal_handler);
    signal (SIGTERM, signal_handler);
    signal (SIGQUIT, signal_handler);

    /* allocate memory for server data */
    s = server_new ();
    s->port = 8000;         /* port for client connections - read from cmdline later */

    daemonize_close_stdin ();

    glue_daemonize_init (&options);
    //log_init (options.verbose, options.log_stderr);

    daemonize_set_user ();

    /* passing true starts daemon in detached mode */
    daemonize (options.daemon);
    //setup_log_output (options.log_stderr);

    /* start the master thread for client management */
    ret = pthread_create (&master_thread, NULL, client_manager, s);
    if (ret != 0)
        CLDD_ERROR("Unable to create client management thread");
    pthread_join (master_thread, NULL);

    daemonize_finish ();
    //close_log_files ();

    /* clean up */
    server_free (s);

    return EXIT_SUCCESS;
}

/**
 * signal_handler
 *
 * Use syslog for now, when logging facilities are available should use those
 * instead.
 *
 * @param sig The signal received
 */
void
signal_handler (int sig)
{
    /* do something useful here later */
    switch (sig)
    {
        case SIGHUP:
            syslog (LOG_WARNING, "Received SIGHUP signal.");
            break;
        case SIGTERM:
            syslog (LOG_WARNING, "Received SIGTERM signal.");
            break;
        default:
            syslog (LOG_WARNING, "Unhandled signal (%d) %s", strsignal(sig));
            break;
    }

    /* stop the thread responsible for client management */
    pthread_cancel (master_thread);
}

/**
 * client_manager
 *
 * Thread function to manage client connections.
 *
 * @param data Thread data for the function
 */
void *
client_manager (void *data)
{
    int listenfd, udpfd;
    const int on = 1;
    struct sockaddr_in servaddr;

    server *s = (server *)data;

    /* create TCP socket to listen for client connections */
    CLDD_MESSAGE("Creating TCP socket");
    listenfd = socket (AF_INET, SOCK_STREAM, 0);

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port        = htons (s->port);

    setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
    listen (listenfd, BACKLOG);

    /* create UDP socket for clients */
//    CLDD_MESSAGE("Creating UDP socket");
//    udpfd = socket (AF_INET, SOCK_DGRAM, 0);

//    bzero (&servaddr, sizeof (servaddr));
//    servaddr.sin_family      = AF_INET;
//    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
//    servaddr.sin_port        = htons (s->port);

//    bind (udpfd, (struct sockaddr *) &servaddr, sizeof (servaddr));

    for (;;)
    {
        client *c = malloc (sizeof (client));

        c->sa_len = sizeof (c->sa);
        /* blocking call waiting for connections */
        CLDD_MESSAGE("Waiting for new connection");
        c->fd = accept (listenfd, (struct sockaddr *) &c->sa, &c->sa_len);
        CLDD_MESSAGE("Received connection from (%s , %d)",
                     inet_ntoa (c->sa.sin_addr), ntohs (c->sa.sin_port));

        /* launch the thread for the client */
        CLDD_MESSAGE("Creating client thread");
        pthread_create (&c->tid, NULL, client_thread, c);
        CLDD_MESSAGE("Done with thread create");
    }

    pthread_exit (NULL);
}

/**
 * client_thread
 *
 * Function to test client connections.
 *
 * @param sockfd The socket of the client
 **/
void *
client_thread (void *data)
{
    ssize_t n;
    char *recv;
    char send[MAXLINE] = "random text to use for testing write to client\n";
//    socklen_t len;
    client *c = (client *)data;

    recv = malloc (MAXLINE * sizeof (char));

    CLDD_MESSAGE("Entering client loop - sock fd = %d", c->fd);
    for (;;)
    {
//        len = c->sa_len;
//        n = recvfrom (c->fd, recv, MAXLINE, 0, (struct sockaddr *) &c->sa, &len);
//        recv[n] = '\0';

//        CLDD_MESSAGE ("Read %d chars on sock fd %d: %s", n, c->fd, recv);
//        if (strcmp (recv, "request\n") == 0)
//            sendto (c->fd, send, strlen (send), 0, (struct sockaddr *) &c->sa, c->sa_len);
//        else if (strcmp (recv, "quit\n") == 0)
//            break;

        /* blocking call to wait for client request */
        n = readline (c->fd, recv, MAXLINE);
        if (n == 0)
            break;

        CLDD_MESSAGE ("Read %d chars on sock fd %d: %s", n, c->fd, recv);
        if (strcmp (recv, "request\n") == 0)
        {
            if ((n = writen (c->fd, send, strlen (send))) != strlen (send))
                CLDD_MESSAGE("Client write error - %d != %d", strlen (send), n);
        }
        else if (strcmp (recv, "quit\n") == 0)
            break;
    }
    CLDD_MESSAGE("Leaving client loop");

    free (recv);
    pthread_exit (NULL);
}
