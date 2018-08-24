#include <jni.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "./ilbc_src/iLBC_define.h"
#include "./ilbc_src/iLBC_decode.h"
#include "./ilbc_src/iLBC_encode.h"

#define LOG_TAG "audiowrapper"

#ifdef BUILD_FROM_SOURCE
#include <utils/Log.h>
#else
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__) 
#endif

#define JNI_COPY    0
//编解码时用到的两个关键的数据结构
static iLBC_Enc_Inst_t g_enc_inst;
static iLBC_Dec_Inst_t g_dec_inst;

//JavaVM *gJavaVM;  //创建javavm的全局变量

/* 编码一次
 */
static int encode(short *samples, unsigned char *data) {
	int i;
	float block[BLOCKL_MAX];//240
	// Convert to float representaion of voice signal.
	for (i = 0; i < g_enc_inst.blockl; i++) {
		block[i] = samples[i];
//		LOGD("encode samples:%d(%x)",i,samples[i]);
	}
	iLBC_encode(data, block, &g_enc_inst);
	return g_enc_inst.no_of_bytes;
}

/*解码一次
 */
static int decode(unsigned char *data, short *samples, int mode) {
	int i;
	float block[BLOCKL_MAX];//240
	// Validate Mode
	if (mode != 0 && mode != 1) {
		LOGE("Bad mode");
		return -1;
	}

	iLBC_decode(block, data, &g_dec_inst, mode);
	// Validate PCM16
	for (i = 0; i < g_dec_inst.blockl; i++) {
		float point;
		point = block[i];
		if (point < MIN_SAMPLE) { //-32768
			point = MIN_SAMPLE;
		} else if (point > MAX_SAMPLE) {  //32767
			point = MAX_SAMPLE;
		}
//		samples[i] = point;
		samples[i] = (short)((int)point >> 8 | (int)point << 8 & 0xff);
//		LOGD("decode point:%d(%f->%d->%x)",i,point,samples[i],samples[i]);
	}
	return g_dec_inst.blockl * 2;
}

jint Java_com_audio_lib_AudioCodec_audio_1codec_1init(JNIEnv *env,jobject this, jint mode) {
	initEncode(&g_enc_inst, mode);
	initDecode(&g_dec_inst, mode, 1);
//	(*env)->GetJavaVM(env,&gJavaVM);//来获取javavm指针,保存到vm中. env是当前线程的变量.
}


jint Java_com_audio_lib_AudioCodec_audio_1encode(JNIEnv *env,jobject this,
		jbyteArray sampleArray, jint sampleOffset,jint sampleLength,
		jbyteArray dataArray, jint dataOffset) {

	jsize samples_sz, data_sz;
	jbyte *samples, *data;
	int bytes_to_encode;
	int bytes_encoded;
//	JNIEnv *env;
//	(*gJavaVM)->AttachCurrentThread(gJavaVM,&env, NULL);

//	LOGD("encode(%p, %d, %d,%p, %d)",
//			sampleArray, sampleOffset, sampleLength,
//			dataArray, dataOffset);
	samples_sz = (*env)->GetArrayLength(env, sampleArray);
	samples = (*env)->GetByteArrayElements(env, sampleArray, JNI_COPY);
	data_sz = (*env)->GetArrayLength(env, dataArray);
	data = (*env)->GetByteArrayElements(env, dataArray, JNI_COPY);

	samples += sampleOffset;//输入偏移地址
	data += dataOffset;//输出偏移地址

	bytes_to_encode = sampleLength;//输入数据长度
	bytes_encoded = 0;//编码后数据长度

	int truncated = bytes_to_encode % (g_enc_inst.blockl * 2);//30->240*2   20->160*2
	if (!truncated) {
		LOGE("Ignore last %d bytes", truncated);
		bytes_to_encode -= truncated;
	}

	while (bytes_to_encode > 0)
	{
		int _encoded;
		_encoded = encode((short *) samples, data);//输出->50
		samples += g_enc_inst.blockl * 2;//输入数据偏移
		data += _encoded; //编码后存储位置偏移
		bytes_encoded += _encoded;//编码后数据长度
		bytes_to_encode -= g_enc_inst.blockl * 2;
	}

    //LOGD("encode len:(%d->%d)",sampleLength,bytes_encoded);
	samples -= sampleLength;//输入数据指针回到起始
	data -= bytes_encoded;  //输出数据指针回到起始
	(*env)->ReleaseByteArrayElements(env, sampleArray, samples, JNI_COPY);
	(*env)->ReleaseByteArrayElements(env, dataArray, data, JNI_COPY);
    //(*gJavaVM)->DetachCurrentThread(gJavaVM);
	return bytes_encoded;
}

jint Java_com_audio_lib_AudioCodec_audio_1decode(JNIEnv *env,jobject this,
		jbyteArray dataArray, jint dataOffset, jint dataLength,
		jbyteArray sampleArray, jint sampleOffset) {
	jsize samples_sz, data_sz;
	jbyte *samples, *data;
	int bytes_to_decode, bytes_decoded;

//	LOGD("decode(%p, %d, %d,%p,%d)",
//			    dataArray, dataOffset, dataLength,
//			    sampleArray, sampleOffset);

	samples_sz = (*env)->GetArrayLength(env, sampleArray);
	samples = (*env)->GetByteArrayElements(env, sampleArray, JNI_COPY);
	data_sz = (*env)->GetArrayLength(env, dataArray);
	data = (*env)->GetByteArrayElements(env, dataArray, JNI_COPY);

	samples += sampleOffset;
	data += dataOffset;

	bytes_to_decode = dataLength;
	bytes_decoded = 0;
	while (bytes_to_decode > 0)
	{
		int _decoded;
		_decoded = decode(data, (short *)samples, 1); //480
		samples += _decoded;//30->240*2=480    20->160*2=320
		data += g_dec_inst.no_of_bytes;//30->50   20->38
		bytes_decoded += _decoded;
		bytes_to_decode -= g_dec_inst.no_of_bytes;
	}
    //LOGD("decode len:(%d->%d)",dataLength,bytes_decoded);
	samples -= bytes_decoded;
	data -= dataLength;
	(*env)->ReleaseByteArrayElements(env, sampleArray, samples, JNI_COPY);
	(*env)->ReleaseByteArrayElements(env, dataArray, data, JNI_COPY);
	return bytes_decoded;
}


