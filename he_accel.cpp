#include "he_accel.h"


void tLweFFTAddMulRTo(hls::stream<cmpxDataIn> &A, hls::stream<cmpxDataIn> &B,  hls::stream<cmpxDataOut> &out) {

	cmpxDataOut Aa, Bb;
	cmpxDataOut valOut;

	cmpxDataOut C[1024];
	for (int i = 0; i < 1024; i++)
		C[i] = cmpxDataOut(0, 0);

	// do the computation
	for (int j = 0; j < 6; j++) {
		for (int i = 0; i < 1024; ++i) {
			A >> Aa;
			B >> Bb;
			C[i] += Aa * Bb;
		}
	}

	// stream back the results
	for (int j=0; j<FFT_LENGTH; j++){
		out.write(C[j]);
	}
}

void dummy_proc_fe(
    bool direction,
    config_t* config,
	hls::stream<cmpxDataIn> &in,
    cmpxDataIn out[FFT_LENGTH])
{
    int i;
    config->setDir(direction);
    config->setSch(0x2AB);
    for (i=0; i< FFT_LENGTH; i++)
    	in.read(out[i]);
}

void dummy_proc_be(
    status_t* status_in,
    bool* ovflo,
    cmpxDataOut in[FFT_LENGTH],
	hls::stream<cmpxDataOut> &out)
{
    int i;
    for (i=0; i< FFT_LENGTH; i++)
        out.write(in[i]);
    *ovflo = status_in->getOvflo() & 0x1;
}

void fft_top(hls::stream<cmpxDataIn> &in, hls::stream<cmpxDataOut> &out, bool direction, bool* ovflo) {

	cmpxDataIn xn[FFT_LENGTH];
	cmpxDataOut xk[FFT_LENGTH];

	config_t fft_config;
	status_t fft_status;

	dummy_proc_fe(direction, &fft_config, in, xn);

	hls::fft<config1>(xn, xk, &fft_status, &fft_config);

	dummy_proc_be(&fft_status, ovflo, xk, out);

}


void top_function(hls::stream<cmpxDataIn> &deca, hls::stream<cmpxDataIn> &gsw, hls::stream<cmpxDataOut> &outStream) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=deca
#pragma HLS INTERFACE axis port=gsw
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl

	hls::stream<cmpxDataOut> decaFFT;
	hls::stream<cmpxDataOut> tmpa;
	bool ovflo;

	// fft deca
	fft_top(deca, decaFFT, false, &ovflo);

	tLweFFTAddMulRTo(decaFFT, gsw, tmpa);

	// inverse FFT on tmpa
	fft_top(tmpa, outStream, true, &ovflo);

}
