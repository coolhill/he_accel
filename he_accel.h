#include <hls_stream.h>
#include <ap_axi_sdata.h>

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

void top_function(hls::stream<intSdCh> inStreamA, hls::stream<intSdCh> inStreamB, hls::stream<intSdCh> outStream);

template <typename T, int KPL, int N>
void tLweFFTAddMulRTo(T R[N], T A[KPL][N], T B[KPL][N]);


template <typename T, int DIM, int SIZE, int U, int TI, int TD>
void wrapped_tLweFFTAddMulRTo(intSdCh in_streamA[SIZE], intSdCh in_streamB[SIZE], intSdCh out_stream[SIZE]);

template <typename T>
T pop_stream(intSdCh const &e);

template <typename T>
intSdCh push_stream(ap_uint<32> const &v, bool last);
