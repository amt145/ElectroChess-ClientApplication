/*
#include "ft.h"
#include "crc.h"
#include "parser.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

FastTransfer::FastTransfer(uint8_t address,
void(*put)(uint8_t),
uint8_t(*get)(),
bool(*empty)())
{
    // initialize local data:
	this->address = address;

	int i;
	for (i = 0; i < ARRAY_SZ; ++i) {
		this->array[i] = 0;
		this->flags[i] = false;
	}

	// initialize callback function members:
	this->put = put;
	this->get = get;
	this->empty = empty;

	// initialize receive/transmit buffers:
	Buffer_init(&this->receive_buf);
	Buffer_init(&this->transmit_buf);
}

int16_t FastTransfer::FT_Read(uint8_t index) {
    this->flags[index] = false;
    return(int16_t)(this->array[index]);
}

bool FastTransfer::FT_Modified(uint8_t index) {
    bool temp = this->flags[index];
    this->flags[index] = false;
    return temp;
}

void FastTransfer::FT_Receive() {
    unsigned char val, tmpVal;
    while(!this->empty() && !Buffer_full(&this->receive_buf)) {
        val = this->get();
        this->receive_buf.data[(unsigned int)(this->receive_buf.tail)] = (unsigned char)val;
        this->receive_buf.tail = this->receive_buf.tail + 1;
        if (this->receive_buf.tail == MAX_PCKT_SZ)
            this->receive_buf.tail = 0;
        if (this->receive_buf.size <= MAX_PCKT_SZ)
            this->receive_buf.size = this->receive_buf.size + 1;
    }
    parse(&this->receive_buf, this->array, this->flags, ARRAY_SZ, this->address);
}

void FastTransfer::FT_ToSend(uint8_t index, int16_t data) {
    	uint16_t combined = (uint16_t)data;
	uint8_t msb = (combined >> 8);
	uint8_t lsb = combined & 0x00FF;
	Buffer_push(&this->transmit_buf, index);
	Buffer_push(&this->transmit_buf, lsb);
	Buffer_push(&this->transmit_buf, msb);
}

void FastTransfer::FT_Send(uint8_t address) {
    size_t payload_sz = Buffer_size(&this->transmit_buf);
    if (payload_sz == 0)
        return;
    this->put(0x06);
    this->put(0x85);
    this->put(address);
    this->put(this->address);
    this->put(payload_sz);

    uint8_t payload[MAX_PCKT_SZ];
    unsigned int i;
    for (i = 0; i < payload_sz; ++i) {
        uint8_t next_byte = Buffer_pop(&this->transmit_buf);
        payload[i] = next_byte;
        this->put(next_byte);
    }
    this->put(crc(payload, payload_sz));
}
*/