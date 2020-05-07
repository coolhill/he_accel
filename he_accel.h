#include <iostream>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>

typedef ap_axis<32, 1,1,1> int_axis;


#include "ap_fixed.h"
#include "hls_fft.h"

#include <complex>
using namespace std;



// configurable params
const char FFT_INPUT_WIDTH                     = 32;
const char FFT_OUTPUT_WIDTH                    = FFT_INPUT_WIDTH;
const char FFT_CONFIG_WIDTH                    = 16;
const char FFT_NFFT_MAX                        = 10;
const int  FFT_LENGTH                          = 1 << FFT_NFFT_MAX;

#include <complex>
using namespace std;

struct config1 : hls::ip_fft::params_t {
    static const unsigned ordering_opt = hls::ip_fft::natural_order;
    static const unsigned config_width = FFT_CONFIG_WIDTH;
    static const unsigned max_nfft = FFT_NFFT_MAX;
    static const unsigned input_width = FFT_INPUT_WIDTH;
    static const unsigned output_width = FFT_OUTPUT_WIDTH;
};

typedef hls::ip_fft::config_t<config1> config_t;
typedef hls::ip_fft::status_t<config1> status_t;

typedef ap_fixed<FFT_INPUT_WIDTH,1> data_in_t;
typedef ap_fixed<FFT_OUTPUT_WIDTH,FFT_OUTPUT_WIDTH-FFT_INPUT_WIDTH+1> data_out_t;
typedef std::complex<data_in_t> cmpxDataIn;
typedef std::complex<data_out_t> cmpxDataOut;

struct complex_axi_type{
	cmpxDataIn data;
    ap_uint<(32+7)/8> keep;
    ap_uint<(32+7)/8> strb;
    ap_uint<1>        user;
    ap_uint<1>        last;
    ap_uint<1>        id;
    ap_uint<1>        dest;
};
