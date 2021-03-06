/*
   Copyright (c) 2012, Stanislav Yakush(st.yakush@yandex.ru)
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the EagleMQ nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __EAGLE_H__
#define __EAGLE_H__

#include <stdint.h>

#include "event.h"
#include "network.h"
#include "list.h"
#include "keylist.h"
#include "queue.h"
#include "user.h"

#define EG_NOTUSED(X) ((void)X)

#define EG_STATUS_OK 0
#define EG_STATUS_ERR -1

#define EG_DEFAULT_ADDR	"127.0.0.1"
#define EG_DEFAULT_PORT 7851
#define EG_DEFAULT_ADMIN_NAME "eagle"
#define EG_DEFAULT_ADMIN_PASSWORD "eagle"
#define EG_DEFAULT_MAX_CLIENTS 16384
#define EG_DEFAULT_MAX_MEMORY 0
#define EG_DEFAULT_CLIENT_TIMEOUT 0
#define EG_DEFAULT_SAVE_TIMEOUT 0
#define EG_DEFAULT_DAEMONIZE 0
#define EG_DEFAULT_STORAGE_PATH "eaglemq.dat"
#define EG_DEFAULT_LOG_PATH "eaglemq.log"
#define EG_DEFAULT_CONFIG_PATH "eaglemq.conf"

#define EG_BUF_SIZE 32768

#define EG_MAX_BUF_SIZE 2147483647
#define EG_MAX_MSG_COUNT 4294967295
#define EG_MAX_MSG_SIZE 2147483647

#define EG_MEMORY_CHECK_TIMEOUT 10

#define BIT_SET(a, b) ((a) |= (1UL<<(b)))
#define BIT_CHECK(a, b) ((a) & (1UL<<(b)))

/* ------- Queue ------- */

typedef struct Queue_t {
	char name[64];
	uint32_t max_msg;
	uint32_t max_msg_size;
	uint32_t flags;
	int auto_delete;
	int force_push;
	int round_robin;
	Queue *queue;
	List *expire_messages;
	List *confirm_messages;
	List *declared_clients;
	List *subscribed_clients_msg;
	List *subscribed_clients_notify;
	Keylist *routes;
} Queue_t;

/* ------- Route ------- */

typedef struct Route_t {
	char name[64];
	uint32_t flags;
	int auto_delete;
	int round_robin;
	Keylist *keys;
} Route_t;

/* ------- Channel ------- */

typedef struct Channel_t {
	char name[64];
	uint32_t flags;
	int auto_delete;
	int round_robin;
	Keylist *topics;
	Keylist *patterns;
} Channel_t;

/* ------- Client context ------- */

typedef struct EagleClient {
	int fd;
	uint64_t perm;
	char *request;
	size_t length;
	size_t pos;
	int noack;
	int offset;
	char *buffer;
	size_t bodylen;
	size_t nread;
	List *responses;
	List *declared_queues;
	List *subscribed_queues;
	Keylist *subscribed_topics;
	Keylist *subscribed_patterns;
	size_t sentlen;
	time_t last_action;
} EagleClient;

/* ------- Server context ------- */

typedef struct EagleServer {
	EventLoop *loop;
	int fd;
	int sfd;
	long long ufd;
	char *addr;
	int port;
	char *unix_socket;
	mode_t unix_perm;
	pid_t child_pid;
	char *name;
	char *password;
	size_t max_clients;
	long long max_memory;
	int client_timeout;
	int storage_timeout;
	char error[NET_ERR_LEN];
	List *clients;
	List *users;
	List *queues;
	List *routes;
	List *channels;
	time_t now_time;
	time_t now_timems;
	time_t start_time;
	time_t last_save;
	time_t last_memcheck;
	int nomemory;
	int msg_counter;
	int daemonize;
	char *storage;
	char *pidfile;
	char *logfile;
	char *config;
	int shutdown;
} EagleServer;

extern EagleServer *server;

/* ------- Command ------- */

typedef void commandHandler(EagleClient *client);

extern commandHandler *commands[];

/* ------- Command handlers ------- */

void auth_command_handler(EagleClient *client);
void ping_command_handler(EagleClient *client);
void stat_command_handler(EagleClient *client);
void save_command_handler(EagleClient *client);
void flush_command_handler(EagleClient *client);
void disconnect_command_handler(EagleClient *client);

void user_create_command_handler(EagleClient *client);
void user_list_command_handler(EagleClient *client);
void user_rename_command_handler(EagleClient *client);
void user_set_perm_command_handler(EagleClient *client);
void user_delete_command_handler(EagleClient *client);

void queue_create_command_handler(EagleClient *client);
void queue_declare_command_handler(EagleClient *client);
void queue_exist_command_handler(EagleClient *client);
void queue_list_command_handler(EagleClient *client);
void queue_rename_command_handler(EagleClient *client);
void queue_size_command_handler(EagleClient *client);
void queue_push_command_handler(EagleClient *client);
void queue_get_command_handler(EagleClient *client);
void queue_pop_command_handler(EagleClient *client);
void queue_confirm_command_handler(EagleClient *client);
void queue_subscribe_command_handler(EagleClient *client);
void queue_unsubscribe_command_handler(EagleClient *client);
void queue_purge_command_handler(EagleClient *client);
void queue_delete_command_handler(EagleClient *client);

void route_create_command_handler(EagleClient *client);
void route_exist_command_handler(EagleClient *client);
void route_list_command_handler(EagleClient *client);
void route_keys_command_handler(EagleClient *client);
void route_rename_command_handler(EagleClient *client);
void route_bind_command_handler(EagleClient *client);
void route_unbind_command_handler(EagleClient *client);
void route_push_command_handler(EagleClient *client);
void route_delete_command_handler(EagleClient *client);

void channel_create_command_handler(EagleClient *client);
void channel_exist_command_handler(EagleClient *client);
void channel_list_command_handler(EagleClient *client);
void channel_rename_command_handler(EagleClient *client);
void channel_publish_command_handler(EagleClient *client);
void channel_subscribe_command_handler(EagleClient *client);
void channel_psubscribe_command_handler(EagleClient *client);
void channel_unsubscribe_command_handler(EagleClient *client);
void channel_punsubscribe_command_handler(EagleClient *client);
void channel_delete_command_handler(EagleClient *client);

#endif
