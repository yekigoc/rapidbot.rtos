#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <debug_macros.h>
#include <rgi.pb-c.h>
#include <usbcommon.h>
#include <stdlib.h>
#include <memory.h>
#include <joystick.h>
#include <pthread.h>


#define MYID 10
#define c1 (double)104.0/32767.0
//#define c1 (double)60.0/32767.0
//#define c2 (double)125.0/32767.0
#define c2 (double)30.0/32767.0

struct js_event jse;
int fd;
int rc;
struct carpad titc;
int pressed1;

unsigned int portno = 30000;
char hostname[] = "localhost";

int login(int * sd)
{
  Rgi__Tellid tellid = RGI__TELLID__INIT;
  tellid.id = MYID;
  unsigned short sz = rgi__tellid__get_packed_size(&tellid);
  char *buffer = (char*)malloc(sz*(sizeof(char)));
  rgi__tellid__pack(&tellid, buffer);
  unsigned short head = PER_TELLID;
  if(write(*sd, &head, 2)<0)
    return -1;
  if(write(*sd, &sz, 2)<0)
    return -1;
  if(write(*sd, buffer, sz)<0)
    return -1;
  free (buffer);
}

int sendpwm(int *sd)
{
  Rgi__Setpwmchan pwmchan = RGI__SETPWMCHAN__INIT;
  pwmchan.channel = 0;
  double k1 = ((double)titc.leftstickx);
  pwmchan.value = 396+k1*(c1);
  double pm = 396.0+k1*(c1);
  pmsg("channel 0 = %f\n", pm);
  unsigned short sz = rgi__setpwmchan__get_packed_size(&pwmchan);
  char * buffer = (char*)malloc(sz*(sizeof(char)));
  rgi__setpwmchan__pack(&pwmchan, buffer);
  unsigned short head = PER_PWM;
  if (write(*sd, &head, 2)<0)
    return -1;
  if (write(*sd, &sz, 2)<0)
    return -1;
  if (write(*sd, buffer, sz)<0)
    return -1;
  free (buffer);

  pwmchan.channel = 1;
  double k2 = ((((double)titc.rightsticky)));
  pwmchan.value = 374+k2*(c2);
  double sm = 374.0+k2*(c2);
  pmsg("channel 1 = %f\n", sm);
  sz = rgi__setpwmchan__get_packed_size(&pwmchan);
  buffer = (char*)malloc(sz*(sizeof(char)));
  rgi__setpwmchan__pack(&pwmchan, buffer);

  if (write(*sd, &head, 2)<0)
    return -1;
  if (write(*sd, &sz, 2)<0)
    return -1;
  if (write(*sd, buffer, sz)<0)
    return -1;
  free (buffer);
}

int lockusb(int * sd)
{
  Rgi__Setlock lock = RGI__SETLOCK__INIT;
  lock.lock = 1;
  unsigned short sz = rgi__setlock__get_packed_size(&lock);
  char *buffer = (char*)malloc(sz*(sizeof(char)));
  rgi__setlock__pack(&lock, buffer);
  unsigned short head = PER_SETLOCK;
  if(write(*sd, &head, 2)<0)
    {
      pmsg ("short write (head)\n");
      return -1;
    }
  if(write(*sd, &sz, 2)<0)
    {
      pmsg ("short write (sz)\n");
      return -1;
    }
  if(write(*sd, buffer, sz)<0)
    {
      pmsg ("short write (buffer)\n");
      return -1;
    }

  pmsg("reading reply\n");
  int ret = 0;
  read(*sd, &head, 2);
  head = ntohs(head);
  pmsg ("head = %i\n", head);
  pmsg("reading size\n");

  read(*sd, &sz, 2);
  sz = ntohs(sz);
  pmsg ("sz = %i\n", sz);
  pmsg ("head = %i\n", head);
  if (head == PER_TELLREADY)
    {
      pmsg ("PER_TELLREADY\n");
      ret = 0;
    }
  else
    {
      pmsg ("PER_TELLLOCKED\n");
      ret = 1;
    }
  pmsg ("reading rest data\n");
  char buf[16];
  read(*sd, buf, sz);
      
  return ret;
}

int unlockusb(int * sd)
{
  Rgi__Setunlock unlock = RGI__SETUNLOCK__INIT;
  unlock.lock = 1;
  unsigned short sz = rgi__setunlock__get_packed_size(&unlock);
  char *buffer = (char*)malloc(sz*(sizeof(char)));
  rgi__setunlock__pack(&unlock, buffer);
  unsigned short head = PER_SETUNLOCK;
  if(write(*sd, &head, 2)<0)
    {
      pmsg ("short write (head)\n");
      return -1;
    }
  if(write(*sd, &sz, 2)<0)
    {
      pmsg ("short write (sz)\n");
      return -1;
    }
  if(write(*sd, buffer, sz)<0)
    {
      pmsg ("short write (buffer)\n");
      return -1;
    }

  int ret = 0;
  pmsg("reading reply\n");
  read(*sd, &head, 2);
  head = ntohs(head);
  pmsg ("head = %i\n", head);
  pmsg("reading size\n");

  read(*sd, &sz, 2);
  sz = ntohs(sz);
  pmsg ("sz = %i\n", sz);
  pmsg ("head = %i\n", head);
  if (head == PER_TELLREADY)
    {
      pmsg ("PER_TELLREADY\n");
      ret = 0;
    }
  else
    {
      pmsg ("PER_TELLLOCKED\n");
      ret = 1;
    }
  pmsg ("reading rest data\n");
  char buf[16];
  read(*sd, buf, sz);

  return ret;
}

static void *thread_func(void *vptr_args)
{
  while (1)
    {
      rc = read_joystick_event(&jse);

      if (rc == 1) 
	{
	  if (jse.type != 129)
	    {
	      switch (jse.number) {
	      case 0: 
		if (jse.type==2)
		  {
		    if (jse.value==0)
		      titc.leftstickx=0;
		    else
		      titc.leftstickx = jse.value;
		  }
		break;
	      case 3:
		if (jse.type==2)		 
		  {
		    if (jse.value==0)
		      titc.rightsticky = 0;
		    else
		      titc.rightsticky = -jse.value;
		  }
		break;
	      case 7:
		if (jse.type == 1)
		  {
		    if (pressed1 ==0)
		      pressed1 = 1;
		    else if (pressed1 ==1)
			  pressed1 = 0;
		  }
		break;
	      default:
		break;
	      }
	    }
	  printf("Event: time %8u, value %8hd, type: %3u, axis/button: %u\n", 
		 jse.time, jse.value, jse.type, jse.number);
	  printf("leftx %i, lefty %i, rightx %i, righty %i\n",
	  titc.leftstickx, titc.leftsticky, titc.rightstickx, titc.rightsticky);
	}
      else
	{
	  /*printf ("lost connection\n");
	    titc.rightstickx = 0;
	    titc.rightsticky = 0;
	    titc.leftstickx = 0;
	    titc.leftsticky = 0;*/
	}

      usleep(10000);
    }
 
  return NULL;
}
 
int main()
{

  fd = open_joystick("/dev/input/js0");
  if (fd < 0) {
    printf("open failed.\n");
    exit(1);
  }

  titc.leftstickx=0;
  titc.leftsticky=0;
  titc.rightstickx=0;
  titc.rightsticky=0;
  pressed1 = 0;
  
  int sd = socket(AF_INET, SOCK_STREAM, 0);  /* init socket descriptor */
  struct sockaddr_in sin;
  struct hostent *host = gethostbyname(hostname);
  int len;

  /*** PLACE DATA IN sockaddr_in struct ***/
  memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(portno);

  /*** CONNECT SOCKET TO THE SERVICE DESCRIBED BY sockaddr_in struct ***/
  if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("connecting");
      exit(1);
    }

  pthread_t thread;
  
  if (pthread_create(&thread, NULL, thread_func, NULL) != 0)
    {
      return EXIT_FAILURE;
    }

  sleep(1);   /* give server time to reply */
  unsigned short head;
  int locked = 0;
  int ret = 0;
  login(&sd);
    while(1)
      {
	//	printf ("pressed1 = %i\n", pressed1);
	if (pressed1 == 1 && locked == 0)
	  {
	    ret = lockusb(&sd);
	    printf ("lockusb = %i\n", ret);
	    locked = 1;
	  }
	else if (pressed1 == 0 && locked == 1 )
	  {
	    ret = unlockusb(&sd);
	    printf ("unlockusb = %i\n", ret);
	    locked = 0;
	  }

	if (locked == 1)
	  {
	    sendpwm(&sd);
	  }
	
	usleep(50000);
      }
  close(sd);
}
