#include "he_accel.h"


void tLweFFTAddMulRTo(hls::stream<custom_axi_type> &A, hls::stream<custom_axi_type> &B,  hls::stream<custom_axi_type> &out) {

	custom_axi_type Aa, Bb;
	custom_axi_type valOut;

	cmpxDataOut C[1024];
	for (int i = 0; i < 1024; i++)
		C[i] = cmpxDataOut(0, 0);

	// do the computation
	for (int j = 0; j < 6; j++) {
		for (int i = 0; i < 1024; ++i) {
			A >> Aa;
			B >> Bb;
			C[i] += Aa.data * Bb.data;
		}
	}

	// stream back the results
	for (int j=0; j<FFT_LENGTH; j++){
		valOut.data = C[j];
		valOut.last = (j == 1023)? 1 : 0;
		out.write(valOut);
	}
}

void dummy_proc_fe(
    bool direction,
    config_t* config,
	hls::stream<custom_axi_type> &in,
    cmpxDataIn out[FFT_LENGTH])
{
    int i;
    custom_axi_type valIn;
    config->setDir(direction);
    config->setSch(0x2AB);
    for (i=0; i< FFT_LENGTH; i++) {
    	in.read(valIn);
    	out[i] = valIn.data;
    }
}

void dummy_proc_fe1(
    bool direction,
    config_t* config,
	hls::stream<custom_axi_type> &in,
    cmpxDataIn out[FFT_LENGTH])
{
    int i;
    custom_axi_type valIn;
    config->setDir(direction);
    config->setSch(0x2AB);
    for (i=0; i< FFT_LENGTH; i++) {
    	in.read(valIn);
    	out[i] = valIn.data;
    }
}
void dummy_proc_be(
    status_t* status_in,
    bool* ovflo,
    cmpxDataOut in[FFT_LENGTH],
	hls::stream<custom_axi_type> &out)
{
    int i;
    custom_axi_type valOut;
    for (i=0; i< FFT_LENGTH; i++) {
    	valOut.data = in[i];
    	valOut.last = (i == 1023)? 1 : 0;
    	out.write(valOut);
    }
    *ovflo = status_in->getOvflo() & 0x1;
}

void dummy_proc_be1(
    status_t* status_in,
    bool* ovflo,
    cmpxDataOut in[FFT_LENGTH],
	hls::stream<custom_axi_type> &out)
{
    int i;
    custom_axi_type valOut;
    for (i=0; i< FFT_LENGTH; i++) {
    	valOut.data = in[i];
    	valOut.last = (i == 1023)? 1 : 0;
    	out.write(valOut);
    }
    *ovflo = status_in->getOvflo() & 0x1;
}
void fft_top(hls::stream<custom_axi_type> &in, hls::stream<custom_axi_type> &out, bool direction, bool* ovflo) {

	cmpxDataIn xn[FFT_LENGTH];
	cmpxDataOut xk[FFT_LENGTH];

	config_t fft_config;
	status_t fft_status;

#pragma HLS interface ap_fifo depth=1 port=ovflo
#pragma HLS interface ap_fifo depth=1024 port=xn,xk
#pragma HLS data_pack variable=xn
#pragma HLS data_pack variable=xk
#pragma HLS dataflow

#pragma HLS data_pack variable=fft_config

	dummy_proc_fe(direction, &fft_config, in, xn);

	hls::fft<config1>(xn, xk, &fft_status, &fft_config);

	dummy_proc_be(&fft_status, ovflo, xk, out);

}

void ifft_top(hls::stream<custom_axi_type> &in, hls::stream<custom_axi_type> &out, bool direction, bool* ovflo) {

	cmpxDataIn xn1[FFT_LENGTH];
	cmpxDataOut xk1[FFT_LENGTH];

	config_t fft_config1;
	status_t fft_status1;
#pragma HLS interface ap_fifo depth=1 port=ovflo
#pragma HLS interface ap_fifo depth=1024 port=xn1,xk1
#pragma HLS data_pack variable=xn1
#pragma HLS data_pack variable=xk1
#pragma HLS dataflow

#pragma HLS data_pack variable=fft_config1
	dummy_proc_fe1(direction, &fft_config1, in, xn1);

	hls::fft<config1>(xn1, xk1, &fft_status1, &fft_config1);

	dummy_proc_be1(&fft_status1, ovflo, xk1, out);

}


void top_function(hls::stream<custom_axi_type> &deca, hls::stream<custom_axi_type> &gsw, hls::stream<custom_axi_type> &outStream) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=deca
#pragma HLS INTERFACE axis port=gsw
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl

	hls::stream<custom_axi_type> decaFFT;
	hls::stream<custom_axi_type> tmpa;
	//hls::stream<custom_axi_type> outSt;
	bool ovflo;

	// fft deca
	fft_top(deca, decaFFT, false, &ovflo);

	tLweFFTAddMulRTo(decaFFT, gsw, tmpa);

	// inverse FFT on tmpa
	//void ifft_top(hls::stream<custom_axi_type>, hls::stream<cmpxDataOut>, bool , bool*); <- earlier version
	ifft_top(tmpa, outStream, true, &ovflo);

}
