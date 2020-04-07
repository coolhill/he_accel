#include <hls_stream.h>
#include <ap_axi_sdata.h>

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

template <typename T>
T pop_stream(intSdCh const &e);

template <typename T>
intSdCh push_stream(ap_uint<32> const &v, bool last);
