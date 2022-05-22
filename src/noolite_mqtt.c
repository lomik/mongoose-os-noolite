#include "mgos.h"
#include "noolite.h"
#include "mgos_mqtt.h"

struct mg_str NOO_MQTT_TOPIC;

int noo_mqtt_mg_str_atoi(struct mg_str s) {
    int res = 0;
    for (int i=0; i<s.len; i++) {
        res *= 10;
        if ((s.p[i]>=0x30) && (s.p[i]<=0x39)) {
            res+= s.p[i]-0x30;
        } else {
            return 0;
        }
    }
    return res;
}

int noo_mqtt_split3(struct mg_str s, struct mg_str* p1, struct mg_str* p2, struct mg_str* p3) {
    int i=0, j=0, n=0;

    for (i=0; i<=s.len; i++) {
        if ((i==s.len) || (s.p[i] == 0x2f)) { // 0x2f = "/"
            switch (n)
            {
            case 0:
                p1->p = s.p+j;
                p1->len = i-j;
                break;
            case 1:
                p2->p = s.p+j;
                p2->len = i-j;
                break;
            case 2:
                p3->p = s.p+j;
                p3->len = i-j;
                break;
            default:
                break;
            }
            n++;
            j=i+1;
        }
    }

    return n;
}


static void noo_mqtt_sent_cb(int ev, void *ev_data, void *userdata) {
    mtrf_msg* pkt = (mtrf_msg*)ev_data;

    char topic[100];
    snprintf(topic, sizeof(topic), "%.*s/sent/raw",
           NOO_MQTT_TOPIC.len, NOO_MQTT_TOPIC.p);
    mgos_mqtt_pubf(topic, 0, false /* retain */,
        "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",
        pkt->st, pkt->mode, pkt->ctr, pkt->res, pkt->ch, pkt->cmd, pkt->fmt, 
        pkt->d0, pkt->d1, pkt->d2, pkt->d3, pkt->id0, pkt->id1, pkt->id2, pkt->id3, pkt->crc, pkt->sp);
    (void) ev;
    (void) ev_data;
    (void) userdata;
}


static void noo_mqtt_recv_cb(int ev, void *ev_data, void *userdata) {
    mtrf_msg* pkt = (mtrf_msg*)ev_data;

    char topic[100];
    snprintf(topic, sizeof(topic), "%.*s/recv/raw",
           NOO_MQTT_TOPIC.len, NOO_MQTT_TOPIC.p);
    mgos_mqtt_pubf(topic, 0, false /* retain */,
        "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",
        pkt->st, pkt->mode, pkt->ctr, pkt->res, pkt->ch, pkt->cmd, pkt->fmt, 
        pkt->d0, pkt->d1, pkt->d2, pkt->d3, pkt->id0, pkt->id1, pkt->id2, pkt->id3, pkt->crc, pkt->sp);

    char mode[10];
    char sub_topic[32];
    char payload[32];

    payload[0] = 0x0; // empty string by default

    switch(pkt->mode) {
        case MTRF_MODE_RX:
            sprintf(mode, "rx");
        case MTRF_MODE_RXF:
            sprintf(mode, "rxf");
        case MTRF_MODE_TX:
            sprintf(mode, "tx");
        case MTRF_MODE_TXF:
            sprintf(mode, "txf");
        default:
            return;
    }

    switch(pkt->cmd) {
        case MTRF_CMD_ON:
            sprintf(sub_topic, "power");
            sprintf(payload, "on");
        case MTRF_CMD_OFF:
            sprintf(sub_topic, "power");
            sprintf(payload, "off");
        case MTRF_CMD_SWITCH:
            sprintf(sub_topic, "switch");
        case MTRF_CMD_BRIGHT_BACK:
            sprintf(sub_topic, "bright_back");
        case MTRF_CMD_STOP_REG:
            sprintf(sub_topic, "stop_reg");
        default:
            return;
    }

    snprintf(topic, sizeof(topic), "%.*s/recv/%s/%d/%s",
           NOO_MQTT_TOPIC.len, NOO_MQTT_TOPIC.p, mode, pkt->ch, sub_topic);

    mgos_mqtt_pub(topic, payload, strlen(payload), 0, false);


    (void) ev;
    (void) ev_data;
    (void) userdata;
}

static void noo_mqtt_write_cb(struct mg_connection *nc, const char *ev_topic,
                              int ev_topic_len, const char *ev_msg, int ev_msg_len,
                              void *ud) {

    LOG(LL_INFO, ("[noolite] mqtt recv topic=%s msg=%s", ev_topic, ev_msg));

    // skip NOO_MQTT_TOPIC+"/write/"
    struct mg_str topic = mg_mk_str_n(ev_topic+NOO_MQTT_TOPIC.len+7, ev_topic_len-NOO_MQTT_TOPIC.len-7);
    struct mg_str msg = mg_mk_str_n(ev_msg, ev_msg_len);

    struct mg_str p1 = MG_NULL_STR;
    struct mg_str p2 = MG_NULL_STR;
    struct mg_str p3 = MG_NULL_STR;

    int n = noo_mqtt_split3(topic, &p1, &p2, &p3);

    if (n>3) {
        return;
    }

    if (n==1 && mg_strcmp(p1, mg_mk_str("raw"))==0) {
        LOG(LL_INFO, ("[noolite] write raw"));
        return;    
    }

    mtrf_msg pkt = EMPTY_MTRF_MSG;

    if (n==3) {
        pkt.ch = noo_mqtt_mg_str_atoi(p2);

        if (mg_strcmp(p1, mg_mk_str("tx"))==0) {
            pkt.mode = MTRF_MODE_TX;
            goto cmd_tx;
        }
        if (mg_strcmp(p1, mg_mk_str("txf"))==0) {
            pkt.mode = MTRF_MODE_TXF;
            goto cmd_txf;
        }
        if (mg_strcmp(p1, mg_mk_str("rx"))==0) {
            pkt.mode = MTRF_MODE_RX;
            goto cmd_rx;
        }
    }

    return;

/* only TX-F commands */
cmd_txf:
    if (mg_strcmp(p3, mg_mk_str("state"))==0) {
        pkt.cmd = MTRF_CMD_READ_STATE;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("state0"))==0) {
        pkt.cmd = MTRF_CMD_READ_STATE;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("state1"))==0) {
        pkt.cmd = MTRF_CMD_READ_STATE;
        pkt.fmt = 1;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("state2"))==0) {
        pkt.cmd = MTRF_CMD_READ_STATE;
        pkt.fmt = 2;
        goto run;
    }

/* common TX and TX-F commands */
cmd_tx:
    if (mg_strcmp(p3, mg_mk_str("power"))==0) {
        if (mg_strcmp(msg, mg_mk_str("on"))==0) {
            pkt.cmd = MTRF_CMD_ON;
            goto run;
        }
        if (mg_strcmp(msg, mg_mk_str("off"))==0) {
            pkt.cmd = MTRF_CMD_OFF;
            goto run;
        }
    }
    if (mg_strcmp(p3, mg_mk_str("switch"))==0) {
        pkt.cmd = MTRF_CMD_SWITCH;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("on"))==0) {
        pkt.cmd = MTRF_CMD_ON;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("off"))==0) {
        pkt.cmd = MTRF_CMD_OFF;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("bind"))==0) {
        pkt.cmd = MTRF_CMD_BIND;
        goto run;
    }
    if (mg_strcmp(p3, mg_mk_str("unbind"))==0) {
        pkt.cmd = MTRF_CMD_UNBIND;
        goto run;
    }

    // unknown command
    return;

cmd_rx:
    if (mg_strcmp(p3, mg_mk_str("bind"))==0) {
        pkt.ctr = MTRF_CTR_BIND_ON;
        goto run;
    }

    // unknown command
    return;

/* execute command */
run:
    noo_write(pkt);
}


bool noo_init_mqtt() {
    if (strcmp(mgos_sys_config_get_noolite_mqtt_topic(), "") == 0) {
        return false;
    }

    NOO_MQTT_TOPIC = mg_mk_str(mgos_sys_config_get_noolite_mqtt_topic());

    mgos_event_add_handler(NOO_EVENT_SENT, noo_mqtt_sent_cb, NULL);
    mgos_event_add_handler(NOO_EVENT_RECV, noo_mqtt_recv_cb, NULL);

    char topic[100];
    snprintf(topic, sizeof(topic), "%.*s/write/#", NOO_MQTT_TOPIC.len, NOO_MQTT_TOPIC.p);
    mgos_mqtt_sub(topic, noo_mqtt_write_cb, NULL);
    return true;
}
