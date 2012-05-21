#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* options
 * r	remove 0D 0A occurrence (\r\n)
 * u	replace 0D 0A occurrence with 0A (DOS -> UNIX)
 * n	replace 0D occurrence with 0A    (MAC -> UNIX)
 * d	replace 0A occurrence with 0D 0A (UNIX -> DOS)
 * m	replace 0A occurrence with 0D    (UNIX -> MAC)
 */

#define FNAME_LEN   256
#define BUFFER_SIZE 2048

static char *options = "rudmn";
static int remove_crnl( FILE *src, FILE *dst );
static int mac2unix( FILE *src, FILE *dst );
static int unix2mac( FILE *src, FILE *dst );
static int dos2unix( FILE *src, FILE *dst );
static int unix2dos( FILE *src, FILE *dst );

static void usage( void ){

	printf( 
		"\n  hexelaborate 1.0 - 2012 Simone Irti\n\n  "
		"usage: hexelaborate <options> <filename>[,<filename>,...]\n\n  "
		"options:\n  "
		"  -r\tremove 0D 0A occurrence (\\r\\n)\n  "
		"  -u\treplace 0D 0A occurrence with 0A (DOS -> UNIX)\n  "
		"  -n\treplace 0D occurrence with 0A    (MAC -> UNIX)\n  "
		"  -d\treplace 0A occurrence with 0A 0D (UNIX -> DOS)\n  "
		"  -m\treplace 0A occurrence with 0D    (UNIX -> MAC)\n\n"
	      );

}

int main( int argc, char *argv[] ){

	int loop, opt, occurrence;
	char fname[FNAME_LEN];
	FILE *src, *dst;
	
	/* check the input parameters */
	if( argc < 2 ){

		usage();
		exit( 0 );
	}

	/* get only the first option required */
	opt = getopt( argc, argv, options );

	/* for each file in input */
	for( loop = 2; loop < argc; loop++ ){

		/* create backup file */
		strncpy( fname, argv[loop], FNAME_LEN - 4 );
		strcat( fname, ".bak" );
		printf( "creating backup file (%s)\n", fname );
		rename( argv[loop], fname );

		/* open the src and dst files */
		printf( "opening source and destination files\n" );
		src = fopen( fname, "r" );
		dst = fopen( argv[loop], "w" );

		/* check the fopen results */
		if( src != NULL && dst != NULL ){

			/* execute the requested operation */
			switch( opt ){

			/* remove \r\n with \n */
			case 'r':
				occurrence = remove_crnl( src, dst );
				break;

			/* replace \r\n with \n */
			case 'u':
				occurrence = dos2unix( src, dst );
				break;

			/* replace \n with \r\n */
			case 'd':
				occurrence = unix2dos( src, dst );
				break;

			/* replace \n with \r */
			case 'm':
				occurrence = unix2mac( src, dst );
				break;

			/* replace \r with \n */
			case 'n':
				occurrence = mac2unix( src, dst );
				break;

			default:
				printf( "ERROR in options\n" );
				occurrence = 0;
				break;
			}

			printf( "found %d occurrence\n", occurrence );

			/* close src and dst files */
			printf( "closing source and destination files\n" );
			fclose( src );
			fclose( dst );
		}
	}

	printf( "end of all operations\n" );

	return 0;
}


static int remove_crnl( FILE *src, FILE *dst ){

	int  i, j, occurrence = 0;
	char buffer[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];

	/* read the src file line by line */
	while( fgets( buffer, BUFFER_SIZE, src ) != NULL ){

		i = 0;
		j = 0;

		/* check every input character */
		while( buffer[i] != '\0' && i < BUFFER_SIZE ){

			if( buffer[i] == '\r' && buffer[i+1] == '\n' ){

				/* jump the \r\n couple in input buffer */
				i++;
				j--;
				occurrence++;

			} else {

				outbuf[j] = buffer[i];

			}

			i++;
			j++;
		}

		/* terminate the output buffer */
		outbuf[j] = '\0';

		/* write the elaborated buffer in destination file and check the result */
		if( fwrite( (void *)outbuf, sizeof( char ), j, dst ) != j ){

			printf( "ERROR writing the file\n" );
			break;
		}
	}

	return occurrence;

}


static int mac2unix( FILE *src, FILE *dst ){

	int  i, occurrence = 0;
	char buffer[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];

	/* read the src file line by line */
	while( fgets( buffer, BUFFER_SIZE, src ) != NULL ){

		i = 0;

		/* check every input character */
		while( buffer[i] != '\0' && i < BUFFER_SIZE ){

			if( buffer[i] == '\r' ){

				/* replace the \r with \n */
				outbuf[i] = '\n';
				occurrence++;

			} else {

				outbuf[i] = buffer[i];

			}

			i++;
		}

		/* terminate the output buffer */
		outbuf[i] = '\0';

		/* write the elaborated buffer in destination file and check the result */
		if( fwrite( (void *)outbuf, sizeof( char ), i, dst ) != i ){

			printf( "ERROR writing the file\n" );
			break;
		}
	}

	return occurrence;

}


static int unix2mac( FILE *src, FILE *dst ){

	int  i, occurrence = 0;
	char buffer[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];

	/* read the src file line by line */
	while( fgets( buffer, BUFFER_SIZE, src ) != NULL ){

		i = 0;

		/* check every input character */
		while( buffer[i] != '\0' && i < BUFFER_SIZE ){

			if( buffer[i] == '\n' ){

				/* replace the \n with \r */
				outbuf[i] = '\r';
				occurrence++;

			} else {

				outbuf[i] = buffer[i];

			}

			i++;
		}

		/* terminate the output buffer */
		outbuf[i] = '\0';

		/* write the elaborated buffer in destination file and check the result */
		if( fwrite( (void *)outbuf, sizeof( char ), i, dst ) != i ){

			printf( "ERROR writing the file\n" );
			break;
		}
	}

	return occurrence;

}


static int dos2unix( FILE *src, FILE *dst ){

	int  i, j, occurrence = 0;
	char buffer[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];

	/* read the src file line by line */
	while( fgets( buffer, BUFFER_SIZE, src ) != NULL ){

		i = 0;
		j = 0;

		/* check every input character */
		while( buffer[i] != '\0' && i < BUFFER_SIZE ){

			if( buffer[i] == '\r' &&  buffer[i+1] == '\n' ){

				/* replace the \r\n with \n */
				outbuf[j] = '\n';
				i++;
				occurrence++;

			} else {

				outbuf[j] = buffer[i];

			}

			i++;
			j++;
		}

		/* terminate the output buffer */
		outbuf[j] = '\0';

		/* write the elaborated buffer in destination file and check the result */
		if( fwrite( (void *)outbuf, sizeof( char ), j, dst ) != j ){

			printf( "ERROR writing the file\n" );
			break;
		}
	}

	return occurrence;
}


static int unix2dos( FILE *src, FILE *dst ){

	int  i, j, occurrence = 0;
	char buffer[BUFFER_SIZE];
	char outbuf[BUFFER_SIZE];

	/* read the src file line by line */
	while( fgets( buffer, BUFFER_SIZE - 2, src ) != NULL ){

		i = 0;
		j = 0;

		/* check every input character */
		while( buffer[i] != '\0' && j < BUFFER_SIZE - 1 ){

			if( buffer[i] == '\n' ){

				/* replace the \n with \r\n */
				outbuf[j] = '\r';
				j++;
				outbuf[j] = '\n';
				occurrence++;

			} else {

				outbuf[j] = buffer[i];

			}

			i++;
			j++;
		}

		/* terminate the output buffer */
		outbuf[j] = '\0';

		/* write the elaborated buffer in destination file and check the result */
		if( fwrite( (void *)outbuf, sizeof( char ), j, dst ) != j ){

			printf( "ERROR writing the file\n" );
			break;
		}

		if( j == BUFFER_SIZE ){

			printf( "WARNING some data maybe lost\n" );
		}
	}

	return occurrence;
}

