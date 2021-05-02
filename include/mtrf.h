#ifndef __NOOLITE_MTRF_H__
#define __NOOLITE_MTRF_H__

typedef struct {
	char st;
	char mode;
	char ctr;
	char res;
	char ch;
	char cmd;
	char fmt;
	char d0;
	char d1;
	char d2;
	char d3;
	char id0;
	char id1;
	char id2;
	char id3;
	char crc;
	char sp;
} mtrf_msg;

#define EMPTY_MTRF_MSG {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

static const char MTRF_REQ_BEGIN = 171;
static const char MTRF_REQ_END = 172;
static const char MTRF_RES_BEGIN = 173;
static const char MTRF_RES_END = 174;

static const char MTRF_MODE_TX = 0;
static const char MTRF_MODE_RX = 1;
static const char MTRF_MODE_TXF = 2;
static const char MTRF_MODE_RXF = 3;
static const char MTRF_MODE_SERVICE = 4;
static const char MTRF_MODE_UPGRADE = 5;

static const char MTRF_CTR_SEND = 0;
static const char MTRF_CTR_BROADCAST = 1;
static const char MTRF_CTR_RECV = 2;
static const char MTRF_CTR_BIND_ON = 3;
static const char MTRF_CTR_BIND_OFF = 4;
static const char MTRF_CTR_CLEAR_CHANNEL = 5;
static const char MTRF_CTR_CLEAR_ALL = 6;
static const char MTRF_CTR_UNBIND = 7;
static const char MTRF_CTR_SEND_F = 8;

static const char MTRF_CMD_OFF = 0;
static const char MTRF_CMD_BRIGHT_DOWN = 1;
static const char MTRF_CMD_ON = 2;
static const char MTRF_CMD_BRIGHT_UP = 3;
static const char MTRF_CMD_SWITCH = 4;
static const char MTRF_CMD_BRIGHT_BACK = 5;
static const char MTRF_CMD_SET_BRIGHTNESS = 6;
static const char MTRF_CMD_LOAD_PRESET = 7;
static const char MTRF_CMD_SAVE_PRESET = 8;
static const char MTRF_CMD_UNBIND = 9;
static const char MTRF_CMD_STOP_REG = 10;
static const char MTRF_CMD_BRIGHT_STEP_DOWN = 11;
static const char MTRF_CMD_BRIGHT_STEP_UP = 12;
static const char MTRF_CMD_BRIGHT_REG = 13;
static const char MTRF_CMD_BIND = 15;
static const char MTRF_CMD_ROLL_COLOUR = 16;
static const char MTRF_CMD_SWITCH_COLOUR = 17;
static const char MTRF_CMD_SWITCH_MODE = 18;
static const char MTRF_CMD_SPEED_MODE_BACK = 19;
static const char MTRF_CMD_BATTERY_LOW = 20;
static const char MTRF_CMD_SENS_TEMP_HUMI = 21;
static const char MTRF_CMD_TEMPORARY_ON = 25;
static const char MTRF_CMD_MODES = 26;
static const char MTRF_CMD_READ_STATE = 128;
static const char MTRF_CMD_WRITE_STATE = 129;
static const char MTRF_CMD_SEND_STATE = 130;
static const char MTRF_CMD_SERVICE = 131;
static const char MTRF_CMD_CLEAR_MEMORY = 132;

#endif