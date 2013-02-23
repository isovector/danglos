#ifndef _MSG_H_
#define _MSG_H_

typedef struct message_envelope {
    int type, dest, src, len;
    char ctrl;
    char *data;
} msg_envelope_t;


extern msg_envelope_t *receive_message(int *sender);

// the lab manual says these should return an int, but doesn't say what
extern void send_message(int pid, msg_envelope_t *msg);
extern void delayed_send(int pid, msg_envelope_t *msg, int delay);

#endif
