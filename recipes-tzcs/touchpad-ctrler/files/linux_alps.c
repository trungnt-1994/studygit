#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
 

#define GET_SELECTDEV _IOR('q', 1, int)
#define SET_SELECTDEV _IOW('q', 2, int)

#define GET_PALMREJECTION _IOR('q', 3, int)
#define SET_PALMREJECTION _IOW('q', 4, int)

#define TP_ON 0x1
#define SP_ON 0x2

int main(int argc, char *argv[])
{
    char *file_name = "/dev/alps";
    int fd;
	int selectDev = 0x3;
	int getSelectDev = 0;
	int getPalmRejection = 0;
	int setPalmRejection = 0;
			
			
	fd = open(file_name, O_RDWR);
    
    if (fd == -1)
    {
        perror("Cound not find file name");
        return 2;
    }
    
    if (argc == 2)
    {
		if (strcmp(argv[1], "getdev") == 0)
        {			    
		    if (ioctl(fd, GET_SELECTDEV, &getSelectDev) == -1)
			{
 		       perror("ioctl GET_SELECTDEV error");
			}
			printf("getSelectDev : %d\n", getSelectDev);
        }
        else if (strcmp(argv[1], "getpalm") == 0)
        {
			if (ioctl(fd, GET_PALMREJECTION, &getPalmRejection) == -1)
			{
				perror("ioctl GET_PALMREJECTION error");
			}
			printf("getPalmRejection : %d\n", getPalmRejection);
		}
		else if(strcmp(argv[1], "disdev") == 0)
		{
			selectDev = (TP_ON|SP_ON);
			printf("selectDev : %d\n", selectDev);
		    if (ioctl(fd, SET_SELECTDEV, &selectDev) == -1)
		    {
 		       perror("ioctl SET_SELECTDEV error");
		    }
		}
		else
        {
            fprintf(stderr, "Usage: %s [getdev | getpalm | setdev]\n", argv[0]);
            return 1;
        }
    }
    else if(argc == 3)
    {
        if (strcmp(argv[1], "disdev") == 0)
        {
			if (strcmp(argv[2], "t") == 0)
        	{
				selectDev &=~TP_ON;
			}
			else if(strcmp(argv[2], "s") == 0)
			{
				selectDev &=~SP_ON;
			}
			else if(strcmp(argv[2], "ts") == 0)
			{
				selectDev &=~(TP_ON|SP_ON);
			}
			else
			{
				selectDev = (TP_ON|SP_ON);
			}
			
			printf("selectDev : %d\n", selectDev);
		    if (ioctl(fd, SET_SELECTDEV, &selectDev) == -1)
		    {
 		       perror("ioctl SET_SELECTDEV error");
		    }
		}
        else if (strcmp(argv[1], "setpalm") == 0)
        {
			if (strcmp(argv[2], "on") == 0)
        	{
				setPalmRejection = 1;
			}
			else if(strcmp(argv[2], "off") == 0)
			{
				setPalmRejection = 0;
			}
			
			printf("setPalmRejection : %d\n", setPalmRejection);
		    if (ioctl(fd, SET_PALMREJECTION, &setPalmRejection) == -1)
		    {
    		    perror("ioctl SET_PALMREJECTION error");
		    }
        }
        else
        {
            fprintf(stderr, "Usage: %s [getdev | getpalm | setdev]\n", argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [getdev | getpalm | setdev]\n", argv[0]);
        return 1;
    }
    


    close (fd);
 
    return 0;
}