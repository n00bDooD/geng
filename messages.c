#include "messages.h"
#include "global.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <strings.h>

#include "city.h"

#define MAX_MESSAGETYPES 512
#define MAX_REQUESTTYPES 512


typedef uint64_t messageid;

#define MSGID_GUARD UINT64_MAX

#define broadcastid(s) (CityHash64(s, strlen(s)) % MAX_MESSAGETYPES)
#define requestid(s) (CityHash64(s, strlen(s)) % MAX_REQUESTTYPES)


typedef struct {
	msgq_listener func;
	void* identity;
} listener;

typedef struct {
	msgq_handler func;
	void* identity;
} handler;


typedef struct {
	messageid msgid;
	void* source;
	void* data;
} pending_broadcast;


#define request_evaluated(r) ((r).state & 1)
#define request_returned(r) ((r).state & 2)

typedef struct {
	messageid msgid;
	msgq_listener response;
	void* requester;
	void* requestee;
	void* arg;
	void* result;
	uint8_t state;
} pending_request;


struct msgq_state {
	listener** listeners;
	handler* handlers;

	pending_broadcast* pending_broadcasts;
	pending_request* pending_requests;
};

static inline void clear_pending_request(pending_request* r)
{
	r->msgid = 0;
	r->response = NULL;
	r->requester = NULL;
	r->requestee = NULL;
	r->arg = NULL;
	r->result = NULL;
	r->state = 0;
}

void fastforward_request(msgq_state* state, pending_request* r);
void return_request(msgq_state* state, pending_request* r);
void evaluate_request(msgq_state* state, pending_request* r);
void dispatch_broadcast(msgq_state* state, pending_broadcast* b);

void fastforward_all_pending(msgq_state* state);
void return_all_pending(msgq_state* state);
void evaluate_all_pending(msgq_state* state);
void dispatch_all_pending(msgq_state* state);


/* # Memory management
 * ####################
 */

msgq_state* msgq_create(msgq_state* s, 
		        size_t broadcast_buffer_length,
			size_t request_buffer_length)
{
	if (s == NULL) {
		s = malloc(sizeof(msgq_state));
	}
	s->listeners = NULL;
	s->handlers = NULL;
	s->pending_broadcasts = NULL;
	s->pending_requests = NULL;

	s->listeners = malloc(sizeof(listener*) * MAX_MESSAGETYPES);
	for(int i = 0; i < MAX_MESSAGETYPES; ++i) { 
		s->listeners[i] = NULL;
	}
	s->handlers = malloc(sizeof(handler) * MAX_REQUESTTYPES);
	for(int i = 0; i < MAX_REQUESTTYPES; ++i) { s->handlers[i].func = NULL;
	                                            s->handlers[i].identity = NULL; }

	s->pending_broadcasts = malloc(sizeof(pending_broadcast) * broadcast_buffer_length);
	for(size_t i = 0; i < broadcast_buffer_length; ++i) {
		s->pending_broadcasts[i].msgid = 0;
		s->pending_broadcasts[i].source = NULL;
		s->pending_broadcasts[i].data = NULL;
	}
	s->pending_broadcasts[broadcast_buffer_length-1].msgid = MSGID_GUARD;

	s->pending_requests = malloc(sizeof(pending_request) * request_buffer_length);
	for(size_t i = 0; i < request_buffer_length; ++i) {
		clear_pending_request(s->pending_requests + i);
	}
	s->pending_requests[request_buffer_length-1].msgid = MSGID_GUARD;
	return s;
}

void msgq_reset(msgq_state* s)
{
	for(int i = 0; i < MAX_MESSAGETYPES; ++i) { 
		if (s->listeners[i] != NULL) {
			free(s->listeners[i]);
			s->listeners[i] = NULL;
		}
	}
	for(int i = 0; i < MAX_REQUESTTYPES; ++i) { 
		if (s->handlers[i].func != NULL) {
			s->handlers[i].func = NULL;
			s->handlers[i].identity = NULL;
		}
	}
#if 0
	free(s->pending_broadcasts);
	s->pending_broadcasts = NULL;
	// TODO: Dropping pending requests means either/and:
	//  * No response/result for the caller
	//  * If evaluated, possible memory leak
	free(s->pending_requests);
	s->pending_requests = NULL;
#endif
}



/* # Public entry points
 * ######################
 */

void msgq_broadcast(msgq_state* state, void* source, const char* message, void* data)
{
	pending_broadcast c;
	c.msgid = broadcastid(message);
	c.source = source;
	c.data = data;

	size_t npending = 0;
	while(state->pending_broadcasts[npending].source != NULL) {
		assert(state->pending_broadcasts[npending+1].msgid != MSGID_GUARD);
		npending += 1;
	}

	state->pending_broadcasts[npending] = c;
}

void msgq_listen(msgq_state* state, void* me, const char* message, msgq_listener listener)
{
	messageid msgid = broadcastid(message);

	size_t i = 0;
	if (state->listeners[msgid] == NULL) {
		state->listeners[msgid] = malloc(sizeof(listener) * 2);
		i = 1;
	} else {
		while(state->listeners[msgid][i++].func != NULL) { }
		state->listeners[msgid] = realloc(state->listeners[msgid], sizeof(listener) * (i+1));
	}
	state->listeners[msgid][i-1].func = listener;
	state->listeners[msgid][i-1].identity = me;
	state->listeners[msgid][i].func = NULL;
	state->listeners[msgid][i].identity = NULL;
}

void msgq_request(msgq_state* state, void* me, const char* what, void* argument, msgq_listener callback)
{
	size_t i = 0;
	while(state->pending_requests[i].response != NULL) {
		assert(state->pending_requests[i+1].msgid != MSGID_GUARD);
		i++;
	}
	state->pending_requests[i].msgid = requestid(what);
	state->pending_requests[i].requester = me;
	state->pending_requests[i].arg = argument;
	state->pending_requests[i].response = callback;
	state->pending_requests[i].state = 0;
}

void msgq_serve(msgq_state* state, void* me, const char* what, msgq_handler handler)
{
	messageid msgid = requestid(what);

	// TODO: Possible memory leak. Maybe not desirable to overwrite, especially considering hash-collisions..
	state->handlers[msgid].identity = me;
	state->handlers[msgid].func = handler;
}


/* # Dispatch loop stuff
 * ######################
 */

void dispatch_all_pending(msgq_state* state)
{
	for(size_t i = 0;;++i) {
		if (state->pending_broadcasts[i].source == NULL) break;

		dispatch_broadcast(state, state->pending_broadcasts + i);
		state->pending_broadcasts[i].msgid = 0;
		state->pending_broadcasts[i].source = NULL;
		state->pending_broadcasts[i].data = NULL;
	}
}
void evaluate_all_pending(msgq_state* state)
{
	size_t i = 0;
	while(state->pending_requests[i].response != NULL) {
		if (!request_evaluated(state->pending_requests[i])) {
			evaluate_request(state, state->pending_requests + i);
		}
		i++;
	}
}
void return_all_pending(msgq_state* state)
{
	size_t i = 0;
	while(state->pending_requests[i].response != NULL) {
		if (request_evaluated(state->pending_requests[i])
		 && !request_returned(state->pending_requests[i])) {
			return_request(state, state->pending_requests + i);
		}
		i++;
	}
}
void fastforward_all_pending(msgq_state* state)
{
	size_t i = 0;
	while(state->pending_requests[i].response != NULL) {
		fastforward_request(state, state->pending_requests + i);
		clear_pending_request(state->pending_requests + i);
		i++;
	}
}
void msgq_flush_all(msgq_state* state)
{
	dispatch_all_pending(state);
	fastforward_all_pending(state);
}


/* # Dispatching/actual message-brokering
 * ########################################
 */

void dispatch_broadcast(msgq_state* state, pending_broadcast* b)
{
	size_t i = 0;
	while(state->listeners[b->msgid][i].func != NULL) {
		state->listeners[b->msgid][i].func(
			state->listeners[b->msgid][i].identity,
			b->source, b->data);
		i++;
	}
}


void evaluate_request(msgq_state* state, pending_request* r)
{
	assert(state != NULL);
	assert(r != NULL);

	handler h = state->handlers[r->msgid];
	assert(h.func != NULL);

	r->result = h.func(h.identity, r->requester, r->arg);
	r->requestee = h.identity;
	r->state |= 1;
}

void return_request(msgq_state* state, pending_request* r)
{
	UNUSED(state);
	assert(r != NULL);
	assert(state != NULL);
	assert(r->response != NULL);
	assert(request_evaluated(*r));

	r->response(r->requester, r->requestee, r->result);
	r->state |= 2;
}

void fastforward_request(msgq_state* state, pending_request* r) {
	if (request_returned(*r)) return;
	if (!request_evaluated(*r)) {
		evaluate_request(state, r);
	}
	return_request(state, r);
}

