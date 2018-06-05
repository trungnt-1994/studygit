#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tzcslib/TzcsUtil.h>

/* ICEWM's startup file */
#define XINITRCFILE   "/root/.xinitrc"
/* NTP configuration file */
#define NTPCONFFILE   "/etc/ntp.conf"

/* TMZC NTP server */
#define NTPSERVERADDR_CONF       "server %s iburst maxpoll 16\n"
/* pattern to find where to put server list */
#define NTPSERVERLIST_PATTERN    "# pool: <http://www.pool.ntp.org/join.html>"
/* pattern to find where to put ntp service */
#define NTPSERVICECMD_PATTERN    "#usr/bin/startup-ntp.sh &"

#define NTPADDRSIZE (65 * 2)   /* NTP server string length */
#define BUFFERSIZE  100        /* buffer size */

int main(void)
{
	int nr;
	ULONG ret;
	char ntp_addr[NTPADDRSIZE], buf[BUFFERSIZE];
	FILE *fp;
	int ccgetline(char *, int, FILE *);

	memset(ntp_addr, 0, NTPADDRSIZE);
	if ((ret = get_hci_string(VAL_NTP_SERVER_ADDRESS, ntp_addr)) == HCI_FAIL || strlen(ntp_addr) == 0)
		return EXIT_FAILURE;

	{ /* add TMZC NTP server to ntp conf file */
		if ((fp = fopen(NTPCONFFILE, "r+")) == NULL) {
			perror("fopen "NTPCONFFILE);
			exit(EXIT_FAILURE);
		}
		while (ccgetline(buf, BUFFERSIZE, fp) >= 0) {
			if (strcmp(NTPSERVERLIST_PATTERN, buf) == 0) {
				fprintf(fp, NTPSERVERADDR_CONF, ntp_addr);
				break;
			}
		}
		fclose(fp);
	}

	{ /* add ntp service to startup */
		if ((fp = fopen(XINITRCFILE, "r+")) == NULL) {
			perror("fopen "XINITRCFILE);
			exit(EXIT_FAILURE);
		}
		while ((nr = ccgetline(buf, BUFFERSIZE, fp)) >= 0) {
			if (strcmp(NTPSERVICECMD_PATTERN, buf) == 0) {
				fseek(fp, -(nr + 1), SEEK_CUR);
				fprintf(fp, "%c", '/');
				break;
			}
		}
		fclose(fp);
	}

	return EXIT_SUCCESS;
}

/*
 * @brief Get at most one less than lim characters from fp stream

 * @param[out] s Buffer string
 * @param[in] lim Maximum available of buffer to fill
 * @param[in] fp File stream
 *
 * @return Return number of bytes that actually read
 */
int ccgetline(char *s, int lim, FILE *fp)
{
	int c, i;

	if (feof(fp))
		return -1;

	for (i = 0; i < lim - 1 && (c = fgetc(fp)) != '\n'; i++)
		*(s + i) = c;
	*(s + i) = '\0';

	return i;
}
