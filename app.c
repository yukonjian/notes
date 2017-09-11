#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	char buf[10]={0};
	int fd, count;
	fd = open("/dev/test_led", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	count = write(fd, "123456789", sizeof("12345678912"));
	printf("the write function count is %d buf is %s \n",count,buf);	

	count = read(fd, buf, 5);
	printf("the read function count is %dbuf is %s\n",count, buf);



	close(fd);
	return 0;
}
