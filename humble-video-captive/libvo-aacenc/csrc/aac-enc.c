/* ------------------------------------------------------------------
 * Copyright (C) 2011 Martin Storsjo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <common/include/voAAC.h>
#include <common/include/cmnMemory.h>
#include "wavreader.h"

void usage(const char* name) {
	fprintf(stderr, "%s [-r bitrate] in.wav out.aac\n", name);
}

int main(int argc, char *argv[]) {
	int bitrate = 64000;
	int ch;
	const char *infile, *outfile;
	FILE *out;
	void *wav;
	int format, sampleRate, channels, bitsPerSample;
	int inputSize;
	uint8_t* inputBuf;
	int16_t* convertBuf;
	VO_AUDIO_CODECAPI codec_api = { 0 };
	VO_HANDLE handle = 0;
	VO_MEM_OPERATOR mem_operator = { 0 };
	VO_CODEC_INIT_USERDATA user_data;
	AACENC_PARAM params = { 0 };
	while ((ch = getopt(argc, argv, "r:")) != -1) {
		switch (ch) {
		case 'r':
			bitrate = atoi(optarg);
			break;
		case '?':
		default:
			usage(argv[0]);
			return 1;
		}
	}
	if (argc - optind < 2) {
		usage(argv[0]);
		return 1;
	}
	infile = argv[optind];
	outfile = argv[optind + 1];

	wav = wav_read_open(infile);
	if (!wav) {
		fprintf(stderr, "Unable to open wav file %s\n", infile);
		return 1;
	}
	if (!wav_get_header(wav, &format, &channels, &sampleRate, &bitsPerSample, NULL)) {
		fprintf(stderr, "Bad wav file %s\n", infile);
		return 1;
	}
	if (format != 1) {
		fprintf(stderr, "Unsupported WAV format %d\n", format);
		return 1;
	}
	if (bitsPerSample != 16) {
		fprintf(stderr, "Unsupported WAV sample depth %d\n", bitsPerSample);
		return 1;
	}
	inputSize = channels*2*1024;
	inputBuf = (uint8_t*) malloc(inputSize);
	convertBuf = (int16_t*) malloc(inputSize);

	voGetAACEncAPI(&codec_api);

	mem_operator.Alloc = cmnMemAlloc;
	mem_operator.Copy = cmnMemCopy;
	mem_operator.Free = cmnMemFree;
	mem_operator.Set = cmnMemSet;
	mem_operator.Check = cmnMemCheck;
	user_data.memflag = VO_IMF_USERMEMOPERATOR;
	user_data.memData = &mem_operator;
	codec_api.Init(&handle, VO_AUDIO_CodingAAC, &user_data);

	params.sampleRate = sampleRate;
	params.bitRate = bitrate;
	params.nChannels = channels;
	params.adtsUsed = 1;
	if (codec_api.SetParam(handle, VO_PID_AAC_ENCPARAM, &params) != VO_ERR_NONE) {
		fprintf(stderr, "Unable to set encoding parameters\n");
		return 1;
	}

	out = fopen(outfile, "wb");
	if (!out) {
		perror(outfile);
		return 1;
	}

	while (1) {
		VO_CODECBUFFER input = { 0 }, output = { 0 };
		VO_AUDIO_OUTPUTINFO output_info = { 0 };
		int read, i;
		uint8_t outbuf[20480];

		read = wav_read_data(wav, inputBuf, inputSize);
		if (read < inputSize)
			break;
		for (i = 0; i < read/2; i++) {
			const uint8_t* in = &inputBuf[2*i];
			convertBuf[i] = in[0] | (in[1] << 8);
		}
		input.Buffer = (uint8_t*) convertBuf;
		input.Length = read;
		codec_api.SetInputData(handle, &input);

		output.Buffer = outbuf;
		output.Length = sizeof(outbuf);
		if (codec_api.GetOutputData(handle, &output, &output_info) != VO_ERR_NONE) {
			fprintf(stderr, "Unable to encode frame\n");
			return 1;
		}
		fwrite(outbuf, 1, output.Length, out);
	}
	free(inputBuf);
	free(convertBuf);
	fclose(out);
	codec_api.Uninit(handle);
	wav_read_close(wav);

	return 0;
}

