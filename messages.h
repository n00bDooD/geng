#ifndef MESSAGES_H
#define MESSAGES_H
#include <stdlib.h>

typedef struct msgq_state msgq_state;

typedef void (*msgq_listener)(void* me, void* source, void* data);
typedef void* (*msgq_handler)(void* me, void* source, void* argument);

void msgq_broadcast(msgq_state*, void* me, const char* what, void* data);
void msgq_listen(msgq_state*, void* me, const char* what, msgq_listener listener);

void msgq_request(msgq_state*, void* me, const char* what, void* argument, msgq_listener responder);
void msgq_serve(msgq_state*, void* me, const char* what, msgq_handler handler);

msgq_state* msgq_create(msgq_state* s,
  		        size_t broadcast_hashmap_len,
  		        size_t request_hashmap_len,
  		        size_t broadcast_buffer_length,
			size_t request_buffer_lenngth);

void msgq_free(msgq_state*);

void msgq_flush_all(msgq_state* s);

#endif /* MESSAGES_H */
