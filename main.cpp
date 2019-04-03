/*
 * main.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: xuleilx
 */

#include "ringbuffer.h"

/* add start by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */
#define CONSUMER_LENGTH 3528
static RingBuffer *ringBuf = NULL;
char consumerData[CONSUMER_LENGTH] = {'\0'};
/* add end by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */

static void captureCallBack(void *pAppleHandle, const void *inBuffer, int inLen)
{
    if(*((int*)pAppleHandle) >= 0)
    {
        /* mod start by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */
        int rc = RingBuffer_write(ringBuf, inBuffer, inLen);
        if(-1 == rc){
            // if ringbuffer no space ,clear ringbuffer and retry write data.
            RingBuffer_clear(ringBuf);
            RingBuffer_write(ringBuf, inBuffer, inLen);
        }
        /* mod end by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */
    }
}

/* add start by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */
void * consumerThread(void * arg){
    int rc = 0;
    while(1){
        rc = RingBuffer_read(ringBuf, consumerData, sizeof(consumerData));
        if((-1 != rc) && (iFDPlay >= 0)){
            gstreamer_play_pcm_buffer(iFDPlay, consumerData, sizeof(consumerData));
        }
        usleep(10000);
    }
}
/* add end by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */

int main()
{
    /* add start by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */
    // create ringbuf
    int ringBufSize = 176400; //default 44100*2*2 --> 1s

    pEnv = getenv("RINGBUF_SIZE");
    if(NULL != pEnv)
    {
        ringBufSize = atoi(pEnv);
        INFO("ringBufSize:%d",ringBufSize);
    }

    if(ringBuf == NULL){
        ringBuf = RingBuffer_create(ringBufSize);
    }

    // create consumerThread
    pthread_t consumerThreadId;
    ret = pthread_create(&consumerThreadId,NULL,consumerThread,NULL);
    if(ret != 0)
    {
        ERROR("consumerThread create failed!");
    }
    pthread_detach(consumerThreadId);
    /* add end by xulei@hsaeyz 20180502 use ringbuf to drop data,for pause musice delay */

    return 0;
}
