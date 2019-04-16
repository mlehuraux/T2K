/*******************************************************************************

 File:        cmdfetcher.c

 Description: an object to get the commands to be posted to a set of Feminos
 cards. Commands can be read from a script and/or can originate from keyboard
 input. Because it is easier to handle stdin with blocking functions, this code
 must be run in its own thread


 Author:      D. Calvet,        denis.calvetATcea.fr
              

 History:
   December 2011 : created
   May 2012      : added on-the-fly command modification to use loop index as
    a variable within a command

   January 2014: added event builder modes for checking event numbers and
   timestamps.

   March 2014: (version 1.3) modified code so that $loop variable can be used
   as 4th or 5th argument in any command

   October 2014: (version 1.4) typing "mars set clock" will fetch the current
   local time and fill that command with this information before sending it.

   November 2014: (version 1.5) changed event builder so that an event cannot
   span across two files when eof_on_eoe is active (with or without event
   builder).

   February 2015: (version 1.6) increased size of command line to 768 characters
   instead of 80.

   March 2015: (version 1.7) corrected bug on HEX file reader to avoid failure
   when reading files that contain non contiguous data segments within a page
   or data segments that do not start on a page boundary.

   April 2015: (version 1.8) added the command "file_suffix" to add a variable
   string to the name of the data files that are created.

   October 2017: (version 1.9) changed the name of the objects being controlled
   from Feminos to Servers. In PandaX-III this program controls the TDCM and
   not the front-end directly, but it can also control the Harpo ROC. So we
   refer all these by being a "Server", abbreviated "srv"

   December 2017: (version 2.0) added command rcp to copy a local file to the
   remote SD Card of a server
   Changed syntax of "program flash" command to add the type of flash
   (Winbond or Spansion). Updated code to support two types of flash
   - Winbond W25Q128BV and Spansion S25FL512S

   October 2018: (version 2.1) changed command "mars" in "iic mod"
   Added to banner type of server, ARC or TDCM.
   Corrected bug in the LIST command so that file names are "ped_..." and
   "thr_..." respectively for pedestals and thresholds.

   December 2018: (version 2.2) changed syntax of "program flash" command
   into "fe program flash" and added argument to distinguish if the ARC or
   some other front-end is concerned by the operation

*******************************************************************************/
#include "cmdfetcher.h"
#include "femarray.h"
#include "evbuilder.h"
#include "programflash.h"
#include "remotecopy.h"

extern int verbose;

#include <stdio.h>
#include <time.h>

////////////////////////////////////////////////
// Manage Major/Minor version numbering manually
#define CLIENT_VERSION_MAJOR 2
#define CLIENT_VERSION_MINOR 2
char client_date[] = __DATE__;
char client_time[] = __TIME__;
////////////////////////////////////////////////

/* Event Builder mode interpretation  */
static char EventBuilder_Mode2str[16][40] = {
		"transparent",                            // 0
		"active",                                 // 1
		"transparent",                            // 2
		"active with event nb verify",            // 3
		"transparent",                            // 4
		"active with event ts verify",            // 5
		"transparent",                            // 6
		"active with event nb and ts verify",     // 7
		"transparent",                            // 8
		"active with event ts +-1 verify",        // 9
		"transparent",                            // 10
		"active with event nb and ts +-1 verify", // 11
		"transparent",                            // 12
		"active with ts +-1 verify",              // 13
		"transparent",                            // 14
		"active with event nb and ts +-1 verify"  // 15
};

/*******************************************************************************
 CmdFetcher_Init
*******************************************************************************/
void CmdFetcher_Init(CmdFetcher *cf)
{
	sprintf(&(cf->cmd_file[0]), "");
	cf->no_echo_this     = 0;
	cf->cmd_cnt          = 0;
	cf->use_stdin        = 1;
	cf->cmd_index        = 0;
	cf->cur_single_fem   = 0;
	cf->cur_multi_fem    = 0;
	cf->cur_fem_cnt      = 0;
	cf->fem_ix_min       = 0;
	cf->fem_ix_max       = 0;
	cf->fa               = (void *) 0;
	cf->eb               = (void *) 0;
	cf->sem_cur_cmd_done = (void *) 0;
}

/*******************************************************************************
 CmdFetcher_UpdateFemBoundaries
*******************************************************************************/
void CmdFetcher_UpdateFemBoundaries(CmdFetcher *cf)
{
	int i;
	unsigned int mask;
	int min_fnd;

	cf->cur_fem_cnt = 0;
	mask    = 0x00000001;
	min_fnd = 0;
	if (cf->cur_multi_fem == 0)
	{
		cf->cur_fem_cnt = 1;
		cf->fem_ix_min  = cf->cur_single_fem;
		cf->fem_ix_max  = cf->cur_single_fem;
	}
	else
	{
		for(i=0; i<MAX_NUMBER_OF_FEMINOS; i++)
		{
			if (cf->cur_multi_fem & mask)
			{
				if (min_fnd == 0)
				{
					cf->fem_ix_min = i;
					min_fnd        = 1;
				}
				cf->fem_ix_max = i;
				cf->cur_fem_cnt++;
			}
			mask <<=1;
		}
	}
}

/*******************************************************************************
 CmdFetcher_Main
*******************************************************************************/
int CmdFetcher_Main(CmdFetcher *cf)
{
	int i, j;
	int err;
	int alldone;
	int cmd_ix;
	int loop_max;
	int loop_ix;
	int cmd_loop_ix;
	char cmd_stdin[768];
	char tmp_str[80];
	char *cmd;
	int cmd_stdin_done;
	int mask;
	int post_cmd, post_daq;
	char srv_tar_str[80];
	unsigned int fem_beg, fem_end, fem_pat;
	FemArray *fa;
	EventBuilder *eb;
	int param[8];
	char str[8][768];
	struct tm *now;
	time_t the_time;
	char src_file[80];
	char dst_file[80];

	printf("CmdFetcher_Main: started\n");

	fa = (FemArray *) cf->fa;
	eb = (EventBuilder *) fa->eb;

	// Parse command file if it was passed in command argument
	if (cf->use_stdin == 0)
	{
		if ((err=CmdFetcher_ParseCmdFile(cf)) < 0)
		{
			return(err);
		}
	}

	CmdFetcher_UpdateFemBoundaries(cf);

	// Print setup
	if (cf->verbose)
	{
		printf("---------------------------------\n");
		printf("Client version    : %d.%d\n", CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR);
		printf("Compiled          : %s at %s\n", client_date, client_time);
		if (fa->is_arc)
		{
			printf("Target Server     : ARC\n");
		}
		else
		{
			printf("Target Server     : TDCM\n");
		}
		if (cf->use_stdin)
		{
			printf("Commands          : from stdin\n");
		}
		else
		{

			if (cf->cmd_file[0] != '\0')
			{
				printf("Command file name : %s\n", cf->cmd_file);
			}
			printf("Command count     : %d\n", cf->cmd_cnt);
			printf("Command list      :\n");
			for (i=0;i<cf->cmd_cnt; i++)
			{
				printf("   %s", cf->snd[i]);
			}
		}
		
		printf("---------------------------------\n");
	}

	// Various init
	alldone        = 0;
	loop_max       = 0;
	loop_ix        = -1;
	cmd_ix         = 0;
	post_cmd       = 0;
	cmd_loop_ix    = 0;
	sprintf(srv_tar_str, "srv(%02d)", cf->cur_single_fem);

	// Create semaphore to signal completion of execution of the current command
	if ((err= Semaphore_Create(&(cf->sem_cur_cmd_done))) < 0)
	{
		printf("CmdFetcher_Main: Semaphore_Create failed %d\n", err);
		return (err);
	}

	// Pass this semaphore to the FemArray
	fa->sem_cur_cmd_done = cf->sem_cur_cmd_done;

	// Command fetching loop
	while (!alldone)
	{
		// Get the next command from the local command array
		if (!cf->use_stdin)
		{
			if (strncmp(&(cf->snd[cmd_ix][0]), "LOOP", 4) == 0)
			{
				if (loop_max == 0)
				{
					if (sscanf(&(cf->snd[cmd_ix][0]), "LOOP %d TO %d", &loop_ix, &loop_max) == 2)
					{
						cmd_loop_ix = cmd_ix+1;
					}
					else if (sscanf(&(cf->snd[cmd_ix][0]), "LOOP %d", &loop_max) == 1)
					{
						loop_ix = 1;
						cmd_loop_ix = cmd_ix+1;
					}
					else
					{
						printf("syntax error in command[%d].\n", cmd_ix);
						err = -1;
						break;
					}
				}
				cmd_ix++;
			}
			else if (strncmp(&(cf->snd[cmd_ix][0]), "NEXT", 4) == 0)
			{
				if (loop_ix < loop_max)
				{
					cmd_ix = cmd_loop_ix;
					loop_ix++;
				}
				else
				{
					loop_max = 0;
					cmd_ix++;
				}
			}

			// Check for end of sequence
			if (strncmp(&(cf->snd[cmd_ix][0]), "END", 3) == 0)
			{
				cf->use_stdin = 1;
			}

			// Do not exceed size of command list
			if (cmd_ix >= cf->cmd_cnt)
			{
				cmd_ix = (cf->cmd_cnt-1);
			}
			cmd = &(cf->snd[cmd_ix][0]);
		}

		// use stdin for commands
		if (cf->use_stdin)
		{
			cmd_stdin_done = 0;
			while (!cmd_stdin_done)
			{
				printf("(%d) >", cf->cmd_index);
				if(fgets(&cmd_stdin[0],768, stdin) < 0)
				{
					printf("failed to get input from stdin\n");
					return(-1);
				}
				// check for exec command
				if (strncmp(cmd_stdin, "exec ", 5) == 0)
				{
					sscanf(&cmd_stdin[5], "%s", cf->cmd_file);
					if (CmdFetcher_ParseCmdFile(cf) < 0)
					{
						printf("command %s failed\n", &cmd_stdin[0]);
						cmd_ix++;
					}
					else
					{
						cmd = &(cf->snd[0][0]);
						cmd_ix = 0;
						cmd_stdin_done = 1;
						cf->use_stdin = 0;
					}
					
				}
				else
				{
					cmd = &cmd_stdin[0];
					cmd_stdin_done = 1;
				}
			}
		}

		// Check for commands that apply to local client
		post_cmd = 1;
		post_daq = 0;
		if (strncmp(cmd, "DAQ", 3) == 0)
		{
			alldone  = 0;
			post_cmd = 0;
			post_daq = 1;
		}
		else if (strncmp(cmd, "\n", 1) == 0)
		{
			//printf("Empty command\n");
			post_cmd = 0;
		}
		else if (strncmp(cmd, "exit", 4) == 0)
		{
			alldone  = 1;
			post_cmd = 0;
		}
		else if (strncmp(cmd, "quit", 4) == 0)
		{
			alldone  = 1;
			post_cmd = 0;
		}
		else if (strncmp(cmd, "version", 7) == 0)
		{
			printf("Client Version %d.%d Compiled %s at %s\n", CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, client_date, client_time);
			sprintf(tmp_str, "version\n");
			cmd = &tmp_str[0];
			post_cmd = 1;
		}
		else if (strncmp(cmd, "sleep", 5) == 0)
		{
			if (sscanf(cmd, "sleep %d\n", &mask) == 1)
			{
				Sleep(mask*1000);
			}
			post_cmd = 0;
		}
		else if (strncmp(cmd, "LIST", 4) == 0)
		{
			// See if we wnt to list pedestal or thresholds
			if ((err = sscanf(cmd, "%s %s %s", &str[0][0], &str[1][0], &str[2][0]) == 3))
			{
				if (strncmp(str[2], "ped", 3) == 0)
				{
					fa->is_list_fr_pnd = 1; // List Pedestals
					fa->is_first_fr = 1;
					fa->list_fr_cnt = cf->cur_fem_cnt;
					post_cmd = 1;
				}
				else if (strncmp(str[2], "thr", 3) == 0)
				{
					fa->is_list_fr_pnd = 2; // List Thresholds
					fa->is_first_fr = 1;
					fa->list_fr_cnt = cf->cur_fem_cnt;
					post_cmd = 1;
				}
				else
				{
					post_cmd = 0;
				}
			}
			else
			{
				post_cmd = 0;
			}

			// Post the command if the syntax is correct
			if (post_cmd)
			{
				// Make the actual command by changing "LIST" to lower case
				if (fa->is_arc)
				{
					sprintf(tmp_str, "list %s", (cmd + 5));
				}
				else
				{
					sprintf(tmp_str, "fe list %s", (cmd + 5));
				}
				strcpy(cmd, tmp_str);
			}
			else
			{
				printf("%s.rep(?): Syntax: LIST <*|asic_begin:asic_end|asic> <ped|thr>\n", srv_tar_str);
			}
			
		}
		else if (strncmp(cmd, "be cmd clr", 10) == 0)
		{
			if (cf->cur_multi_fem)
			{
				mask = 0x1;
				for (j=0; j<MAX_NUMBER_OF_FEMINOS; j++)
				{
					if (cf->cur_multi_fem & mask)
					{
						FemProxy_MsgStatClear(&(fa->fp[j]));
					}
					mask = mask << 1;
				}
			}
			else
			{
				FemProxy_MsgStatClear(&(fa->fp[cf->cur_single_fem]));
			}
			post_cmd = 1;
		}
		// Command to show/restore FEM credits
		else if (strncmp(cmd, "credits", 7) == 0)
		{
			param[0] = 0;
			if (strncmp(cmd, "credits show", 12) == 0)
			{
				param[0] = 0;
			}
			else if (strncmp(cmd, "credits restore", 15) == 0)
			{
				if (sscanf(cmd, "credits restore %i %i %c", &param[1], &param[2], &tmp_str[0]) == 3)
				{
					if (tmp_str[0] == 'F')
					{
						fa->cred_unit = 'F'; // unit is frames
					}
					else
					{
						fa->cred_unit = 'B'; // credit unit is bytes
					}
				}
				else
				{
					param[1] = MAX_REQ_CREDIT_FRAMES;
					param[2] = CREDIT_THRESHOLD_FRAMES_FOR_REQ;
					fa->cred_unit = 'F'; // credit unit is frames
				}
				param[0] = 1;
			}

			mask = 0x1;
			for (j=0; j<MAX_NUMBER_OF_FEMINOS; j++)
			{
				if (fa->fem_proxy_set & mask)
				{
					if (param[0] == 0)
					{
						if (fa->cred_unit == 'F')
						{ 
							sprintf(tmp_str, "Frame(s)");
						}
						else
						{ 
							sprintf(tmp_str, "Bytes");
						}
						printf("srv(%02d) Credits = %d %s Request_Threshold = %d %s\n", j, fa->fp[j].req_credit, tmp_str, fa->req_threshold, tmp_str);
					}
					else if (param[0] == 1)
					{
						fa->fp[j].req_credit = param[1];
						fa->req_threshold    = param[2];
					}
					fa->fp[j].req_seq_nb = 0;
				}
				mask = mask << 1;
			}
			post_cmd = 0;
		}
		// Check if the command reads or changes to which Server(s) commands are sent
		else if ((strncmp(cmd, "srv ", 4) == 0) || (strncmp(cmd, "srv\n", 4) == 0))
		{
			// Wildcard to address all Servers?
			if (strncmp(cmd, "srv *", 5) == 0)
			{
				cf->cur_multi_fem = fa->fem_proxy_set;
				sprintf(srv_tar_str, "srv( *)");
			}
			else
			{
				// Multi Server parameter supplied?
				if (sscanf(cmd, "srv 0x%x\n", &mask) == 1)
				{
					cf->cur_multi_fem = fa->fem_proxy_set & mask;
					sprintf(srv_tar_str, "srv(0x%08x)", cf->cur_multi_fem);
				}
				else
				{
					// Single FEM parameter supplied?
					if (sscanf(cmd, "srv %d\n", &mask) == 1)
					{
						if (fa->fem_proxy_set & (1<<mask))
						{
							cf->cur_single_fem = mask;
							cf->cur_multi_fem  = 0;
							sprintf(srv_tar_str, "srv(%02d)", mask);
						}
						else
						{
							printf("srv(%02d).rep(.): Warning Server %d does not exist.\n", cf->cur_single_fem, mask);
						}
					}
				}
			}
			CmdFetcher_UpdateFemBoundaries(cf);
			if (cf->verbose >= 1)
			{
				if (cf->cur_multi_fem)
				{
					if (cf->cur_multi_fem == fa->fem_proxy_set)
					{
						printf("%s.rep(.): command apply to all Servers from %02d to %02d (%d Servers)\n",
							srv_tar_str,
							cf->fem_ix_min,
							cf->fem_ix_max,
							cf->cur_fem_cnt);
					}
					else
					{
						printf("%s.rep(.): command apply to Server subset 0x%08x (first:%02d  last:%02d  %d Servers)\n",
							srv_tar_str,
							cf->cur_multi_fem,
							cf->fem_ix_min,
							cf->fem_ix_max,
							cf->cur_fem_cnt);
					}
				}
				else 
				{
					printf("%s.rep(.): command apply to Server %d (%d Server)\n",
						srv_tar_str,
						cf->cur_single_fem,
						cf->cur_fem_cnt);
				}
			}
			post_cmd = 0;
		}
		// debug printout verboseness
		else if (strncmp(cmd, "verbose", 7) == 0)
		{
			if (sscanf(cmd, "verbose %d\n", &(cf->verbose)) == 1)
			{
				verbose = cf->verbose;
			}
			if (cf->verbose > 1)
			{
				printf("%s.rep(?): verbose = %d\n", srv_tar_str, cf->verbose);
			}
			post_cmd = 0;

		}
		// event data printout verboseness
		else if (strncmp(cmd, "vflags", 6) == 0)
		{
			if (sscanf(cmd, "vflags 0x%x\n", &(eb->vflags)) == 1)
			{

			}
			else if (cf->verbose > 1)
			{
				printf("%s.rep(?): vflags = 0x%x\n", srv_tar_str, eb->vflags);
			}
			post_cmd = 0;

		}
		// open result file
		else if (strncmp(cmd, "fopen", 5) == 0)
		{
			if (strncmp(cmd, "fopen asc", 9) == 0)
			{
				mask = 1; // save data in ASCII 
			}
			else if (strncmp(cmd, "fopen bin", 9) == 0)
			{
				mask = 2; // save data in binary
			}
			else
			{
				mask = 2;
			}
			EventBuilder_FileAction(eb, EBFA_OpenFirst, mask);
			post_cmd = 0;
		}
		// close result file
		else if (strncmp(cmd, "fclose", 6) == 0)
		{
			EventBuilder_FileAction(eb, EBFA_CloseLast, 0);
			post_cmd = 0;
		}
		// result file path
		else if (strncmp(cmd, "path", 4) == 0)
		{
			if (sscanf(cmd, "path %s\n", &tmp_str[0]) == 1)
			{
				if (tmp_str[strlen(&tmp_str[0])-1] == '/')
				{
					sprintf(&(eb->file_path[0]),"%s", &tmp_str[0]);
				}
				else
				{
					sprintf(&(eb->file_path[0]),"%s/", &tmp_str[0]);
				}
			}
			printf("%s.rep(?): path: \"%s\"\n", srv_tar_str, &(eb->file_path[0]));
			post_cmd = 0;
		}
		// Event Builder mode
		else if (strncmp(cmd, "event_builder", 13) == 0)
		{
			if (sscanf(cmd, "event_builder %i\n", &param[0]) == 1)
			{
				if ((param[0] >= 0) && (param[0] <16))
				{
					eb->eb_mode = param[0];
				}
			}
			// Flush any data left over in the Event Builder
			EventBuilder_Flush(eb);
			printf("%s.rep(?): Event Builder Mode: %s (0x%x)\n", srv_tar_str, &EventBuilder_Mode2str[eb->eb_mode][0], eb->eb_mode);
			post_cmd = 0;
		}
		// File chunk size
		else if (strncmp(cmd, "file_chunk", 10) == 0)
		{
			if (sscanf(cmd, "file_chunk %d\n", &param[0]) == 1)
			{
				if (param[0] <= 2048)
				{
					eb->file_max_size = param[0] * 1024 * 1024;
				}
			}
			printf("%s.rep(?): File Chunk: %d MBytes\n", srv_tar_str, eb->file_max_size / (1024 * 1024));
			post_cmd = 0;
		}
		// File name suffix
		else if (strncmp(cmd, "file_suffix", 11) == 0)
		{
			if (sscanf(cmd, "file_suffix %s\n", &(eb->file_suff[0])) == 1)
			{
				
			}
			printf("%s.rep(?): File Name Suffix: \"%s\"\n", srv_tar_str, &(eb->file_suff[0]));
			post_cmd = 0;
		}
		// Drop credit to inject an error
		else if (strncmp(cmd, "drop credit", 11) == 0)
		{
			fa->drop_a_credit = 1;
			printf("%s.rep(?): One credit frame will be dropped\n", srv_tar_str);
			post_cmd = 0;
		}
		// Delay credit to inject an error
		else if (strncmp(cmd, "delay credit", 12) == 0)
		{
			if (sscanf(cmd, "delay credit %d\n", &fa->delay_a_credit) == 1)
			{

			}
			else
			{
				fa->delay_a_credit = 1000;
			}
			printf("%s.rep(?): Sending credit will be delayded by %d ms\n", srv_tar_str, fa->delay_a_credit);
			post_cmd = 0;
		}
		// Re-program Front-End Flash with new firmware
		else if (strncmp(cmd, "fe program flash", 16) == 0)
		{
			err = 0;

			// Get file name
			if (sscanf(cmd, "fe program flash %s %s", &tmp_str[0], cf->cmd_file) != 2)
			{
				printf("%s.rep(?): syntax: fe program flash <Winbond|Spansion> <prom_file.mcs>\n", srv_tar_str);
				err = -1;
				cf->no_echo_this = 1;
			}
			else
			{
				// Check flash model
				if (strcmp(&tmp_str[0], "Winbond") == 0)
				{
					param[0] = FLASH_TYPE_WINBOND_W25Q128BV;
				}
				else if (strcmp(&tmp_str[0], "Spansion") == 0)
				{
					param[0] = FLASH_TYPE_SPANSION_S25FL512S;
				}
				else
				{
					printf("%s.rep(?): unsupported flash type \"%s\"\n", srv_tar_str, tmp_str);
					err = -1;
					cf->no_echo_this = 1;
				}

				if (err == 0)
				{
					// Check that file name extension is .mcs
					if (strncmp(&cf->cmd_file[strlen(cf->cmd_file) - 4], ".mcs", 4) != 0)
					{
						printf("%s.rep(?): file format must be \".mcs\" not \"%s\"\n", srv_tar_str, &cf->cmd_file[strlen(cf->cmd_file) - 4]);
						err = -1;
						cf->no_echo_this = 1;
					}

					// Check that the command applies only to one node
					if (cf->cur_multi_fem != 0)
					{
						printf("%s.rep(?): command cannot be applied to multiple nodes.\n", srv_tar_str);
						err = -1;
						cf->no_echo_this = 1;
					}

					if (err == 0)
					{
						// Program flash
						if ((err = CmdFetcher_ProgramFlash(cf, param[0])) < 0)
						{
							printf("%s.rep(?): CmdFetcher_ProgramFlash failed with error code: %d.\n", srv_tar_str, err);
						}
						else
						{
							printf("%s.rep(?): CmdFetcher_ProgramFlash completed.\n", srv_tar_str);
						}
					}
				}
			}
			post_cmd = 0;
		}
		// Commands that use the loop index as a variable in 4th or 5th argument
		else if ((err = sscanf(cmd, "%s %s %s %s %s", &str[0][0], &str[1][0], &str[2][0], &str[3][0], &str[4][0])) >= 4)
		{
			if ((err == 5) && (strncmp(&str[4][0], "$loop", 5) == 0))
			{
				sprintf(tmp_str, "%s %s %s %s 0x%x\n", &str[0][0], &str[1][0], &str[2][0], &str[3][0], loop_ix);
				cmd = &tmp_str[0];
				post_cmd = 1;
			}
			else if ((err == 4) && (strncmp(&str[3][0], "$loop", 5) == 0))
			{
				sprintf(tmp_str, "%s %s %s 0x%x\n", &str[0][0], &str[1][0], &str[2][0], loop_ix);
				cmd = &tmp_str[0];
				post_cmd = 1;
			}
		}
		// Make a copy of a file on the remote SD-Card of the TDCM
		else if (strncmp(cmd, "rcp", 3) == 0)
		{
			err = 0;
			// Get file names
			if (sscanf(cmd, "rcp %s %s", &src_file[0], &dst_file[0]) != 2)
			{
				printf("%s.rep(?): syntax: rcp <loc_src_file> <rem_dst_file>\n", srv_tar_str);
				err = -1;
				cf->no_echo_this = 1;
			}
			// Check that the command applies only to one Server
			if (cf->cur_multi_fem != 0)
			{
				printf("%s.rep(?): command cannot be applied to multiple Servers.\n", srv_tar_str);
				err = -1;
				cf->no_echo_this = 1;
			}
			if (err == 0)
			{
				// Make copy to remote SD Card
				if ((err = CmdFetcher_RemoteCopy(cf, &src_file[0], &dst_file[0])) < 0)
				{
					printf("%s.rep(?): CmdFetcher_RemoteCopy failed with error code: %d.\n", srv_tar_str, err);
				}
				else
				{
					printf("%s.rep(?): CmdFetcher_RemoteCopy completed.\n", srv_tar_str);
				}
			}
			post_cmd = 0;
		}

		// Set time and date on Feminos or TCM
		else if (strncmp(cmd, "iic mod set_clock", 17) == 0)
		{
			err = 0;

			// Get the local time
			time(&the_time);
			now = localtime(&the_time);

			// Rebuild a new command with the current date and time
			sprintf(tmp_str, "iic mod set_clock %d %d %d %d %d %d\n", now->tm_mday, (now->tm_mon) + 1, (now->tm_year) + 1900, now->tm_hour, now->tm_min, now->tm_sec);
			cmd = &tmp_str[0];
			post_cmd = 1;
		}
		else
		{
			
		}

		// Echo the command to console
		if ((cf->verbose > 1) && (cf->no_echo_this == 0))
		{
			printf("%s.cmd(%d): %s", srv_tar_str, cf->cmd_index, cmd);
		}
		cf->no_echo_this = 0;

		// Post command to Server(s)
		if ((post_cmd) || (post_daq))
		{
			//
			if (cf->cur_multi_fem == 0)
			{
				fem_beg = cf->cur_single_fem;
				fem_end = cf->cur_single_fem;
				fem_pat = 1<<cf->cur_single_fem;
			}
			else
			{
				fem_beg = 0;
				fem_end = cf->fem_ix_max;
				fem_pat = cf->cur_multi_fem;
			}

			// Pass the command to the FemArray that will send it
			if (post_cmd)
			{
				if ((err = FemArray_SendCommand(fa, fem_beg, fem_end, fem_pat, cmd)) < 0)
				{
					return (err);
				}

				// Wait completion of the command to be signalled by network receive thread
				if ((err = Semaphore_Wait_Timeout(cf->sem_cur_cmd_done, 4000000)) < 0)
				{
					if (err == -2)
					{
						printf("CmdFetcher_Main: Semaphore_Wait_Timeout: timeout detected.\n");
					}
					else
					{
						printf("CmdFetcher_Main: Semaphore_Wait_Timeout failed %d\n", err);
					}
				}
			}
			else
			{
				if ((err = FemArray_SendDaq(fa, fem_beg, fem_end, fem_pat, cmd)) < 0)
				{
					return (err);
				}

				// check DAQ completion
				if (fa->daq_size_left == 0)
				{
					// Trick to break the command LOOP
					loop_ix = loop_max;
				}
			}
		}

		// Point to the next command in the array
		if (cf->use_stdin == 0)
		{
			cmd_ix++;
		}
		else
		{
			cf->cmd_index++;
		}
	}

	// Stop Server array network receiver thread
	fa->state = 0;

	// Stop the event builder
	eb->state = 0;
	// Wakeup the event builder
	if ((err = Semaphore_Signal(eb->sem_wakeup)) < 0)
	{
		printf("CmdFetcher_Main: Semaphore_Signal failed %d\n", err);
		return(err);
	}

	printf("CmdFetcher_Main: completed.\n");
	return(0);
}

/*******************************************************************************
 CmdFetcher_ParseCmdFile
*******************************************************************************/
int CmdFetcher_ParseCmdFile(CmdFetcher *cf)
{
	FILE *fptr;
	int ix = 0;
	int err = 0;
	int sl_comment, ml_comment, ml_cnext;

	if (!(fptr = fopen(&(cf->cmd_file[0]), "r")))
	{
		return(-1);
	}

	// scan all lines
	cf->cmd_cnt = 0;
	ml_comment  = 0;
	ml_cnext    = 0;
	while (fgets(cf->snd[ix], CMD_LINE_SIZE, fptr))
	{
		// check for single or multi-line comments
		sl_comment = 0;
		ml_cnext   = ml_comment;
		if (cf->snd[ix][0] == '/')
		{
			if (cf->snd[ix][1] == '/')
			{
				sl_comment = 1;
			}
			if (cf->snd[ix][1] == '*')
			{
				ml_comment = 1;
			}
		}
		if ( (cf->snd[ix][0] == '*') && (cf->snd[ix][1] == '/') )
		{
			ml_comment = 0;
		}
		
		// accept command line unless commented, empty line, etc
		if ( (cf->snd[ix][0] != '\n') &&
			(cf->snd[ix][0] != ' ') &&
			(cf->snd[ix][0] != '\t') &&
			(cf->snd[ix][0] != '#') &&
			(sl_comment == 0) &&
			(ml_comment == 0) &&
			(ml_cnext == 0)
			)
		{
			if (ix < (CMD_ARRAY_SIZE-2))
			{
				cf->cmd_cnt++;
				ix++;
			}
			else
			{
				printf("maximum command count (%d) reached.\n", cf->cmd_cnt);
				err = -1;
				break;
			}
		}
	}

	// Check for unterminated commented section
	if (ml_comment)
	{
		printf("unterminated commented section. Use \"*/\" at beginning of line.\n");
		err = -1;
	}

	// Always append a end command in case the user forgot it
	sprintf(cf->snd[ix], "END\n");
	cf->cmd_cnt++;
	fclose(fptr);
	return (err);
}

