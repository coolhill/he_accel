#include "he_accel.h"

void mult(hls::stream<complex_axi_type> &decaFFT, hls::stream<complex_axi_type> &gsw, hls::stream<complex_axi_type> &outStream) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=decaFFT
#pragma HLS INTERFACE axis port=gsw
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl

	complex_axi_type valOut, Aa, Bb;

	cmpxDataIn C[1024];
	for (int i = 0; i < 1024; i++)
		C[i] = 0;

	// do the computation
	for (int j = 0; j < 6; j++) {
		for (int i = 0; i < 1024; ++i) {
			decaFFT >> Aa;
			gsw >> Bb;
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
