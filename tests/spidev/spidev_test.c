/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Access transiver via spidev driver at Linux/ARM
*/

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/spi/spidev.h>

/* Zigbee short register offset*/
#define PANIDL		0x01
#define PANIDH		0x02
#define SADRL		0x03
#define SADRH		0x04
#define EADR0		0x05
#define EADR1		0x06
#define EADR2		0x07
#define EADR3		0x08
#define EADR4		0x09
#define EADR5		0x0A
#define EADR6		0x0B
#define EADR7		0x0C
#define PACON1		0x08
#define FIFOEN		0x18
#define TXNTRIG		0x1B
#define GATECLK		0x20
#define WAKECTL		0x22
#define TXBCNMSK	0x25
#define SOFTRST		0x2A
#define TXPEMISP	0x2E
#define ISRSTS		0x31
#define INTMSK		0x32
#define SLPACK		0x35
#define RFCTL		0x36
#define BBREG2		0x3A
#define BBREG3		0x3B
#define BBREG5		0x3D
#define BBREG6		0x3E
#define BBREG7		0x3F


/* Zigbee long register offset*/
#define NORMAL_FIFO		0x000
#define NORMAL_FIFO1	0x001
#define NORMAL_FIFO2	0x002
#define NORMAL_FIFO3	0x003
#define RFCTRL0			0x200
#define RFCTRL1			0x201
#define RFCTRL2			0x202
#define RFCTRL3			0x203
#define RFCTRL4			0x204
#define RFCTRL6			0x206
#define	RFCTRL7			0x207
#define RFCTRL8			0x208
#define SLPCAL_0		0x209
#define SLPCAL_1		0x20A
#define SLPCAL_2		0x20B
#define	SCLKDIV			0x220
#define TESTMODE		0x22F
#define GPIODIR			0x23D
#define SECCTRL			0x24D
#define RFCTRL50		0x250
#define RFCTRL51		0x251
#define RFCTRL52		0x252
#define RFCTRL59		0x259
#define RFCTRL73		0x273
#define RFCTRL74		0x274
#define RFCTRL75		0x275
#define RFCTRL76		0x276


#define zb_uint16_t unsigned short
#define zb_uint8_t unsigned char

#define ZB_HTOBE16_VAL(ptr, val)                \
{                                               \
  zb_uint16_t _v = (val);                       \
  ZB_HTOBE16((ptr), &_v);                       \
}

#define ZB_HTOBE16(ptr, val)                            \
  (((zb_uint8_t *)(ptr))[0] = ((zb_uint8_t *)(val))[1], \
   ((zb_uint8_t *)(ptr))[1] = ((zb_uint8_t *)(val))[0]  \
  )



int fd;
int ifd;


int spi_zigbee_short_read(unsigned char short_addr, unsigned char *v);
int spi_zigbee_short_write(int short_addr, unsigned char val);
int spi_zigbee_long_write(int long_addr, unsigned char val);
int spi_zigbee_long_read(int long_addr, unsigned char *val);
int long_write_fifo(int long_addr, unsigned char *buf, int len);
int long_read_fifo(int long_addr, unsigned char *buf, int len);
void transiver_init();
void send_pkt();
void intr_wait();

int
main(
  int argc,
  char **argv)
{
#if 0
  unsigned char v;
  unsigned char buf[200];
  int i;
#endif
  (void)argv;
  (void)argc;

  errno = 0;
  fd = open("/dev/spidev2.1", O_RDWR, 0);
  if (fd == -1)
  {
    fprintf(stderr, "Error open spidev %d\n", errno);
    return 1;
  }
  errno = 0;
  ifd = open("/dev/zbintr", O_RDWR, 0);
  if (ifd == -1)
  {
    fprintf(stderr, "Error open zbintr %d\n", errno);
    return 1;
  }


#if 0
  short_read_reg(EADR0, &v);
  printf("read1 %d\n", v);
  short_write_reg(EADR0, 0x20);
  short_read_reg(EADR0, &v);
  printf("read2 %d\n", v);

  long_read_reg(RFCTRL0, &v);
  printf("read3 %d\n", v);
  long_write_reg(RFCTRL0, 0x03);
  long_read_reg(RFCTRL0, &v);
  printf("read4 %d\n", v);

  for (i = 0 ; i < 20 ; ++i)
  {
    buf[i] = 50-i;
  }
  long_write_fifo(NORMAL_FIFO, buf, 20);

  printf("fifo2\n");
  long_read_fifo(NORMAL_FIFO, buf, 20);
  for (i = 0 ; i < 20 ; ++i)
  {
    printf("%02x ", buf[i]);
  }
  printf("\n");
#endif

  transiver_init();
  send_pkt();

  return 0;
}


int spi_zigbee_short_read(unsigned char short_addr, unsigned char *v)
{
  struct spi_ioc_transfer ioc;
  unsigned char txb[2];
  unsigned char rxb[2];
  int ret = 0;

  memset(&ioc, 0, sizeof(ioc));
  ioc.tx_buf = (unsigned long)txb;
  ioc.rx_buf = (unsigned long)rxb;
  /* read/write (write address), then read/write (read value) */
  ioc.len = 2;
  ioc.bits_per_word = 8;
  txb[0] = (short_addr << 1);
  txb[1] = 0;

  errno = 0;
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &ioc);
  fprintf(stderr, "rx1 ioctl ret %d errno %d\n", ret, errno);

  *v = rxb[1];

  return ret;
}


int spi_zigbee_short_write(int short_addr, unsigned char val)
{
  struct spi_ioc_transfer ioc;
  unsigned char txb[2];
  int ret = 0;

  memset(&ioc, 0, sizeof(ioc));
  ioc.tx_buf = (unsigned long)txb;
  ioc.len = 2;
  ioc.bits_per_word = 8;

  txb[0] = (short_addr << 1) | 1;
  txb[1] = val;

  errno = 0;
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &ioc);
  fprintf(stderr, "tx ioctl ret %d errno %d\n", ret, errno);

  return ret;
}


int spi_zigbee_long_write(int long_addr, unsigned char val)
{
  struct spi_ioc_transfer ioc;
  unsigned char txb[3];
  int ret = 0;

  memset(&ioc, 0, sizeof(ioc));
  ioc.tx_buf = (unsigned long)txb;
  ioc.len = 3;
  ioc.bits_per_word = 8;

  ZB_HTOBE16_VAL(txb, (long_addr << 5 ) | 0x8010);
  txb[2] = val;

  errno = 0;
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &ioc);
  fprintf(stderr, "l tx ioctl ret %d errno %d\n", ret, errno);

  return ret;
}


int spi_zigbee_long_read(int long_addr, unsigned char *val)
{
  struct spi_ioc_transfer ioc;
  unsigned char txb[3];
  unsigned char rxb[3];
  int ret = 0;

  memset(&ioc, 0, sizeof(ioc));
  ioc.tx_buf = (unsigned long)txb;
  ioc.rx_buf = (unsigned long)rxb;
  ioc.len = 3;
  ioc.bits_per_word = 8;

  ZB_HTOBE16_VAL(txb, (long_addr << 5 ) | 0x8000);
  txb[2] = 0;

  errno = 0;
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &ioc);
  fprintf(stderr, "l rx ioctl ret %d errno %d\n", ret, errno);

  *val = rxb[2];

  return ret;
}


int long_write_fifo(int long_addr, unsigned char *buf, int len)
{
  struct spi_ioc_transfer ioc[24];
  unsigned char txb[24][8];
  int ret = 0;
  int i = 0;
  int n = len / 6;

  memset(&ioc, 0, sizeof(ioc));
  for (i = 0 ; i < n ; ++i)
  {
    ioc[i].tx_buf = (unsigned long)txb[i];
    ZB_HTOBE16_VAL(txb[i], ((long_addr + i * 6) << 5 ) | 0x8010);
    ioc[i].len = 8;
    memcpy(txb[i] + 2, buf + i * 6, 6);
    ioc[i].bits_per_word = 8;
  }
  if (len % 6)
  {
    ioc[i].tx_buf = (unsigned long)txb[i];
    ZB_HTOBE16_VAL(txb[i], ((long_addr + i * 6) << 5 ) | 0x8010);
    ioc[i].len = len % 6 + 2;
    memcpy(txb[i] + 2, buf + i * 6, len % 6);
    ioc[i].bits_per_word = 8;
    n++;
  }

  errno = 0;
  ret = ioctl(fd, SPI_IOC_MESSAGE(n), ioc);
  fprintf(stderr, "l tx ioctl ret %d errno %d\n", ret, errno);

  return ret;
}


int long_read_fifo(int long_addr, unsigned char *buf, int len)
{
  struct spi_ioc_transfer ioc[24];
  unsigned char txb[24][8];
  unsigned char rxb[24][8];
  int ret = 0;
  int i = 0;
  int n = len / 6;

  memset(&ioc, 0, sizeof(ioc));
  memset(&txb, 0, sizeof(txb));
  for (i = 0 ; i < n ; ++i)
  {
    ioc[i].tx_buf = (unsigned long)txb[i];
    ioc[i].rx_buf = (unsigned long)rxb[i];
    ZB_HTOBE16_VAL(txb[i], ((long_addr + i * 6) << 5 ) | 0x8000);
    ioc[i].len = 8;
    ioc[i].bits_per_word = 8;
  }
  if (len % 6)
  {
    ioc[i].tx_buf = (unsigned long)txb[i];
    ioc[i].rx_buf = (unsigned long)rxb[i];
    ZB_HTOBE16_VAL(txb[i], ((long_addr + i * 6) << 5 ) | 0x8000);
    ioc[i].len = len % 6 + 2;
    ioc[i].bits_per_word = 8;
    n++;
  }

  errno = 0;
  ret = ioctl(fd, SPI_IOC_MESSAGE(n), ioc);
  fprintf(stderr, "l tx ioctl ret %d errno %d\n", ret, errno);

  n = len / 6;
  for (i = 0 ; i < n ; ++i)
  {
    memcpy(buf + i * 6, rxb[i] + 2, 6);
  }
  memcpy(buf + i * 6, rxb[i] + 2, len % 6);

  return ret;
}


void transiver_init()
{
	//Set up the MAC address
	spi_zigbee_short_write(EADR0, 0x20);
	spi_zigbee_short_write(EADR1, 0x21);
	spi_zigbee_short_write(EADR2, 0x22);
	spi_zigbee_short_write(EADR3, 0x23);
	spi_zigbee_short_write(EADR4, 0x24);
	spi_zigbee_short_write(EADR5, 0x25);
	spi_zigbee_short_write(EADR6, 0x26);
	spi_zigbee_short_write(EADR7, 0x27);

	//SPI Sync
	spi_zigbee_short_write(GATECLK, 0x20);

	//Power Amplifying time
	spi_zigbee_short_write(PACON1, 0x08);

	//TXON time
	spi_zigbee_short_write(FIFOEN, 0x94);

	//VCO calibration period

	spi_zigbee_short_write(TXPEMISP, 0x95);

	//Search Energy Detection
	spi_zigbee_short_write(BBREG3, 0x50);

	//Searching time
	spi_zigbee_short_write(BBREG5, 0x07);

	//Append RSSI value in RX packets
	spi_zigbee_short_write(BBREG6, 0x40);

	//RF optimized control
	spi_zigbee_long_write( RFCTRL0, 0x03);
	spi_zigbee_long_write( RFCTRL1, 0x02);
	spi_zigbee_long_write( RFCTRL2, 0x66);
	spi_zigbee_long_write( RFCTRL4, 0x06);
	spi_zigbee_long_write( RFCTRL6, 0x30);
	spi_zigbee_long_write( RFCTRL7, 0xEC);
	spi_zigbee_long_write( RFCTRL8, 0x8C);

	//Setting GPIO to output
	spi_zigbee_long_write( GPIODIR, 0x00);

	//Enable 802.15.4-2006 security
	spi_zigbee_long_write( SECCTRL, 0x20);

	//RFoptimized control
	spi_zigbee_long_write( RFCTRL50, 0x05);
	spi_zigbee_long_write( RFCTRL51, 0xC0);
	spi_zigbee_long_write( RFCTRL52, 0x01);
	spi_zigbee_long_write( RFCTRL59, 0x00);
	spi_zigbee_long_write( RFCTRL73, 0x40);
	spi_zigbee_long_write( RFCTRL74, 0xC5);
	spi_zigbee_long_write( RFCTRL75, 0x13);
	spi_zigbee_long_write( RFCTRL76, 0x07);

	//Enable all interrupt
	spi_zigbee_short_write(INTMSK, 0x00);

	//Baseband Reset
	spi_zigbee_short_write(SOFTRST, 0x02);

	//0x36 sequential setup
	spi_zigbee_short_write(RFCTL, 0x04);
	spi_zigbee_short_write(RFCTL, 0x00);
	spi_zigbee_short_write(RFCTL, 0x02);
	usleep(200);
	spi_zigbee_short_write(RFCTL, 0x01);
	usleep(200);
	spi_zigbee_short_write(RFCTL, 0x00);

}


void send_pkt()
{
	zb_uint8_t val;

  //Consist a fake packet : header - 0xCC; Data - 0xDD
  spi_zigbee_long_write(NORMAL_FIFO, 0x01);
  spi_zigbee_long_write(NORMAL_FIFO1, 0x02);
  spi_zigbee_long_write(NORMAL_FIFO2, 0xCC);
  spi_zigbee_long_write(NORMAL_FIFO3, 0xDD);

  spi_zigbee_long_read(NORMAL_FIFO, &val);
  spi_zigbee_long_read(NORMAL_FIFO1, &val);
  spi_zigbee_long_read(NORMAL_FIFO2, &val);
  spi_zigbee_long_read(NORMAL_FIFO3, &val);

  //set the transmission trigger bit
  spi_zigbee_short_read(TXNTRIG, &val);
  val = val | 0x1;
  spi_zigbee_short_write(TXNTRIG, val);

  intr_wait();

  spi_zigbee_short_read(ISRSTS, &val);
  printf("isrsts 0x%x\n", val);
}


void intr_wait()
{
  int ret;
  fd_set set;

  FD_ZERO(&set);
  FD_SET(ifd, &set);

  printf("entering select()...\n");
  errno = 0;
  ret = select(ifd+1, &set, NULL, NULL, NULL);
  printf("select() ret %d errno %d\n", ret, errno);
}
