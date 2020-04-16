#include <iostream>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include "he_accel.h"

// declare 32 bit integer with side-channel
typedef ap_axis<32, 1, 1, 1> int_axis;

void top_function(hls::stream<int_axis> &A, hls::stream<int_axis> &B, hls::stream<int_axis> &outStream);
void fft_top(hls::stream<cmpxDataIn> &in, hls::stream<cmpxDataOut> &out, bool direction, bool* ovflo);

int main()
{

	// prepare data for DUT
	hls::stream<int_axis> inputStreamA("A");
	hls::stream<int_axis> inputStreamB("B");
	hls::stream<int_axis> outputStream("Out");

	int_axis valueIn, valOut;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 1024; i++) {

			valueIn.data = i;
			valueIn.strb = 0xf;
			valueIn.keep = 0xf;
			valueIn.last = 0;
			inputStreamA << valueIn;
			inputStreamB << valueIn;
		}
	}
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 1024; i++) {

			valueIn.data = 0;
			valueIn.strb = 0xf;
			valueIn.keep = 0xf;
			valueIn.last = (j == 3 && i == 1023) ? 1 : 0;
			inputStreamA << valueIn;
			inputStreamB << valueIn;
		}
	}

	top_function(inputStreamA, inputStreamB, outputStream);

	for (int i = 0; i < 1024; i++) {
		outputStream.read(valOut);
		printf("Value is %d\n", (int)valOut.data);
	}

	return 0;
}
