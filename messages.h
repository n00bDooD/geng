#ifndef MESSAGES_H
#define MESSAGES_H

typedef struct msgq_state msgq_state;

typedef void (*msgq_listener)(void* me, void* source, void* data);
typedef void* (*msgq_handler)(void* me, void* source, void* argument);

void msgq_broadcast(msgq_state*, void* me, const char* what, void* data);
void msgq_listen(msgq_state*, void* me, const char* what, msgq_listener listener);

void msgq_request(msgq_state*, void* me, const char* what, void* argument, msgq_listener responder);
void msgq_serve(msgq_state*, void* me, const char* what, msgq_handler handler);

#endif /* MESSAGES_H */
