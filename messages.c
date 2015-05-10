#include "messages.h"
#include "global.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <strings.h>

#include "city.h"


typedef uint64_t messageid;

#define MSGID_GUARD UINT64_MAX
#define MSGID_UNUSED (UINT64_MAX-1)

#define broadcastid(state, s) (CityHash64(s, strlen(s)) % state->messagebuckets)
#define requestid(state, s) (CityHash64(s, strlen(s)) % state->requestbuckets)


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
	size_t messagebuckets;
	size_t requestbuckets;

	size_t broadcast_start;
	size_t broadcast_length;
	size_t request_start;
	size_t request_length;
	pending_broadcast* pending_broadcasts;
	pending_request* pending_requests;
};

static inline void clear_pending_request(pending_request* r)
{
	r->msgid = MSGID_UNUSED;
	r->response = NULL;
	r->requester = NULL;
	r->requestee = NULL;
	r->arg = NULL;
	r->result = NULL;
	r->state = 0;
}

static inline void clear_pending_broadcast(pending_broadcast* r)
{
	r->msgid = MSGID_UNUSED;
	r->source = NULL;
	r->data = NULL;
}

void fastforward_request(msgq_state* state, pending_request* r);
void return_request(msgq_state* state, pending_request* r);
void evaluate_request(msgq_state* state, pending_request* r);
void dispatch_broadcast(msgq_state* state, pending_broadcast* b);

void fastforward_all_pending(msgq_state* state);
void return_all_pending(msgq_state* state);
void evaluate_all_pending(msgq_state* state);
void dispatch_all_pending(msgq_state* state);

void dispatch_broadcasts(msgq_state* state, size_t brdc);
void evaluate_requests(msgq_state* state, size_t reqc);
void return_requests(msgq_state* state, size_t reqc);


/* # Memory management
 * ####################
 */

msgq_state* msgq_create(msgq_state* s, 
		        size_t broadcast_hashmap_len,
		        size_t request_hashmap_len,
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

	s->messagebuckets = broadcast_hashmap_len;
	s->requestbuckets = request_hashmap_len;

	s->broadcast_start = 0;
	s->broadcast_length = 0;
	s->request_start = 0;
	s->request_length = 0;

	s->listeners = malloc(sizeof(listener*) * s->messagebuckets);
	for(size_t i = 0; i < s->messagebuckets; ++i) { 
		s->listeners[i] = malloc(sizeof(listener));
		s->listeners[i][0].func = NULL;
		s->listeners[i][0].identity = NULL;
	}
	s->handlers = malloc(sizeof(handler) * s->requestbuckets);
	for(size_t i = 0; i < s->requestbuckets; ++i) { s->handlers[i].func = NULL;
	                                            s->handlers[i].identity = NULL; }

	s->pending_broadcasts = malloc(sizeof(pending_broadcast) * broadcast_buffer_length);
	for(size_t i = 0; i < broadcast_buffer_length; ++i) {
		clear_pending_broadcast(s->pending_broadcasts + i);
	}
	s->pending_broadcasts[broadcast_buffer_length-1].msgid = MSGID_GUARD;

	s->pending_requests = malloc(sizeof(pending_request) * request_buffer_length);
	for(size_t i = 0; i < request_buffer_length; ++i) {
		clear_pending_request(s->pending_requests + i);
	}
	s->pending_requests[request_buffer_length-1].msgid = MSGID_GUARD;
	return s;
}

void msgq_free(msgq_state* s)
{
	free(s->listeners);
	free(s->handlers);
	free(s->pending_broadcasts);
	free(s->pending_requests);
}

void msgq_reset(msgq_state* s)
{
	for(size_t i = 0; i < s->messagebuckets; ++i) { 
		if (s->listeners[i] != NULL) {
			free(s->listeners[i]);
			s->listeners[i] = NULL;
		}
	}
	for(size_t i = 0; i < s->requestbuckets; ++i) { 
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
	for (size_t i = state->broadcast_start + state->broadcast_length;;++i) {
		if (state->pending_broadcasts[i].msgid == MSGID_GUARD) {
			i = 0;
		}
		if (state->pending_broadcasts[i].msgid == MSGID_UNUSED) {
			state->pending_broadcasts[i].msgid = broadcastid(state, message);
			state->pending_broadcasts[i].source = source;
			state->pending_broadcasts[i].data = data;
			state->broadcast_length++;
			return;
		}
		if (i == state->broadcast_start-1) {
			// TODO: We have been around one loop already..
			// there is probably no available spot in the buffer. 
			return;
		}
	}
}

void msgq_listen(msgq_state* state, void* me, const char* message, msgq_listener ln)
{
	messageid msgid = broadcastid(state, message);

	size_t i = 0;
	while (state->listeners[msgid][i].func != NULL) {
		++i;
	}
	listener* l = realloc(state->listeners[msgid], sizeof(listener) * (i+2));
	if (l == NULL) {
		// Alloc error
		return;
	}
	state->listeners[msgid] = l;
	state->listeners[msgid][i].func = ln;
	state->listeners[msgid][i].identity = me;

	state->listeners[msgid][i+1].func = NULL;
	state->listeners[msgid][i+1].identity = NULL;
}

void msgq_request(msgq_state* state, void* me, const char* what, void* argument, msgq_listener callback)
{
	for (size_t i = state->request_start + state->request_length;;++i) {
		if (state->pending_requests[i].msgid == MSGID_GUARD) {
			i = 0;
		}
		if (state->pending_requests[i].msgid == MSGID_UNUSED) {
			// Do stuff
			state->pending_requests[i].msgid = requestid(state, what);
			state->pending_requests[i].response = callback;
			state->pending_requests[i].requester = me;
			state->pending_requests[i].requestee = NULL;
			state->pending_requests[i].arg = argument;
			state->pending_requests[i].result = NULL;
			state->pending_requests[i].state = 0;
			state->request_length++;
			return;
		}
		if (i == state->request_start-1) {
			// TODO: We have been around one loop already..
			// there is probably no available spot in the buffer. 
			return;
		}
	}
}

void msgq_serve(msgq_state* state, void* me, const char* what, msgq_handler handler)
{
	messageid msgid = requestid(state, what);

	if (state->handlers[msgid].func == NULL) {
		// ?
		return;
	}
	state->handlers[msgid].func = handler;
	state->handlers[msgid].identity = me;
}


void msgq_flush_all(msgq_state* state)
{
	dispatch_broadcasts(state, 0);
	return_requests(state, 0);
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

void fastforward_request(msgq_state* state, pending_request* r) 
{
	if (request_returned(*r)) return;
	if (!request_evaluated(*r)) {
		evaluate_request(state, r);
	}
	return_request(state, r);
}


void dispatch_single_broadcast(msgq_state* state)
{
	if (state == NULL || state->broadcast_length == 0) return;

	if (state->pending_broadcasts[state->broadcast_start].msgid == MSGID_GUARD) {
		state->broadcast_start = 0;
	}
	pending_broadcast* b = state->pending_broadcasts + state->broadcast_start;

	assert(b->msgid != MSGID_UNUSED);

	dispatch_broadcast(state, b);
	clear_pending_broadcast(b);

	state->broadcast_start++;
	state->broadcast_length--;
}

void dispatch_broadcasts(msgq_state* state, size_t brdc)
{
	if (state == NULL || state->broadcast_length == 0) return;

	size_t foo = 0;
	size_t i;
	for (i = state->broadcast_start;;++i) {
		if (state->pending_broadcasts[i].msgid == MSGID_GUARD) {
			i = 0;
		}

		assert(state->pending_broadcasts[i].msgid != MSGID_UNUSED);

		dispatch_broadcast(state, state->pending_broadcasts + i);
		clear_pending_broadcast(state->pending_broadcasts + i);
		foo++;

		if (brdc > 0 && foo == brdc) {
			break;
		}
		// no more broadcasts
		if (state->broadcast_length == foo) {
			i = 0;
			break;
		}
	}

	state->broadcast_start = i;
	state->broadcast_length -= foo;
}

void return_requests(msgq_state* state, size_t reqc)
{
	if (state == NULL || state->request_length == 0) return;

	if (state->pending_requests[state->request_start].msgid == MSGID_GUARD) {
		state->request_start = 0;
	}

	size_t clear_count = 0;
	size_t foo = 0;
	bool inc = true;
	size_t i;
	for (i = state->request_start;;++i) {
		if (state->pending_requests[i].msgid == MSGID_GUARD) {
			i = 0;
		}
		if (request_evaluated(state->pending_requests[i])) {
			if (request_returned(state->pending_requests[i])) {
				if (inc) clear_count++;
			} else {
				if (reqc > 0 && foo == reqc) {
					break;
				}
				// We have found our request
				return_request(state, state->pending_requests + i);
				foo++;
				// Keep rollin until we find another, 
				// meet our quota, or run out
				if (inc) clear_count++;
			}
		} else {
			// One or more requests have not yet been
			// evaluated, and therefore cannot be
			// cleared yet.
			// Note this and keep on rollin
			inc = false;
		}
		if (i == state->request_start-1) {
			break;
		}
	}

	size_t j = 0;
	for (i = state->request_start;;++i) {
		if (state->pending_requests[i].msgid == MSGID_GUARD) {
			i = 0;
		}
		clear_pending_request(state->pending_requests + i);
		j++;
		if (j == clear_count) {
			state->request_start = i;
			state->request_length -= clear_count;
			break;
		}
		if (i == state->request_start-1) {
			return;
		}
	}
}
