#ifndef __NOOLITE_H__
#define __NOOLITE_H__

#include "mtrf.h"

#define NOO_EVENT_BASE MGOS_EVENT_BASE('N', 'O', 'O')

#define NOO_MQTT_TOPIC "noo2"

enum noo_event {
  NOO_EVENT_RECV = NOO_EVENT_BASE,
  NOO_EVENT_SENT,
};

bool noo_init();
bool noo_init_mqtt();

bool noo_write(mtrf_msg pkt);

/* Mode TX */
void noo_tx_bind(char chan);
void noo_tx_unbind(char chan);
void noo_tx_on(char chan);
void noo_tx_off(char chan);
void noo_tx_power(char chan, bool state);
void noo_tx_switch(char chan);


/* Mode TX-F */
void noo_txf_bind(char chan);
void noo_txf_unbind(char chan);
void noo_txf_on(char chan);
void noo_txf_off(char chan);
void noo_txf_power(char chan, bool state);
void noo_txf_switch(char chan);

#endif