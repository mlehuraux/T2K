/*******************************************************************************

 File:        remotecopy.h

 Description: Definitions for functions to make remote copy of files on 
 SD Card of controlled node


 Author:      D. Calvet,        denis.calvetATcea.fr
              

 History:
   December 2017 : created

*******************************************************************************/
#include "remotecopy.h"
#include "femarray.h"

#include <stdio.h>
#include <time.h>

extern int verbose;

#define RCP_DATA_CHUNK_SIZE  512
#define DISPLAY_CHUNK_MODULO 128

/*******************************************************************************
CmdFetcher_SendCommand
*******************************************************************************/
int CmdFetcher_SendCommand(CmdFetcher *cf, unsigned char *cmd)
{
	int err;
	unsigned int fem_beg, fem_end, fem_pat;
	FemArray *fa;

	int fem_err_cnt;

	fa = (FemArray *)cf->fa;

	fem_beg = cf->cur_single_fem;
	fem_end = cf->cur_single_fem;
	fem_pat = 1 << cf->cur_single_fem;

	// Get the initial number of command errors for that fem
	fem_err_cnt = fa->fp[fem_beg].cmd_failed;

	// Pass command to FemArray that will send it
	if ((err = FemArray_SendCommand(fa, fem_beg, fem_end, fem_pat, cmd)) < 0)
	{
		return(err);
	}

	// Wait completion of the command to be signalled by network receive thread
	if ((err = Semaphore_Wait_Timeout(cf->sem_cur_cmd_done, 4000000)) < 0)
	{
		if (err == -2)
		{
			printf("CmdFetcher_SendCommand: Semaphore_Wait_Timeout: timeout detected.\n");
		}
		else
		{
			printf("CmdFetcher_SendCommand: Semaphore_Wait_Timeout failed %d\n", err);
		}
		return(err);
	}

	// Check that command execution was successfull
	if (fa->fp[fem_beg].cmd_failed != fem_err_cnt)
	{
		err = -1;
	}

	cf->cmd_index++;

	return(err);
}

/*******************************************************************************
CmdFetcher_RemoteCopy
*******************************************************************************/
int CmdFetcher_RemoteCopy(CmdFetcher *cf, char *src, char *dst)
{
	int err;
	int err_fin;
	char command[1400];
	unsigned char data_chunk[520];
	int done;
	int byte_read;
	int byte_total;
	char *ptr;
	int i;
	int eof_reached;
	int chunk_count;
	FILE *src_fp = (FILE *) 0;
	int verbose_saved;
	struct tm *now;
	time_t the_time;

	if (strcmp(dst, ".") == 0)
	{
		strcpy(dst, src);
	}

	// Open local source file
	if (!(src_fp = fopen(src, "rb")))
	{
		err = -1;
		printf("Could not open file %s.\n", src);
		return(err);
	}

	// save verbose
	verbose_saved = verbose;
	// be silent
	verbose = 0;
	err_fin = 0;

	// Mount remote disk
	sprintf(command, "be sd mount\n");
	if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
	{
		printf("Command failed: %s", command);
		err_fin = err;
		goto close_local_file;
	}

	// Enable write to media
	/*

	OK, I decided not to do it here. The user has to do it
	himself with the appropriate command. This can avoid
	some manipulation errors

	sprintf(command, "be sd wena 1\n");
	if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
	{
		printf("Command failed: %s", command);
		err_fin = err;
		goto umount_remote_card;
	}
	*/

	// Open remote file
	sprintf(command, "be sd fopen wf %s\n", dst);
	if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
	{
		printf("Command failed: %s", command);
		err_fin = err;
		goto disable_write_remote_card;
	}

	// Read source file in chunks and write them to remote card until all is done
	done = 0;
	byte_total = 0;
	eof_reached = 0;
	chunk_count = 0;
	while (!done)
	{
		byte_read = fread(&data_chunk[0], 1, RCP_DATA_CHUNK_SIZE, src_fp);
		byte_total += byte_read;
		chunk_count++;

		if (byte_read != 0)
		{
			// Prepare Command
			sprintf(command, "be sd fwrite ");
			ptr = &command[0] + strlen(command);

			// Convert every data byte to ASCII
			for (i = 0; i < byte_read; i++)
			{
				sprintf(ptr, "%02x", data_chunk[i]);
				ptr += 2;
			}
			sprintf(ptr, "\n");
			//printf("Command:%s", command);

			// Write to remote file
			if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
			{
				printf("Command failed: %s", command);
				err_fin = err;
				done = 1;
			}

			if ((chunk_count%DISPLAY_CHUNK_MODULO) == 0)
			{
				printf(".");
				fflush(stdout);
			}
		}

		if (byte_read != RCP_DATA_CHUNK_SIZE)
		{
			if (feof(src_fp))
			{
				eof_reached = 1;
				done = 1;
			}
			else if (ferror(src_fp))
			{
				done = 1;
			}
		}
	}

	// Epilogue
	printf("\n");
	printf("Wrote %d bytes (sent %d data chunks)\n", byte_total, chunk_count);
	if (eof_reached)
	{
		printf("Complete file transfer.\n");
	}
	else
	{
		printf("Warning! Incomplete file transfer.\n");
	}

	// Get the current local time
	time(&the_time);
	now = localtime(&the_time);

	// Close remote file
	sprintf(command, "be sd fclose\n");
	if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
	{
		printf("Command failed: %s", command);
		err_fin = err;
	}

	// Build command to set the current date and time for the remote file
	sprintf(command, "be sd fdate %s %d/%d/%d %d:%d:%d\n",
		dst,
		((now->tm_year) + 1900),
		((now->tm_mon) + 1),
		now->tm_mday,
		now->tm_hour,
		now->tm_min,
		now->tm_sec);
	// Set remote file date and time
	if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
	{
		printf("Command failed: %s", command);
		err_fin = err;
	}

disable_write_remote_card:
	// Disable write to media
	sprintf(command, "be sd wena 0\n");
	if ((err = CmdFetcher_SendCommand(cf, command)) < 0)
	{
		printf("Command failed: %s", command);
		err_fin = err;
		goto umount_remote_card;
	}

umount_remote_card:
	// Unmount remote disk
	sprintf(command, "be sd umount\n");
	if (CmdFetcher_SendCommand(cf, command) < 0)
	{
		printf("Command failed: %s", command);
	}

close_local_file:

	// Close source file
	if (src_fp) fclose(src_fp);

	// restore verbose
	verbose = verbose_saved;

	return(err_fin);
}
