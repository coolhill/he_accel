#include <hls_stream.h>
#include <ap_axi_sdata.h>

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

void top_function(hls::stream<intSdCh> &inStream, hls::stream<intSdCh> &outStream);

int main()
{
	hls::stream<intSdCh> inputStream;
	hls::stream<intSdCh> outputStream;

	for (int i = 0; i < 1000; i++) {
		intSdCh valueIn;
		valueIn.data = i;
		valueIn.keep = 1;
		valueIn.strb = 1;
		valueIn.user = 1;
		valueIn.last = 1;
		valueIn.id = 0;
		valueIn.dest = 0;
		inputStream << valueIn;
	}

	top_function(inputStream, outputStream);

	for (int i = 0; i < 1000; i++) {
		intSdCh valueOut;
		outputStream.read(valueOut);
		printf("Value is %d\n", (int)valueOut.data);
	}

	return 0;
}
