#include <hls_stream.h>
#include <ap_axi_sdata.h>

// declare 32 bit integer with side-channel
typedef ap_axis<32, 2, 5, 6> intSdCh;

void top_function(intSdCh inStreamA[6*1024], intSdCh inStreamB[6*1024], intSdCh outStream[1024]);

int main()
{
	// prepare data for DUT
	intSdCh inputStreamA[6*1024];
	intSdCh inputStreamB[6*1024];
	intSdCh outputStream[1024];

	for (int i = 0; i < 1024; i++) {
		intSdCh valueIn;
		valueIn.data = i;
		valueIn.keep = 1;
		valueIn.strb = 1;
		valueIn.user = 1;
		valueIn.last = 1;
		valueIn.id = 0;
		valueIn.dest = 0;
		inputStreamA[i] = valueIn;
		inputStreamB[i] = valueIn;
	}

	for (int i = 0; i < 1024; i++) {
			intSdCh valueIn;
			valueIn.data = i;
			valueIn.keep = 1;
			valueIn.strb = 1;
			valueIn.user = 1;
			valueIn.last = 1;
			valueIn.id = 0;
			valueIn.dest = 0;
			inputStreamA[i+1024] = valueIn;
			inputStreamB[i+1024] = valueIn;
		}

	top_function(inputStreamA, inputStreamB, outputStream);

	for (int i = 0; i < 1024; i++) {
		printf("Value is %d\n", (int)outputStream[i].data);
	}

	return 0;
}
