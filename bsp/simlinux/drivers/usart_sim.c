#include  <rthw.h>
#include  <rtthread.h>

#ifdef _WIN32
#include  <windows.h>
#include  <mmsystem.h>
#include  <conio.h>
#endif

#include  <stdio.h>
#include "serial.h"

struct serial_int_rx serial_rx;
extern struct rt_device serial_device;

#ifdef _WIN32
/*
 * Handler for OSKey Thread
 */
static HANDLE       OSKey_Thread;
static DWORD        OSKey_ThreadID;

static DWORD WINAPI ThreadforKeyGet(LPVOID lpParam);
void rt_hw_usart_init(void)
{
    /*
     * create serial thread that receive key input from keyboard
     */

    OSKey_Thread = CreateThread(NULL,
                                0,
                                (LPTHREAD_START_ROUTINE)ThreadforKeyGet,
                                0,
                                CREATE_SUSPENDED,
                                &OSKey_ThreadID);
    if (OSKey_Thread == NULL)
    {
        //Display Error Message

        return;
    }
    SetThreadPriority(OSKey_Thread,
                      THREAD_PRIORITY_NORMAL);
    SetThreadPriorityBoost(OSKey_Thread,
                           TRUE);
    SetThreadAffinityMask(OSKey_Thread,
                          0x01);
    /*
     * Start OS get key Thread
     */
    ResumeThread(OSKey_Thread);
}

#else /* POSIX version */

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h> /* for tcxxxattr, ECHO, etc */
#include <unistd.h> /* for STDIN_FILENO */


static void * ThreadforKeyGet(void * lpParam);
static pthread_t OSKey_Thread;
void rt_hw_usart_init(void)
{
    int res;
    res = pthread_create(&OSKey_Thread, NULL, &ThreadforKeyGet, NULL);
    if (res)
    {
        printf("pthread create faild, <%d>\n", res);
        exit(EXIT_FAILURE);
    }
}
#endif
/*
 * �����(��)�� 0xe04b
 * �����(��)�� 0xe048
 * �����(��)�� 0xe04d
 * �����(��)�� 0xe050
 */
static int savekey(unsigned char key)
{
    /* save on rx buffer */
    {
        rt_base_t level;

        /* disable interrupt */
        //��ʱ�ر��жϣ���ΪҪ����uart���ݽṹ
        level = rt_hw_interrupt_disable();

        /* save character */
        serial_rx.rx_buffer[serial_rx.save_index] = key;
        serial_rx.save_index ++;
        //����Ĵ�����save_index�Ƿ��Ѿ�����������β������������ת��ͷ������Ϊһ�����λ�����
        if (serial_rx.save_index >= SERIAL_RX_BUFFER_SIZE)
            serial_rx.save_index = 0;

        //���������ʾ��ת���save_index׷����read_index��������read_index������һ���ɵ�����
        /* if the next position is read index, discard this 'read char' */
        if (serial_rx.save_index == serial_rx.read_index)
        {
            serial_rx.read_index ++;
            if (serial_rx.read_index >= SERIAL_RX_BUFFER_SIZE)
                serial_rx.read_index = 0;
        }

        /* enable interrupt */
        //uart���ݽṹ�Ѿ�������ɣ�����ʹ���ж�
        rt_hw_interrupt_enable(level);
    }

    /* invoke callback */
    if (serial_device.rx_indicate != RT_NULL)
    {
        rt_size_t rx_length;

        /* get rx length */
        rx_length = serial_rx.read_index > serial_rx.save_index ?
                    SERIAL_RX_BUFFER_SIZE - serial_rx.read_index + serial_rx.save_index :
                    serial_rx.save_index - serial_rx.read_index;

        serial_device.rx_indicate(&serial_device, rx_length);
    }
    return 0; 
}
#ifdef _WIN32
static DWORD WINAPI ThreadforKeyGet(LPVOID lpParam)
#else

/*simulate windows' getch(), it works!!*/
static void setgetchar(void) 
{
    struct termios oldt, newt;

	// get terminal input's attribute
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

	//set termios' local mode
    newt.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
#define getch  getchar

static void * ThreadforKeyGet(void * lpParam)
#endif /* not _WIN32*/
{
    unsigned char key;

#ifndef _WIN32
    sigset_t  sigmask, oldmask;
	/* set the getchar without buffer */
	sigfillset(&sigmask);
	pthread_sigmask(SIG_BLOCK, &sigmask, &oldmask);
 	setgetchar();
#endif
    (void)lpParam;              //prevent compiler warnings
    for (;;)
    {
        key = getch();
#ifdef _WIN32
        if (key == 0xE0)
        {
            key = getch();

            if (key == 0x48) //up key , 0x1b 0x5b 0x41
            {
                savekey(0x1b);
                savekey(0x5b);
                savekey(0x41);
            }
            else if (key == 0x50)//0x1b 0x5b 0x42
            {
                savekey(0x1b);
                savekey(0x5b);
                savekey(0x42);
            }

            continue;
        }
#endif
        savekey(key);
    }
} /*** ThreadforKeyGet ***/
