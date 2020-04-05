#include <hls_stream.h>
#include <ap_axi_sdata.h>

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

void top_function(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream) {

#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=inStream
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS


	for (int i = 0; i < 1000; i++) {

		// Read and cache (will block if FIFO sender is empty
		intSdCh valueIn = inStream.read();
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
