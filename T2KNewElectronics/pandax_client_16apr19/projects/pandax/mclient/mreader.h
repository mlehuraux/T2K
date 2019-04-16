/*******************************************************************************

_____________________

File:        mreader.h

Description: Definitions for the application to analyze the data files recorded
by the Feminos readout system. It provides a number of options to decode the
content of such binary files and update statistics.


Author:      D. Calvet,        calvet@hep.saclay.cea.fr


History:
September 2014    : created


*******************************************************************************/

#ifndef MREADER_H
#define MREADER_H

#include "histo_int.h"
#include "accu_array.h"
#include "platform_spec.h"

#define MAX_NB_OF_FEMINOS 24
#define ASIC_PER_FEMINOS   4

// Maximum event size is:
// 24 Feminos * 4 ASICs * 80 channels * 512 time-bins * 2 bytes = 7.5 MByte
#define MAX_EVENT_SIZE (MAX_NB_OF_FEMINOS * ASIC_PER_FEMINOS * 80 * 512 * 2)

typedef struct _Features {
	__int64  tot_file_rd;                    // Total number of bytes read from file
	int  tot_fr_cnt;                         // Total number of frames read from file
	int  tot_sobe;                           // Total number of Start of Built Event
	int  tot_sobe_size;                      // Total number of Start of Built Event with size
	int  tot_eobe;                           // Total number of End of Built Event
	int  tot_fem_soe;                        // Total number of Feminos Start of Event
	int  tot_fem_eoe;                        // Total number of Feminos End of Event
	char run_str[256];                       // Run information string
	
	histoint h_event_size;                   // Histogram of built event size
	unsigned int h_event_size_bins[1024];    // Bins of histogram
	__int64 sum_built_event_size;            // Sum of the size of all built events with size
	
	histoint h_glo_asic_ch_hit;              // Histogram of ASIC channel hit count per event, all ASICs together in the same histogram
	unsigned int h_glo_asic_ch_hit_bins[80]; // Bins of histogram

	histoint h_glo_asic_ch_ne;               // Histogram of count of non empty ASIC channels, all ASICs together in the same histogram
	unsigned int h_glo_asic_ch_ne_bins[80];  // Bins of histogram

	histoint h_highest_ch_hit;               // Histogram of the highest channel hit count in an ASIC per event
	unsigned int h_highest_ch_hit_bins[80];  // Bins of histogram
	
	histoint h_asic_ch_hit[MAX_NB_OF_FEMINOS][ASIC_PER_FEMINOS]; // Histograms of ASIC channel hit count for each ASIC separately
	unsigned int h_asic_ch_hit_bins[MAX_NB_OF_FEMINOS][ASIC_PER_FEMINOS][80]; // Bins of histograms

	AccuArray a_ch_hit_per_asic;             // Accumulated ASIC channel hit count for each ASIC
	unsigned int ch_hit_per_asic[MAX_NB_OF_FEMINOS*ASIC_PER_FEMINOS]; // Array of accumulators

	histoint h_inter_event_time;              // Histograms of inter-event time
	unsigned int h_inter_event_time_bins[1024]; // Bins of histograms

	histoint h_cha_data_size;                // Histogram of channel data size
	unsigned int h_cha_data_size_bins[2080]; // Bins of histogram

	AccuArray a_first_time_bin_othr;         // Accumulated first time bin over threshold
	unsigned int first_time_bin_othr[512];   // Array of accumulators

	unsigned char cur_fr[MAX_EVENT_SIZE];    // Current frame
	int  cur_event_size;                     // Current size of the event being read out
	int  cur_highest_ch_hit_count;           // Current highest ASIC channel hit count seen in this event
	unsigned short cur_ts_h;                 // Current event timestamp High 
	unsigned short cur_ts_m;                 // Current event timestamp Medium
	unsigned short cur_ts_l;                 // Current event timestamp Low
	unsigned char  cur_fem;                  // Current FEM
	unsigned char  cur_asic;                 // Current ASIC
	unsigned char  cur_cha;                  // Current Channel
	int  is_eb_active;                       // Tells if the event builder is active
	int  got_first_fragment;                 // Flag to indicate if we had the first data fragment for this event
	int cur_cha_data_size;                   // Current size of channel data                            
	int cur_ne_ch[MAX_NB_OF_FEMINOS][ASIC_PER_FEMINOS]; // Current count of non empty channels for each ASIC
} Features;

#endif
