#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include "he_accel.h"

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

/** termwise multiplication and addTo in Lagrange space */
template <typename T, int KPL, int N>
void tLweFFTAddMul(intSdCh in_streamA[KPL*N], intSdCh in_streamB[KPL*N], intSdCh out_stream[N]) {

	T C[N];
	for (int i = 0; i < N; i++)
		C[i] = 0;

	// do the computation
	L1:for (int j = 0; j < KPL; j++)
		L2:for (int i = 0; i < N; ++i)
			C[i] += (in_streamA[i + (j*N)].data) * (in_streamB[i + (j*N)].data);


	// stream back the results
	for (int j=0; j<N; j++){
		out_stream[j] =  push_stream<T>(C[j], j==1023);
	}
}

void top_function(intSdCh inStreamA[6*1024], intSdCh inStreamB[6*1024], intSdCh outStream[1024]) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=inStreamA
#pragma HLS INTERFACE axis port=inStreamB
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS

	tLweFFTAddMul<ap_int<32>, 6, 1024>(inStreamA, inStreamB, outStream);

}

// --------------------------------------------------------
// functions to insert and extract elements from an axi stream
// includes conversion to correct data type
template <typename T>
T pop_stream(intSdCh const &e)
{
#pragma HLS INLINE

	T ret = e.data;

	volatile ap_uint<32> strb = e.strb;
	volatile ap_uint<32> keep = e.keep;
	volatile ap_uint<2> user = e.user;
	volatile ap_uint<1> last = e.last;
	volatile ap_uint<5> id = e.id;
	volatile ap_uint<6> dest = e.dest;

	return ret;
}

template <typename T>
intSdCh push_stream(T const &v, bool last = false)
{
#pragma HLS INLINE

	intSdCh e;

	e.data = v;
	e.strb = (1<<sizeof(T))-1;
	e.keep = (1<<sizeof(T))-1;
	e.user = 0;
	e.last = last ? 1 : 0;
	e.id = 0;
	e.dest = 0;
	return e;
}
