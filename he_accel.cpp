#include <hls_stream.h>
#include <ap_axi_sdata.h>

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

void top_function(hls::stream<intSdCh> &inStreamA, hls::stream<intSdCh> &inStreamB, hls::stream<intSdCh> &outStream) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=inStreamA
#pragma HLS INTERFACE axis port=inStreamB
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS


	for (int i = 0; i < 1000; i++) {

		// Read and cache (will block if FIFO sender is empty
		intSdCh valueIn = inStreamA.read();
		intSdCh valueOut;

		valueOut.data = valueIn.data * 5;

		// copy other side-channels
		valueOut.keep = valueIn.keep;
		valueOut.strb = valueIn.strb;
		valueOut.user = valueIn.user;
		valueOut.last = valueIn.last;
		valueOut.id = valueIn.id;
		valueOut.dest = valueIn.dest;

		outStream.write(valueOut);
	}
}

/** termwise multiplication and addTo in Lagrange space */
template <typename T, int KPL, int N>
void tLweFFTAddMulRTo(T R[N], T A[KPL][N], T B[KPL][N])   {

	// result = result + p*sample
	for (int p = 0; p < KPL; ++p){
		for (int i = 0; i < N; ++i){
			R[i] += A[p][i]*B[p][i];
		}
	}
}

template <typename T, int DIM, int SIZE, int U, int TI, int TD>
void wrapped_tLweFFTAddMulRTo(intSdCh in_stream[2*SIZE], intSdCh out_stream[SIZE]) {

	ap_uint<32> A[6][1024], B[6][1024], C[1024];
	// stream in the 2 input matrices
	for(int i=0; i<6; i++)
		for(int j=0; j<1024; j++){
			int k = i*6 + j;
			A[i][j] = pop_stream(in_stream[k]);
		}


	for (int i=0; i<6; i++)
		for (int j=0; j<DIM; j++){
			int k = i*6 + j + 1024;
			B[i][j] = pop_stream(in_stream[k]);
		}

	tLweFFTAddMulRTo<ap_uint<32>, 6, 1024>(C, A, B);
}

// --------------------------------------------------------
// functions to insert and extract elements from an axi stream
// includes conversion to correct data type
ap_uint<32> pop_stream(intSdCh const &e)
{
#pragma HLS INLINE

	ap_uint<32> ret = e.data;

	volatile ap_uint<32> strb = e.strb;
	volatile ap_uint<32> keep = e.keep;
	volatile ap_uint<2> user = e.user;
	volatile ap_uint<1> last = e.last;
	volatile ap_uint<5> id = e.id;
	volatile ap_uint<6> dest = e.dest;

	return ret;
}

intSdCh push_stream(ap_uint<32> const &v, bool last = false)
{
#pragma HLS INLINE

	intSdCh e;

	e.data = v;
	e.strb = (1<<32)-1;
	e.keep = (1<<32)-1;
	e.user = 0;
	e.last = last ? 1 : 0;
	e.id = 0;
	e.dest = 0;
	return e;
}
