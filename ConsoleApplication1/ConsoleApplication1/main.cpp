
//只用于单通道,注意数据类型，只能8k16位
#define WEBRTC_WIN
#define WEBRTC_AUDIO_PROCESSING_ONLY_BUILD
#include"webrtc/modules/audio_processing/ns/include/noise_suppression.h"
#define DR_WAV_IMPLEMENTATION//使用drwav必须定义这个宏

#include"dr_wav.h"

#include<cstdio>
#include<cstdlib>
#include <cstdint>
int main() {
	unsigned int channels = 0;
	unsigned int sampleRate = 0;
	drwav_uint64 totalSampleCount = 0;
	drwav_int16* pSampleData = drwav_open_and_read_file_s16("1.wav", &channels, &sampleRate, &totalSampleCount);//默认的就是转成16bit,所以写的时候也要改成16bit
	if (pSampleData == NULL) {
		printf("open failed");//Error opening and reading WAV file.
		getchar();
		return 0;
	}
	float *in = new float[totalSampleCount];

	for (size_t i = 0; i <totalSampleCount; i++)
	{
		in[i] = pSampleData[i];
	}
	int framesize = sampleRate / 100;
	drwav_int16 nframe = totalSampleCount / framesize;

	int level = 3;
	NsHandle *nshandle = WebRtcNs_Create();
	int status=WebRtcNs_Init(nshandle, sampleRate);

	if (status != 0) {
		printf("WebRtcNs_Init fail\n");
		return -1;
	}

	WebRtcNs_set_policy(nshandle, level);


	drwav_uint64 len = nframe * framesize;
	float *out = new float[len];
	float*pin = &in[0];
	float*pout = &out[0];
	drwav_int16 *outout = new drwav_int16[len];
	for (int  i = 0; i <nframe; i++)
	{

		//float  *outout=new float[framesize];
		WebRtcNs_Analyze(nshandle,pin);
		WebRtcNs_Process(nshandle,&pin,1,&pout);
	
		for (int j = 0; j < framesize; j++)
		{
			outout[i*framesize+j] = pout[j];
		}
	
		pin += framesize;
		pout += framesize;
	}



	WebRtcNs_Free(nshandle);

	drwav_data_format format;
	format.container = drwav_container_riff;     // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
	format.format = DR_WAVE_FORMAT_PCM;          // <-- Any of the DR_WAVE_FORMAT_* codes.
	format.channels = channels;
	format.sampleRate = sampleRate;
	format.bitsPerSample = 16;
	drwav* pWav = drwav_open_file_write("2.wav", &format);

	drwav_uint64 samplesWritten = drwav_write(pWav, len,outout);
	drwav_free(pSampleData);
	if (samplesWritten>0)
	{
		printf("success");
	}
	


	getchar();
     return 0;

}


