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

#include "common.h"

#include "cldd.h"
#include "client.h"
#include "cmdline.h"
#include "conf.h"
#include "daemon.h"
#include "error.h"
#include "log.h"
#include "server.h"
#include "stream.h"
#include "utils.h"

/* function prototypes */
void signal_handler (int sig);
void * client_manager (gpointer data);
static void process_events (server *s);

GMainLoop *main_loop;
GThread *main_task;

struct options options;
bool running = true;

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
    GError *error;
    server *s;

    if (argc == 1)
        usage (argv);

    success = parse_cmdline (argc, argv, &options);
    if (!success)
        CLDD_ERROR("Error while parsing command line arguments\n");

    /* setup signal handling first */
    signal (SIGHUP,  signal_handler);
    signal (SIGINT,  signal_handler);
    signal (SIGTERM, signal_handler);
    signal (SIGQUIT, signal_handler);

    /* glib-2.32 complains if you don't initialize the right way */
#ifdef HAVE_GLIB232
    g_type_init ();
#else
    g_thread_init (NULL);
#endif

    /* allocate memory for server data */
    s = server_new ();
    s->port = options.port;

    /* lock process into memory to prevent swapping */
    if ((s->rt = rt_memlock ()) == false)
        CLDD_MESSAGE("rt_memlock failed, real time not enabled");

    /* check whether or not high resolution timers are enabled */
    if ((s->rt = rt_hrtimers ()) == false)
        CLDD_MESSAGE("rt_hrtimers failed, real time not enabled");

    daemonize_close_stdin ();

    glue_daemonize_init (&options);
    stat_log_new (s, &options);

    daemonize_set_user ();

    /* passing true starts daemon in detached mode */
    daemonize (options.daemon);

    /* TODO: setup hardware data and threads */
    /* TODO: setup logging data and threads */
    /* TODO: setup control data and threads */

    stat_log_setup_output (s);

    /* start the master thread for client management */
    main_task = g_thread_create ((GThreadFunc)client_manager,
                                 (gpointer)s, true, &error);

    /* enter the main loop */
    main_loop = g_main_loop_new (NULL, false);
    g_main_loop_run (main_loop);
    g_thread_join (main_task);
    g_main_loop_unref (main_loop);

    daemonize_finish ();
    stat_log_file_close (s);

    /* clean up */
    server_free (s);

    return EXIT_SUCCESS;
}

/**
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
            /* this signal causes an errno of EINTR on epoll_pwait to exit
             * the socket wait loop */
            break;
        case SIGTERM:
            syslog (LOG_WARNING, "Received SIGTERM signal.");
            running = false;
            break;
        case SIGINT:
            syslog (LOG_WARNING, "Received SIGINT signal.");
            running = false;
            break;
        default:
            syslog (LOG_WARNING, "Unhandled signal (%d) %s", strsignal(sig));
            break;
    }
}

/**
 * Thread function to manage client connections.
 *
 * @param data Thread data for the function
 */
void *
client_manager (gpointer data)
{
    int i, n, ret, nready;
    const int on = 1;
    struct sockaddr_in servaddr;
    node *link = NULL;
    client *c = NULL;
    node *n = NULL;
    ///

    /* create TCP socket to listen for client connections */
    CLDD_MESSAGE("Creating TCP socket");
    s->fd = socket (AF_INET, SOCK_STREAM, 0);
    if (s->fd < 0)
        CLDD_ERROR("Socket creation");

    /* set the socket to allow re-bind without wait issues */
    setsockopt (s->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (int));
    set_nonblocking (s->fd);

    /* make the server socket non-blocking */
    set_nonblocking (s->fd);

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port        = htons (s->port);

    server *s = (server *)data;
>>>>>>> 5964528... Modified select to use GList

    /* setup the socket for incoming connections */
    if (listen (s->fd, BACKLOG) < 0)
        CLDD_ERROR("Unable to listen on socket %ld", s->fd);

    /* set the server up to use epoll */
    server_init_epoll (s);

    for (;;)
    {
        /* get the client data ready */
        c = client_new ();
        c->sa_len = sizeof (c->sa);

        s->num_fds = epoll_wait (s->epoll_fd, s->events, EPOLL_QUEUE_LEN, -1);
        if (s->num_fds < 0)
            CLDD_ERROR("Error while epoll_wait()");

        for (i = 0; i < s->num_fds; i++)
        {
            /* error */
            if (s->events[i].events & (EPOLLHUP | EPOLLERR))
            {
                CLDD_MESSAGE("epoll: EPOLLERR");
                close(s->events[i].data.fd);
                continue;
            }
            assert (s->events[i].events & EPOLLIN);

            // receiving a connection request
            if (s->events[i].data.fd == s->fd)
            {
                CLDD_MESSAGE("Waiting for a new client connection");
                /* blocking call waiting for connections */
                c->fd = accept (s->fd, (struct sockaddr *) &c->sa, &c->sa_len);
                if (c->fd == -1)
                {
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                        CLDD_ERROR("accept");
                    continue;
                }
                CLDD_MESSAGE("Received connection from (%s, %d)",
                             inet_ntoa (c->sa.sin_addr),
                             ntohs (c->sa.sin_port));

                /* make the new fd non-blocking */
                set_nonblocking (c->fd);

                // add the new socket descriptor to the epoll loop
                s->event.data.fd = c->fd;
                if (epoll_ctl (s->epoll_fd, EPOLL_CTL_ADD, c->fd, &s->event) == -1)
                    CLDD_ERROR("epoll_ctl() error");

                /* launch the thread for the client */
                pthread_mutex_lock (&s->server_data_lock);
                s->n_clients++;
                s->n_max_connected = (s->n_clients > s->n_max_connected)
                                    ? s->n_clients : s->n_max_connected;
                pthread_mutex_unlock (&s->server_data_lock);

            c = (client *)n->data;
            FD_SET(c->fd, &s->fds);
            if (c->fd > s->maxfd)
                s->maxfd = c->fd;
            it = next;
        }

        /* check for socket requests */
        nready = select (s->maxfd + 1, &s->fds, NULL, NULL, NULL);

        pthread_mutex_unlock (&s->data_lock);
>>>>>>> 5534035... Working select implementation

                continue;
            }

            /* retrieve the client with fd == event fd */
            link = s->client_list->link;
            while (link != NULL)
            {
                if (((client *)(link->data))->fd == c->fd)
                {
                    c = (client *)link->data;
                    break;
                }
                link = link->next;
            }

            /* read messages from client */
        else if (s->num_fds == 0)
            continue;
        else
            /* data is available on one or more sockets */
            process_events (s);
>>>>>>> 8d223d3... Mostly functional output stream
    }

    CLDD_MESSAGE("Exiting the client manager");

    /* close all clients */
    server_close_clients (s);
    close (s->fd);

    g_main_loop_quit (main_loop);
    g_thread_exit (NULL);
}

/**
 *
 */
void
read_fds (server *s)
{
    int ret;
    client *c = NULL;
    GList *it, *next;

    /* check if a client is trying to connect */
    ret = pthread_mutex_trylock (&s->data_lock);
    if (FD_ISSET(s->fd, &s->fds))
    {
        /* get the client data ready */
        c = client_new ();
        c->sa_len = sizeof (c->sa);

        /* blocking call waiting for connections */
        c->fd = accept (s->fd, (struct sockaddr *) &c->sa, &c->sa_len);
        set_nonblocking (c->fd);
        CLDD_MESSAGE("Received connection from (%s, %d)",
                     inet_ntoa (c->sa.sin_addr),
                     ntohs (c->sa.sin_port));

        s->n_clients++;
        s->n_max_connected = (s->n_clients > s->n_max_connected) ?
                              s->n_clients : s->n_max_connected;

        /* add the client data to the linked list */
        s->client_list = g_list_append (s->client_list, (gpointer)c);
        CLDD_MESSAGE("Added client to list, new size: %d",
                     g_list_length (s->client_list));
        c = NULL;
    }
    pthread_mutex_unlock (&s->data_lock);

    /* go through the available connections */
    it = s->client_list;
    while (it != NULL)
    {
        c = (client *)it->data;
        next = g_list_next (it);

        /* process any request that caused an event */
        pthread_mutex_trylock (&s->data_lock);
        if (FD_ISSET(c->fd, &s->fds))
        {
            /* unlock client thread condition variable */
            CLDD_MESSAGE("received cmd on %d", c->fd);
            pthread_mutex_lock (&c->lock);
            c->data_ready = true;
            pthread_cond_signal (&c->ready);
            pthread_mutex_unlock (&c->lock);
        }
        pthread_mutex_unlock (&s->server_data_lock);
    }
}

/**
 * client_func
 *
 * Function to test client connections.
 *
 * @param data The data containing the client and server references
 **/
void *
client_func (void *data)
{
    ssize_t n;

                close (c->fd);
                client_free (c);
                c = NULL;
            }
=======
            /* check if the client is pending quit */
            if (c->quit)
            {
                g_mutex_trylock (&s->data_lock);
                s->n_clients--;
                s->client_list = g_list_delete_link (s->client_list, it);
                CLDD_MESSAGE("Removed client from list, new size: %d",
                             g_list_length (s->client_list));
                /* log the client stats before closing it */
                //fprintf (s->statsfp, "%s, %d, %d, %d\n",
                //         c->hbuf, c->fd_mgmt, c->nreq, c->ntot);
                g_mutex_unlock (&s->data_lock);

                if (c->stream->open)
                    stream_close (c->stream);

                close (c->fd_mgmt);
                client_free (c);
                c = NULL;
            }
>>>>>>> bf9f481... Updated calls for server setup
        }
        else if (strcmp (c->msg, "quit\n") == 0)
        {
            pthread_mutex_unlock (&master_lock);
            c->nreq++;
            break;
        }

        /* message handled, reset */
        c->msg_pending = false;

        pthread_mutex_unlock (&c->msg_lock);
    }
    CLDD_MESSAGE("[%5d] Leaving client loop", c->fd);

    pthread_mutex_lock (&s->server_data_lock);
    s->n_clients--;
    s->client_list = llist_remove (s->client_list, (void *)c, client_compare);
    CLDD_MESSAGE("[%5d] Removed client from list, new size: %d",
                 c->fd, llist_length (s->client_list));
    pthread_mutex_unlock (&s->server_data_lock);
    /* log the client stats before closing it */
    fprintf (s->statsfp, "%s, %d, %d, %d\n",
             inet_ntoa (c->sa.sin_addr), c->fd, c->nreq, c->ntot);
    close (c->fd);
    llist_remove (s->client_list, c, client_compare);
//    client_free (c);
    c = NULL;

    pthread_exit (NULL);
}

/**
 * monitor the client request queue and spawn client threads as necessary
 */
void *
client_spawn_handler (void *data)
{
    server *s = (server *)data;
    client *c = NULL;
//    struct client_data_t cd;
//    struct client_data_t *pcd;

//    for (;;)
//    {
//        usleep (1000);

//        pthread_mutex_lock (&s->spawn_queue_lock);

//        CLDD_MESSAGE("Spawning clients");
        /* if any client requests were made spawn a new thread */
//        while (queue_is_empty (s->spawn_queue))
//            pthread_cond_wait (&s->spawn_queue_ready, &s->spawn_queue_lock);

        /* pop the next node in the queue */
//        s->spawn_queue = queue_dequeue (s->spawn_queue, (void **)&c);
//        pthread_mutex_unlock (&s->spawn_queue_lock);

        /* launch the thread for the client */
//        pthread_mutex_lock (&s->server_data_lock);
//        s->n_clients++;
//        s->n_max_connected = (s->n_clients > s->n_max_connected)
//                             ? s->n_clients : s->n_max_connected;
//        pthread_mutex_unlock (&s->server_data_lock);
//        cd.s = s;
//        cd.c = c;
//        pcd = &cd;
//        pthread_create (&c->tid, NULL, client_func, pcd);
//        CLDD_MESSAGE("Create client thread %ld", c->tid);

//        pthread_mutex_lock (&s->server_data_lock);
//        s->client_list = llist_append (s->client_list, (void *)c);
//        CLDD_MESSAGE("Added client to list, new size: %d",
//                     llist_length (s->client_list));
//        pthread_mutex_unlock (&s->server_data_lock);
//    }

    pthread_exit (NULL);
}

/**
 * wait for new client connection requests
 */
void *
client_queue_handler (void *data)
{
    int nready;
    server *s = (server *)data;
    client *c = NULL;

//    FD_ZERO(&s->rset);
//    s->maxfd = s->fd;

//    for (;;)
//    {
//        usleep (1000);

        /* get the client data ready */
//        c = client_new ();
//        c->sa_len = sizeof (c->sa);

//        FD_SET(s->fd, &s->rset);
//        if ((nready = select (s->maxfd + 1, &s->rset, NULL, NULL, NULL)) < 0)
//        {
//            if (errno == EINTR)
//                continue;
//            else
//                CLDD_ERROR("select() error");
//        }

//        if (FD_ISSET(s->fd, &s->rset))
//        {
            /* blocking call waiting for connections */
//            CLDD_MESSAGE("Waiting for a new client connection");
//            c->fd = accept (s->fd, (struct sockaddr *) &c->sa, &c->sa_len);
//            CLDD_MESSAGE("Received connection from (%s, %d)",
//                        inet_ntoa (c->sa.sin_addr), ntohs (c->sa.sin_port));

            /* queue up the new connection */
//            pthread_mutex_lock (&s->spawn_queue_lock);
//            s->spawn_queue = queue_enqueue (s->spawn_queue, (void *)c);
//            pthread_mutex_unlock (&s->spawn_queue_lock);
            /* don't need to signal if a queue exists */
//            if (queue_size (s->spawn_queue) == 1)
//                pthread_cond_signal (&s->spawn_queue_ready);
//        }

//        c = NULL;
//    }

    pthread_exit (NULL);
}
