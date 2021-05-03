#include "mgos.h"
#include "mgos_rpc.h"
#include "noolite.h"

int NOOLITE_UART_NO = 0;

bool noo_write(mtrf_msg pkt) {
    if (mgos_uart_write_avail(NOOLITE_UART_NO) < 17) {
        return false;
    }

    pkt.st = MTRF_REQ_BEGIN;
    pkt.sp = MTRF_REQ_END;

    char buf[17] = {pkt.st, pkt.mode, pkt.ctr, pkt.res, pkt.ch, pkt.cmd, pkt.fmt, 
        pkt.d0, pkt.d1, pkt.d2, pkt.d3, pkt.id0, pkt.id1, pkt.id2, pkt.id3, pkt.crc, pkt.sp};

    // crc
    int i = 0;
    char x = 0;
	for (i = 0; i < 15; i++) {
		x += buf[i];
	}
	buf[15] = x;
    pkt.crc = x;

    mgos_uart_write(NOOLITE_UART_NO, buf, 17);
    mgos_uart_flush(NOOLITE_UART_NO);

    LOG(LL_INFO, ("[noolite] sent data={%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}",
            buf[0], buf[1], buf[2], buf[3],
            buf[4], buf[5], buf[6], buf[7],
            buf[8], buf[9], buf[10], buf[11],
            buf[12], buf[13], buf[14], buf[15],
            buf[16]));

    mgos_event_trigger(NOO_EVENT_SENT, &pkt);
    return true;
}

void noo_recv_cb(int uart_no, void *arg) {
    if (mgos_uart_read_avail(uart_no) < 1) {
        return;
    }
    char buf[17];
    size_t recv;
    recv = mgos_uart_read(uart_no, buf, 17);
    if (recv>0) {
        LOG(LL_INFO, ("[noolite] recv len=%d, data={%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}",
            recv,
            buf[0], buf[1], buf[2], buf[3],
            buf[4], buf[5], buf[6], buf[7],
            buf[8], buf[9], buf[10], buf[11],
            buf[12], buf[13], buf[14], buf[15],
            buf[16]));
    }
    if (recv==17) {
        mtrf_msg pkt = {
            buf[0], buf[1], buf[2], buf[3],
            buf[4], buf[5], buf[6], buf[7],
            buf[8], buf[9], buf[10], buf[11],
            buf[12], buf[13], buf[14], buf[15],
            buf[16]};
        mgos_event_trigger(NOO_EVENT_RECV, &pkt);
    }
    (void) recv;
}

bool noo_init_uart() {
    struct mgos_uart_config ucfg;
    mgos_uart_config_set_defaults(NOOLITE_UART_NO, &ucfg);

    ucfg.baud_rate = 9600;
    ucfg.num_data_bits = 8;
    ucfg.parity = MGOS_UART_PARITY_NONE;
    ucfg.stop_bits = MGOS_UART_STOP_BITS_1;
    ucfg.rx_buf_size =ucfg.tx_buf_size = 256;
    ucfg.rx_linger_micros = 15;

    if (!mgos_uart_configure(NOOLITE_UART_NO, &ucfg)) {
        LOG(LL_ERROR, ("Failed to configure UART%d", NOOLITE_UART_NO));
        return false;
    }

    LOG(LL_ERROR, ("Successed to configure UART%d", NOOLITE_UART_NO));

    mgos_uart_set_dispatcher(NOOLITE_UART_NO, noo_recv_cb, NULL);
    mgos_uart_set_rx_enabled(NOOLITE_UART_NO, true);
    return true;
}



bool mgos_mongoose_os_noolite_init(void) {
    if (!mgos_sys_config_get_noolite_enable()) {
        return true;
    }

    mgos_event_register_base(NOO_EVENT_BASE, "noolite module");
    noo_init_uart();
    noo_init_mqtt();

    return true;
}

/* HELPER COMMANDS */

/* Mode TX */

void noo_tx_cmd(char chan, char cmd) {
    mtrf_msg pkt = {0,MTRF_MODE_TX,0,0,chan,cmd,0,0,0,0,0,0,0,0,0,0,0};
    noo_write(pkt);
}
void noo_tx_bind(char chan) {
    noo_tx_cmd(chan, MTRF_CMD_BIND);
}

void noo_tx_unbind(char chan) {
    noo_tx_cmd(chan, MTRF_CMD_UNBIND);
}

void noo_tx_on(char chan) {
    noo_tx_cmd(chan, MTRF_CMD_ON);
}

void noo_tx_off(char chan) {
    noo_tx_cmd(chan, MTRF_CMD_OFF);
}

void noo_tx_power(char chan, bool state) {
    if (state) {
        noo_tx_on(chan);
    } else {
        noo_tx_off(chan);
    }
}

void noo_tx_switch(char chan) {
    noo_tx_cmd(chan, MTRF_CMD_SWITCH);
}


/* Mode TX-F */
void noo_txf_cmd(char chan, char cmd) {
    mtrf_msg pkt = {0,MTRF_MODE_TXF,0,0,chan,cmd,0,0,0,0,0,0,0,0,0,0,0};
    noo_write(pkt);
}
void noo_txf_bind(char chan) {
    noo_txf_cmd(chan, MTRF_CMD_BIND);
}

void noo_txf_unbind(char chan) {
    noo_txf_cmd(chan, MTRF_CMD_UNBIND);
}

void noo_txf_on(char chan) {
    noo_txf_cmd(chan, MTRF_CMD_ON);
}

void noo_txf_off(char chan) {
    noo_txf_cmd(chan, MTRF_CMD_OFF);
}

void noo_txf_power(char chan, bool state) {
    if (state) {
        noo_txf_on(chan);
    } else {
        noo_txf_off(chan);
    }
}

void noo_txf_switch(char chan) {
    noo_txf_cmd(chan, MTRF_CMD_SWITCH);
}