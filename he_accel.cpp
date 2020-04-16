#include <iostream>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>

typedef ap_axis<32, 1,1,1> int_axis;
void top_function(hls::stream<int_axis> &A, hls::stream<int_axis> &B, hls::stream<int_axis> &outStream) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=A
#pragma HLS INTERFACE axis port=B
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl

	int_axis valOut, Aa, Bb;

	int C[1024];
	for (int i = 0; i < 1024; i++)
		C[i] = 0;

	// do the computation
	for (int j = 0; j < 6; j++) {
		for (int i = 0; i < 1024; ++i) {
			A >> Aa;
			B >> Bb;
			C[i] += Aa.data * Bb.data;
		}
	}

	// stream back the results
	for (int j=0; j<1024; j++){
		valOut.data = C[j];
		valOut.last = (j == 1023)? 1 : 0;
		valOut.strb = 0xf;
		valOut.keep = 0xf;
		outStream << valOut;
	}
}
