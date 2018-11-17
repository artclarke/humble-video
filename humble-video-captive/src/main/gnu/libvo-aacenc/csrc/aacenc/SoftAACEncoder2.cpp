/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "SoftAACEncoder2"
#include <utils/Log.h>

#include "SoftAACEncoder2.h"

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/hexdump.h>

namespace android {

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

SoftAACEncoder2::SoftAACEncoder2(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component)
    : SimpleSoftOMXComponent(name, callbacks, appData, component),
      mAACEncoder(NULL),
      mNumChannels(1),
      mSampleRate(44100),
      mBitRate(0),
      mAACProfile(OMX_AUDIO_AACObjectLC),
      mSentCodecSpecificData(false),
      mInputSize(0),
      mInputFrame(NULL),
      mInputTimeUs(-1ll),
      mSawInputEOS(false),
      mSignalledError(false) {
    initPorts();
    CHECK_EQ(initEncoder(), (status_t)OK);
    setAudioParams();
}

SoftAACEncoder2::~SoftAACEncoder2() {
    aacEncClose(&mAACEncoder);

    delete[] mInputFrame;
    mInputFrame = NULL;
}

void SoftAACEncoder2::initPorts() {
    OMX_PARAM_PORTDEFINITIONTYPE def;
    InitOMXParams(&def);

    def.nPortIndex = 0;
    def.eDir = OMX_DirInput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = kNumSamplesPerFrame * sizeof(int16_t) * 2;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 1;

    def.format.audio.cMIMEType = const_cast<char *>("audio/raw");
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingPCM;

    addPort(def);

    def.nPortIndex = 1;
    def.eDir = OMX_DirOutput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 8192;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 2;

    def.format.audio.cMIMEType = const_cast<char *>("audio/aac");
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingAAC;

    addPort(def);
}

status_t SoftAACEncoder2::initEncoder() {
    if (AACENC_OK != aacEncOpen(&mAACEncoder, 0, 0)) {
        ALOGE("Failed to init AAC encoder");
        return UNKNOWN_ERROR;
    }
    return OK;
}

OMX_ERRORTYPE SoftAACEncoder2::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
    switch (index) {
        case OMX_IndexParamAudioPortFormat:
        {
            OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                (OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;

            if (formatParams->nPortIndex > 1) {
                return OMX_ErrorUndefined;
            }

            if (formatParams->nIndex > 0) {
                return OMX_ErrorNoMore;
            }

            formatParams->eEncoding =
                (formatParams->nPortIndex == 0)
                    ? OMX_AUDIO_CodingPCM : OMX_AUDIO_CodingAAC;

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAac:
        {
            OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;

            if (aacParams->nPortIndex != 1) {
                return OMX_ErrorUndefined;
            }

            aacParams->nBitRate = mBitRate;
            aacParams->nAudioBandWidth = 0;
            aacParams->nAACtools = 0;
            aacParams->nAACERtools = 0;
            aacParams->eAACProfile = (OMX_AUDIO_AACPROFILETYPE) mAACProfile;
            aacParams->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4FF;
            aacParams->eChannelMode = OMX_AUDIO_ChannelModeStereo;

            aacParams->nChannels = mNumChannels;
            aacParams->nSampleRate = mSampleRate;
            aacParams->nFrameLength = 0;

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (pcmParams->nPortIndex != 0) {
                return OMX_ErrorUndefined;
            }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            pcmParams->nChannels = mNumChannels;
            pcmParams->nSamplingRate = mSampleRate;

            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalGetParameter(index, params);
    }
}

OMX_ERRORTYPE SoftAACEncoder2::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
    switch (index) {
        case OMX_IndexParamStandardComponentRole:
        {
            const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                (const OMX_PARAM_COMPONENTROLETYPE *)params;

            if (strncmp((const char *)roleParams->cRole,
                        "audio_encoder.aac",
                        OMX_MAX_STRINGNAME_SIZE - 1)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPortFormat:
        {
            const OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                (const OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;

            if (formatParams->nPortIndex > 1) {
                return OMX_ErrorUndefined;
            }

            if (formatParams->nIndex > 0) {
                return OMX_ErrorNoMore;
            }

            if ((formatParams->nPortIndex == 0
                        && formatParams->eEncoding != OMX_AUDIO_CodingPCM)
                || (formatParams->nPortIndex == 1
                        && formatParams->eEncoding != OMX_AUDIO_CodingAAC)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAac:
        {
            OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;

            if (aacParams->nPortIndex != 1) {
                return OMX_ErrorUndefined;
            }

            mBitRate = aacParams->nBitRate;
            mNumChannels = aacParams->nChannels;
            mSampleRate = aacParams->nSampleRate;
            if (aacParams->eAACProfile != OMX_AUDIO_AACObjectNull) {
                mAACProfile = aacParams->eAACProfile;
            }

            if (setAudioParams() != OK) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (pcmParams->nPortIndex != 0) {
                return OMX_ErrorUndefined;
            }

            mNumChannels = pcmParams->nChannels;
            mSampleRate = pcmParams->nSamplingRate;
            if (setAudioParams() != OK) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalSetParameter(index, params);
    }
}

static CHANNEL_MODE getChannelMode(OMX_U32 nChannels) {
    CHANNEL_MODE chMode = MODE_INVALID;
    switch (nChannels) {
        case 1: chMode = MODE_1; break;
        case 2: chMode = MODE_2; break;
        case 3: chMode = MODE_1_2; break;
        case 4: chMode = MODE_1_2_1; break;
        case 5: chMode = MODE_1_2_2; break;
        case 6: chMode = MODE_1_2_2_1; break;
        default: chMode = MODE_INVALID;
    }
    return chMode;
}

static AUDIO_OBJECT_TYPE getAOTFromProfile(OMX_U32 profile) {
    if (profile == OMX_AUDIO_AACObjectLC) {
        return AOT_AAC_LC;
    } else if (profile == OMX_AUDIO_AACObjectHE) {
        return AOT_SBR;
    } else if (profile == OMX_AUDIO_AACObjectELD) {
        return AOT_ER_AAC_ELD;
    } else {
        ALOGW("Unsupported AAC profile - defaulting to AAC-LC");
        return AOT_AAC_LC;
    }
}

status_t SoftAACEncoder2::setAudioParams() {
    // We call this whenever sample rate, number of channels or bitrate change
    // in reponse to setParameter calls.

    ALOGV("setAudioParams: %lu Hz, %lu channels, %lu bps",
         mSampleRate, mNumChannels, mBitRate);

    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_AOT,
            getAOTFromProfile(mAACProfile))) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }

    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_SAMPLERATE, mSampleRate)) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_BITRATE, mBitRate)) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_CHANNELMODE,
            getChannelMode(mNumChannels))) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_TRANSMUX, TT_MP4_RAW)) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }

    return OK;
}

void SoftAACEncoder2::onQueueFilled(OMX_U32 portIndex) {
    if (mSignalledError) {
        return;
    }

    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);

    if (!mSentCodecSpecificData) {
        // The very first thing we want to output is the codec specific
        // data. It does not require any input data but we will need an
        // output buffer to store it in.

        if (outQueue.empty()) {
            return;
        }

        if (AACENC_OK != aacEncEncode(mAACEncoder, NULL, NULL, NULL, NULL)) {
            ALOGE("Unable to initialize encoder for profile / sample-rate / bit-rate / channels");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
            return;
        }

        OMX_U32 actualBitRate  = aacEncoder_GetParam(mAACEncoder, AACENC_BITRATE);
        if (mBitRate != actualBitRate) {
            ALOGW("Requested bitrate %lu unsupported, using %lu", mBitRate, actualBitRate);
        }

        AACENC_InfoStruct encInfo;
        if (AACENC_OK != aacEncInfo(mAACEncoder, &encInfo)) {
            ALOGE("Failed to get AAC encoder info");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
            return;
        }

        BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;
        outHeader->nFilledLen = encInfo.confSize;
        outHeader->nFlags = OMX_BUFFERFLAG_CODECCONFIG;

        uint8_t *out = outHeader->pBuffer + outHeader->nOffset;
        memcpy(out, encInfo.confBuf, encInfo.confSize);

        outQueue.erase(outQueue.begin());
        outInfo->mOwnedByUs = false;
        notifyFillBufferDone(outHeader);

        mSentCodecSpecificData = true;
    }

    size_t numBytesPerInputFrame =
        mNumChannels * kNumSamplesPerFrame * sizeof(int16_t);

    // Limit input size so we only get one ELD frame
    if (mAACProfile == OMX_AUDIO_AACObjectELD && numBytesPerInputFrame > 512) {
        numBytesPerInputFrame = 512;
    }

    for (;;) {
        // We do the following until we run out of buffers.

        while (mInputSize < numBytesPerInputFrame) {
            // As long as there's still input data to be read we
            // will drain "kNumSamplesPerFrame * mNumChannels" samples
            // into the "mInputFrame" buffer and then encode those
            // as a unit into an output buffer.

            if (mSawInputEOS || inQueue.empty()) {
                return;
            }

            BufferInfo *inInfo = *inQueue.begin();
            OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

            const void *inData = inHeader->pBuffer + inHeader->nOffset;

            size_t copy = numBytesPerInputFrame - mInputSize;
            if (copy > inHeader->nFilledLen) {
                copy = inHeader->nFilledLen;
            }

            if (mInputFrame == NULL) {
                mInputFrame = new int16_t[numBytesPerInputFrame / sizeof(int16_t)];
            }

            if (mInputSize == 0) {
                mInputTimeUs = inHeader->nTimeStamp;
            }

            memcpy((uint8_t *)mInputFrame + mInputSize, inData, copy);
            mInputSize += copy;

            inHeader->nOffset += copy;
            inHeader->nFilledLen -= copy;

            // "Time" on the input buffer has in effect advanced by the
            // number of audio frames we just advanced nOffset by.
            inHeader->nTimeStamp +=
                (copy * 1000000ll / mSampleRate)
                    / (mNumChannels * sizeof(int16_t));

            if (inHeader->nFilledLen == 0) {
                if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                    mSawInputEOS = true;

                    // Pad any remaining data with zeroes.
                    memset((uint8_t *)mInputFrame + mInputSize,
                           0,
                           numBytesPerInputFrame - mInputSize);

                    mInputSize = numBytesPerInputFrame;
                }

                inQueue.erase(inQueue.begin());
                inInfo->mOwnedByUs = false;
                notifyEmptyBufferDone(inHeader);

                inData = NULL;
                inHeader = NULL;
                inInfo = NULL;
            }
        }

        // At this  point we have all the input data necessary to encode
        // a single frame, all we need is an output buffer to store the result
        // in.

        if (outQueue.empty()) {
            return;
        }

        BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

        uint8_t *outPtr = (uint8_t *)outHeader->pBuffer + outHeader->nOffset;
        size_t outAvailable = outHeader->nAllocLen - outHeader->nOffset;

        AACENC_InArgs inargs;
        AACENC_OutArgs outargs;
        memset(&inargs, 0, sizeof(inargs));
        memset(&outargs, 0, sizeof(outargs));
        inargs.numInSamples = numBytesPerInputFrame / sizeof(int16_t);

        void* inBuffer[]        = { (unsigned char *)mInputFrame };
        INT   inBufferIds[]     = { IN_AUDIO_DATA };
        INT   inBufferSize[]    = { (INT)numBytesPerInputFrame };
        INT   inBufferElSize[]  = { sizeof(int16_t) };

        AACENC_BufDesc inBufDesc;
        inBufDesc.numBufs           = sizeof(inBuffer) / sizeof(void*);
        inBufDesc.bufs              = (void**)&inBuffer;
        inBufDesc.bufferIdentifiers = inBufferIds;
        inBufDesc.bufSizes          = inBufferSize;
        inBufDesc.bufElSizes        = inBufferElSize;

        void* outBuffer[]       = { outPtr };
        INT   outBufferIds[]    = { OUT_BITSTREAM_DATA };
        INT   outBufferSize[]   = { 0 };
        INT   outBufferElSize[] = { sizeof(UCHAR) };

        AACENC_BufDesc outBufDesc;
        outBufDesc.numBufs           = sizeof(outBuffer) / sizeof(void*);
        outBufDesc.bufs              = (void**)&outBuffer;
        outBufDesc.bufferIdentifiers = outBufferIds;
        outBufDesc.bufSizes          = outBufferSize;
        outBufDesc.bufElSizes        = outBufferElSize;

        // Encode the mInputFrame, which is treated as a modulo buffer
        AACENC_ERROR encoderErr = AACENC_OK;
        size_t nOutputBytes = 0;

        do {
            memset(&outargs, 0, sizeof(outargs));

            outBuffer[0] = outPtr;
            outBufferSize[0] = outAvailable - nOutputBytes;

            encoderErr = aacEncEncode(mAACEncoder,
                                      &inBufDesc,
                                      &outBufDesc,
                                      &inargs,
                                      &outargs);

            if (encoderErr == AACENC_OK) {
                outPtr += outargs.numOutBytes;
                nOutputBytes += outargs.numOutBytes;

                if (outargs.numInSamples > 0) {
                    int numRemainingSamples = inargs.numInSamples - outargs.numInSamples;
                    if (numRemainingSamples > 0) {
                        memmove(mInputFrame,
                                &mInputFrame[outargs.numInSamples],
                                sizeof(int16_t) * numRemainingSamples);
                    }
                    inargs.numInSamples -= outargs.numInSamples;
                }
            }
        } while (encoderErr == AACENC_OK && inargs.numInSamples > 0);

        outHeader->nFilledLen = nOutputBytes;

        outHeader->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;

        if (mSawInputEOS) {
            // We also tag this output buffer with EOS if it corresponds
            // to the final input buffer.
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;
        }

        outHeader->nTimeStamp = mInputTimeUs;

#if 0
        ALOGI("sending %d bytes of data (time = %lld us, flags = 0x%08lx)",
              nOutputBytes, mInputTimeUs, outHeader->nFlags);

        hexdump(outHeader->pBuffer + outHeader->nOffset, outHeader->nFilledLen);
#endif

        outQueue.erase(outQueue.begin());
        outInfo->mOwnedByUs = false;
        notifyFillBufferDone(outHeader);

        outHeader = NULL;
        outInfo = NULL;

        mInputSize = 0;
    }
}

}  // namespace android

android::SoftOMXComponent *createSoftOMXComponent(
        const char *name, const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData, OMX_COMPONENTTYPE **component) {
    return new android::SoftAACEncoder2(name, callbacks, appData, component);
}
