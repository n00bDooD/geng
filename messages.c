#include "messages.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "city.h"

#define MAX_MESSAGETYPES 512
#define MAX_REQUESTTYPES 512


typedef uint64_t messageid;

#define MSGID_GUARD UINT64_MAX

#define broadcastid(s) CityHash64(s, MAX_MESSAGETYPES)
#define requestid(s) CityHash64(s, MAX_REQUESTTYPES)


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


#define request_evaluated(r) (r.state & 1)
#define request_returned(r) (r.state & 2)

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
	listener* listeners[MAX_MESSAGETYPES];
	handler handlers[MAX_REQUESTTYPES];

	pending_broadcast* pending_broadcasts;
	pending_request* pending_requests;
};

void fastforward_request(msgq_state* state, pending_request* r);
void return_request(msgq_state* state, pending_request* r);
void evaluate_request(msgq_state* state, pending_request* r);



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
	//s->listeners = malloc(sizeof(listener*) * MAX_MESSAGETYPES);
	for(int i = 0; i < MAX_MESSAGETYPES; ++i) { s->listeners[i] = NULL; }
	//s->handlers = malloc(sizeof(handler*) * MAX_REQUESTTYPES);
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
		s->pending_requests[i].msgid = 0;
		s->pending_requests[i].response = NULL;
		s->pending_requests[i].requester = NULL;
		s->pending_requests[i].requestee = NULL;
		s->pending_requests[i].arg = NULL;
		s->pending_requests[i].result = NULL;
		s->pending_requests[i].state = 0;
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

	listener* l = malloc(sizeof(listener));
	l->identity = me;
	l->func = listener;

	size_t i = 0;
	while(state->listeners[msgid][i] != NULL) {
		i++;
	}
	state->listeners = realloc(state->listeners, sizeof(listener) * (i+1));
	state->listeners[msgid][i] = l;
	state->listeners[msgid][i+1] = NULL;
}

void msgq_request(msgq_state* state, void* me, const char* what, void* argument, msgq_listener callback)
{
	pending_request* r = malloc(sizeof(pending_request));
	r->msgid = requestid(what);
	r->requester = me;
	r->arg = argument;
	r->response = callback;
	r->state = 0;

	size_t i = 0;
	while(state->pending_requests[i] != NULL) {
		assert(state->pending_requests[i+1].msgid != MSGID_GUARD);
		i++;
	}
	state->pending_requests[i] = r;
}

void msgq_serve(msgq_state* state, void* me, const char* what, msgq_handler handler)
{
	messageid msgid = requestid(what);

	handler* l = malloc(sizeof(handler));
	l->identity = me;
	l->func = listener;

	// TODO: Possible memory leak. Maybe not desirable to overwrite, especially considering hash-collisions..
	state->handlers[msgid] = *l;
}

void msgq_dispatch_all(msgq_state* state)
{
	dispatch_all_pending(state);
	fastforward_all_pending(state);
}


/* # Dispatch loop stuff
 * ######################
 */

void dispatch_all_pending(msgq_state* state)
{
	size_t i = 0;
	for(size_t i = 0;;++i) {
		if (state->pending_broadcasts[i].source == NULL) break;

		dispatch_broadcast(state, b);
	}
	free(state->pending_broadcasts);
	state->pending_broadcasts = NULL;
}
void evaluate_all_pending(msgq_state* state)
{
	pending_request* r;
	size_t i = 0;
	while((r = state->pending_requests[i]) != NULL) {
		if (!request_evaluated(r)) {
			evaluate_request(r);
		}
	}
}
void return_all_pending(msgq_state* state)
{
	pending_request* r;
	size_t i = 0;
	while((r = state->pending_requests[i]) != NULL) {
		if (request_evaluated(r) && !request_returned(r)) {
			return_request(state, r);
		}
	}
}
void fastforward_all_pending(msgq_state* state)
{
	pending_request* r;
	size_t i = 0;
	while((r = state->pending_requests[i]) != NULL) {
		fastforward_request(state, r);
	}
	free(state->pending_requests);
	state->pending_requests = NULL;
}


/* # Dispatching/actual message-brokering
 r->evaluated) {* ########################
 */

void dispatch_broadcast(msgq_state* state, pending_broadcast* b)
{
	listener* l;
	size_t i = 0;
	while((l = state->listeners[b->msgid][i]) != NULL) {
		l->func(l->identity, source, data);
		i++;
	}
}


void evaluate_request(msgq_state* state, pending_request* r)
{
	assert(state != NULL);
	assert(r != NULL);

	handler h = state->handlers[r->msgid];
	assert(h != NULL);
	assert(h->func != NULL);

	h->result = h->func(h->identity, r->requester, r->arg);
	h->requestee = h->identity;
	r->state |= 1;
}

void return_request(msgq_state* state, pending_request* r)
{
	assert(r != NULL);
	assert(state != NULL);
	assert(r->response != NULL);
	assert(request_evaluated(r));

	r->response(r->requester, r->requestee, r->result);
	r->state |= 2;
}

void fastforward_request(msgq_state* state, pending_request* r) {
	if (request_returned(r)) return;
	if (!request_evaluated(r)) {
		evaluate_request(state, r);
	}
	return_request(state, r);
}

