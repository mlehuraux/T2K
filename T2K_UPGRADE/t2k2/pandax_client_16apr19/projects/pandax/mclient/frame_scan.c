/*******************************************************************************
                  Minos -> PandaX-III / TDCM - Harpo / ROC
                  ________________________________________

 File:        frame_scan.c

 Description: Frame Decoder


 Author:      D. Calvet,        denis.calvetATcea.fr
              

 History:

   September 2014: created 

   August 2018: added decoding for prefix PFX_EXTD_CARD_CHIP_CHAN_HIT_IX

   October 2018: added decoder for PFX_EXTD_CARD_CHIP_LAST_CELL_READ

   November 2018: corrected bug on accumulation of hit channel count in various
   histograms (corresponding to the processing within PFX_CHIP_CHAN_HIT_CNT)
   Updated hitstograms as needed when PFX_START_OF_EVENT and PFX_END_OF_EVENT
   are found (same as PFX_START_OF_EVENT_MINOS and PFX_END_OF_EVENT_MINOS which
   are no longer used)

*******************************************************************************/
#include "frame.h"
#include "frame_scan.h"
#include "platform_spec.h"
#include <stdio.h>

/*******************************************************************************
 Frame_Scan
*******************************************************************************/
void Frame_Scan(Features *f, void *fp, void *fr, int fr_sz, unsigned int vflg)
{
	unsigned short *p;
	int i, j, k;
	int sz_rd;
	int done = 0;
	unsigned short r0, r1, r2, r3;
	unsigned int tmp;
	volatile unsigned int tmp_vui;
	int tmp_i[10];
	int si;
	char *c;
	unsigned int *ui;
	float mean, std_dev, ber;
	char tmp_str[10];
	__int64 tstamp, prev_ts;
	int cur_src_id;

	p = (unsigned short *) fr;

	done       = 0;
	i          = 0;
	sz_rd      = 0;
	si         = 0;
	cur_src_id = 0;

	if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_SIZE))
	{
		fprintf((FILE *)fp, "Frame payload: %d bytes\n", fr_sz);
	}

	while (!done)
	{
		// Is it a prefix for 14-bit content?
		if ((*p & PFX_14_BIT_CONTENT_MASK) == PFX_CARD_CHIP_CHAN_HIT_IX)
		{
			r0 = GET_CARD_IX(*p);
			r1 = GET_CHIP_IX(*p);
			r2 = GET_CHAN_IX(*p);

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HIT_CH))
			{
				fprintf((FILE *)fp, "Card %02d Chip %01d Channel %02d\n", r0, r1, r2);
			}

			// Update which are the current FEM, ASIC and Channel
			f->cur_fem  = r0;
			f->cur_asic = r1;
			f->cur_cha  = r2;

			// Append the accumulated event fragment size to its histogram
			if (f->cur_cha_data_size != -1)
			{
				HistoInt_AddEntry(&(f->h_cha_data_size), f->cur_cha_data_size);
			}
			f->cur_cha_data_size = 0;

			// Increment the non-empty channel count for this ASIC
			f->cur_ne_ch[r0][r1]++;

			i++;
			p++;
			sz_rd+=2;
			si = 0;
		}
		else if ((*p & PFX_14_BIT_CONTENT_MASK) == PFX_CARD_CHIP_CHAN_HISTO)
		{
			r0 = GET_CARD_IX(*p);
			r1 = GET_CHIP_IX(*p);
			r2 = GET_CHAN_IX(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HIT_CH))
			{
				fprintf((FILE *)fp, "Card %02d Chip %01d Channel %02d ", r0, r1, r2);
			}
			i++;
			p++;
			sz_rd+=2;
		}

		// Is it a prefix for 12-bit content?
		else if ((*p & PFX_12_BIT_CONTENT_MASK) == PFX_ADC_SAMPLE)
		{
			r0 = GET_ADC_DATA(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_CHAN_DATA))
			{
				fprintf((FILE *)fp, "%03d 0x%04x (%4d)\n", si, r0, r0);
			}

			// Increment size of current channel data
			f->cur_cha_data_size += 2;

			i++;
			p++;
			sz_rd+=2;
			si++;
		}
		else if ((*p & PFX_12_BIT_CONTENT_MASK) == PFX_LAT_HISTO_BIN)
		{
			r0 = GET_LAT_HISTO_BIN(*p);
			i++;
			p++;
			sz_rd+=2;
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			r2 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp = (((unsigned int) r2) << 16) | (unsigned int) (r1);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_BINS))
			{
				fprintf((FILE *)fp, "%03d %03d\n", r0, tmp);
			}
		}
		else if ((*p & PFX_11_BIT_CONTENT_MASK) == PFX_CHIP_LAST_CELL_READ)
		{
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			r2 = *p;
			i++;
			p++;
			sz_rd+=2;
			r3 = *p;
			i++;
			p++;
			sz_rd+=2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LAST_CELL_READ))
			{
				fprintf((FILE *)fp, "Chip %01d Last_Cell_Read %03d (0x%03x)\n",
					GET_CHIP_IX_LCR(r0),
					GET_LAST_CELL_READ(r0),
					GET_LAST_CELL_READ(r0));
			
				fprintf((FILE *)fp, "Chip %01d Last_Cell_Read %03d (0x%03x)\n",
					GET_CHIP_IX_LCR(r1),
					GET_LAST_CELL_READ(r1),
					GET_LAST_CELL_READ(r1));
			
				fprintf((FILE *)fp, "Chip %01d Last_Cell_Read %03d (0x%03x)\n",
					GET_CHIP_IX_LCR(r2),
					GET_LAST_CELL_READ(r2),
					GET_LAST_CELL_READ(r2));
			
				fprintf((FILE *)fp, "Chip %01d Last_Cell_Read %03d (0x%03x)\n",
					GET_CHIP_IX_LCR(r3),
					GET_LAST_CELL_READ(r3),
					GET_LAST_CELL_READ(r3));
			}
		}

		// Is it a prefix for 9-bit content?
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_TIME_BIN_IX)
		{
			r0 = GET_TIME_BIN(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_CHAN_DATA))
			{
				fprintf((FILE *)fp, "Time_Bin: %d\n", r0);
			}

			// Put time bin index in accumulator array
			// But we only want this histogram for the TPC, so we only take this on FEM0 to FEM 19
			if ((f->cur_fem >= 0) && (f->cur_fem < 20))
			{
				AccuArray_Accumulate(&(f->a_first_time_bin_othr), r0, 1);
			}

			// Increment size of current channel data
			f->cur_cha_data_size += 2;

			i++;
			p++;
			sz_rd+=2;
			si = 0;
		}
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_HISTO_BIN_IX)
		{
			r0 = GET_HISTO_BIN(*p);
			i++;
			p++;
			sz_rd += 2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_BINS))
			{
				fprintf((FILE *)fp, "Bin %3d Val %5d\n", r0, *p);
			}
			i++;
			p++;
			sz_rd += 2;
		}
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_PEDTHR_LIST)
		{
			r0 = GET_PEDTHR_LIST_FEM(*p);
			r1 = GET_PEDTHR_LIST_ASIC(*p);
			r2 = GET_PEDTHR_LIST_MODE(*p);
			r3 = GET_PEDTHR_LIST_TYPE(*p);
			i++;
			p++;
			sz_rd+=2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LISTS))
			{
				if (r3 == 0) // pedestal entry
				{
					fprintf((FILE *)fp, "# Pedestal List for FEM %02d ASIC %01d\n", r0, r1);
				}
				else
				{
					fprintf((FILE *)fp, "# Threshold List for FEM %02d ASIC %01d\n", r0, r1);
				}
				fprintf((FILE *)fp, "fem %02d\n", r0);
			}

			// Determine the number of entries
			if (r2 == 0) // AGET
			{
				r2 = 71;
			}
			else // AFTER
			{
				r2 = 78;
			}
			// Get all entries
			for (j=0;j<=r2; j++)
			{
				tmp_i[0] = (int) * ((short *) p);
				if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LISTS))
				{
					if (r3 == 0) // pedestal entry
					{
						sprintf(tmp_str, "ped");
					}
					else
					{
						sprintf(tmp_str, "thr");
					}
					if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LISTS))
					{
						fprintf((FILE *)fp, "%s %1d %2d 0x%04x (%4d)\n", tmp_str, r1, j, *p, tmp_i[0]);
					}
				}
				i++;
				p++;
				sz_rd+=2;
			}
		}
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_DFRAME)
		{
			r0 = GET_VERSION_FRAMING(*p);
			r1 = GET_SOURCE_TYPE(*p);
			sourcetype2str((unsigned short) r1, &tmp_str[0]);
			r2 = GET_SOURCE_ID(*p);
			i++;
			p++;
			sz_rd+=2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_FRBND))
			{
				fprintf((FILE *)fp, "--- Start of Data Frame (V.%01d) %s %02d --\n", r0, &tmp_str[0], r2);
				fprintf((FILE *)fp, "Filled with %d bytes\n", *p);
			}
			i++;
			p++;
			sz_rd+=2;
		}
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_MFRAME)
		{
			r0 = GET_VERSION_FRAMING(*p);
			r1 = GET_SOURCE_TYPE(*p);
			sourcetype2str((unsigned short)r1, &tmp_str[0]);
			r2 = GET_SOURCE_ID(*p);
			i++;
			p++;
			sz_rd+=2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_FRBND))
			{
				fprintf((FILE *)fp, "--- Start of Moni Frame (V.%01d) %s %02d --\n", r0, &tmp_str[0], r2);
				fprintf((FILE *)fp, "Filled with %d bytes\n", *p);
			}
			i++;
			p++;
			sz_rd+=2;
		}
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_CFRAME)
		{
			r0 = GET_VERSION_FRAMING(*p);
			r1 = GET_SOURCE_TYPE(*p);
			sourcetype2str((unsigned short)r1, &tmp_str[0]);
			r2 = GET_SOURCE_ID(*p);
			i++;
			p++;
			sz_rd+=2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_FRBND))
			{
				fprintf((FILE *)fp, "--- Start of Config Frame (V.%01d) %s %02d --\n", r0, &tmp_str[0], r2);
				fprintf((FILE *)fp, "Error code: %d\n", *((short *)p));
			}
			i++;
			p++;
			sz_rd+=2;
		}
		else if ((*p & PFX_9_BIT_CONTENT_MASK) == PFX_CHIP_CHAN_HIT_CNT)
		{
			r0 = GET_CHIP_IX_CHC(*p);
			r1 = GET_CHAN_HIT_CNT(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HIT_CNT))
			{
				fprintf((FILE *)fp, "Chip %01d Channel_Hit_Count %02d\n", r0, r1);
			}

			// Add the number of hit channels to the global ASIC hit channel histogram
			HistoInt_AddEntry(&(f->h_glo_asic_ch_hit), r1);

			// Add the number of hit channels to the individual ASIC hit channel histogram
			HistoInt_AddEntry(&(f->h_asic_ch_hit[cur_src_id][r0]), r1);

			// Add the number of hit channels to the histogram of the accumulated sum of hit channels of each individual ASIC
			AccuArray_Accumulate(&(f->a_ch_hit_per_asic), (4*cur_src_id+r0), r1);

			// See if the number of channel hit in this ASIC is greater than that detected so far in this event
			if (r1 > f->cur_highest_ch_hit_count)
			{
				f->cur_highest_ch_hit_count = r1;
			}

			i++;
			p++;
			sz_rd += 2;
		}

		// Is it a prefix for 8-bit content?
		else if ((*p & PFX_8_BIT_CONTENT_MASK) == PFX_ASCII_MSG_LEN)
		{
			r0 = GET_ASCII_LEN(*p);
			i++;
			p++;
			sz_rd += 2;
			c = (char *)p;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_ASCII))
			{
				for (j = 0; j<r0; j++)
				{
					fprintf((FILE *)fp, "%c", *c);
					c++;
				}
			}
			// Skip the null string terminating character
			r0++;
			// But if the resulting size is odd, there is another null character that we should skip
			if (r0 & 0x0001)
			{
				r0++;
			}
			p += (r0 >> 1);
			i += (r0 >> 1);
			sz_rd += r0;
		}
		else if ((*p & PFX_8_BIT_CONTENT_MASK) == PFX_START_OF_EVENT)
		{
			// Event type, source type and ID
			r0 = GET_SOE_EV_TYPE(*p);
			r1 = GET_SOE_SOURCE_TYPE(*p);
			sourcetype2str(r1, &tmp_str[0]);
			r2 = GET_SOE_SOURCE_ID(*p);
			cur_src_id = r2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "-- Start of Event (Type %01d From %s %02d) --\n", r0, &tmp_str[0], r2);
			}
			i++;
			p++;
			sz_rd += 2;

			// Time Stamp lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd += 2;

			// Time Stamp middle 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd += 2;

			// Time Stamp upper 16-bit
			r2 = *p;
			i++;
			p++;
			sz_rd += 2;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "Time 0x%04x 0x%04x 0x%04x\n", r2, r1, r0);
			}

			// If the flag telling if the event builder is used or not is not set, we can now determine that the event builder is not used
			if (f->is_eb_active == -1)
			{
				f->is_eb_active = 0;
			}
			f->tot_fem_soe++;

			// Initialisation when event builder is inactive
			if (f->is_eb_active == 0)
			{
				f->cur_event_size = 0;
				f->cur_highest_ch_hit_count = 0;
				f->got_first_fragment = 0;

				// Clear the number of non-empty channel for each ASIC
				for (k = 0; k<MAX_NB_OF_FEMINOS; k++)
				{
					for (j = 0; j<ASIC_PER_FEMINOS; j++)
					{
						f->cur_ne_ch[k][j] = 0;
					}
				}
			}

			// Is this the first fragment we see for this event
			if (f->got_first_fragment == 0)
			{
				// Do we have an event before this one with a known time stamp
				if (!((f->cur_ts_h == 0) && (f->cur_ts_m == 0) && (f->cur_ts_l == 0)))
				{
					// OK so we can compute the inter-event time
					tstamp = ((__int64)r2 << 32) | ((__int64)r1 << 16) | (__int64)(r0);
					prev_ts = ((__int64)f->cur_ts_h << 32) | ((__int64)f->cur_ts_m << 16) | ((__int64)f->cur_ts_l);
					tstamp = (tstamp - prev_ts) / 10000; // Convert in 100 us units

					// And append it to the inter event time histogram
					HistoInt_AddEntry(&(f->h_inter_event_time), (unsigned int)tstamp);
				}

				// Save time stamp of the current event
				f->cur_ts_h = r2;
				f->cur_ts_m = r1;
				f->cur_ts_l = r0;


				// We have to remember that we got the first fragment for this event
				f->got_first_fragment = 1;
			}

			// Event Count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd += 2;

			// Event Count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd += 2;

			tmp = (((unsigned int)r1) << 16) | ((unsigned int)r0);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "Event_Count 0x%08x (%d)\n", tmp, tmp);
			}
		}

		// Is it a prefix for 6-bit content?
		else if ((*p & PFX_6_BIT_CONTENT_MASK) == PFX_END_OF_EVENT)
		{
			// Source type and ID
			r0 = GET_EOE_SOURCE_TYPE(*p);
			sourcetype2str(r0, &tmp_str[0]);
			r1 = GET_EOE_SOURCE_ID(*p);
			cur_src_id = r1;
			i++;
			p++;
			sz_rd += 2;

			// Unused word
			i++;
			p++;
			sz_rd += 2;

			// Event size
			r2 = *p;
			i++;
			p++;
			sz_rd += 2;
			r3 = *p;
			i++;
			p++;
			sz_rd += 2;
			tmp = (((unsigned int)r3) << 16) | ((unsigned int)r2);

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "----- End of Event ----- (from %s %02d - size %d bytes)\n", &tmp_str[0], r1, tmp);
			}

			f->tot_fem_eoe++;

			// Put the event size in an histogram when running without event builder, i.e. only 1 Feminos
			if (f->is_eb_active == 0)
			{
				// Convert this size to KB with rounding and add this entry to the event size histogram
				HistoInt_AddEntry(&(f->h_event_size), (unsigned int)((tmp + 512) / 1024));
				f->sum_built_event_size += tmp;

				// Update the histogram of highest number of hit channel in an ASIC seen for this event
				HistoInt_AddEntry(&(f->h_highest_ch_hit), f->cur_highest_ch_hit_count);

				// Put the count of non-empty channels per ASIC in histogram 
				for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
				{
					for (j = 0; j<ASIC_PER_FEMINOS; j++)
					{
						if (f->cur_ne_ch[i][j])
						{
							HistoInt_AddEntry(&(f->h_glo_asic_ch_ne), f->cur_ne_ch[i][j]);
						}
					}
				}
			}
			else
			{
				// Increase the size of the current event by the size of this fragment
				f->cur_event_size += tmp;
			}

			// Append the accumulated event fragment size to its histogram
			// This will commit the size of the data for the last channel of the current event
			if (f->cur_cha_data_size != -1)
			{
				HistoInt_AddEntry(&(f->h_cha_data_size), f->cur_cha_data_size);
			}
			f->cur_cha_data_size = -1; // The size is not initialized for the next event
		}
		else if ((*p & PFX_6_BIT_CONTENT_MASK) == PFX_BERT_STAT)
		{
			// Source type and ID
			r0 = GET_BERT_STAT_SOURCE_TYPE(*p);
			sourcetype2str(r0, &tmp_str[0]);
			r0 = GET_BERT_STAT_SOURCE_ID(*p);
			i++;
			p++;
			sz_rd += 2;

			// Bit error count
			r1 = *p;
			i++;
			p++;
			sz_rd += 2;

			// Number of Mbit received 
			r2 = *p;
			i++;
			p++;
			sz_rd += 2;
			r3 = *p;
			i++;
			p++;
			sz_rd += 2;
			tmp = (((unsigned int)r3) << 16) | ((unsigned int)r2);

			// Calculate BER
			if (r1)
			{
				ber = ((float)r1) / ((float)tmp);
				ber = ber / 1.0e6;
			}
			else
			{
				ber = 3.0 / ((float)tmp);
				ber = ber / 1.0e6;
			}

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "BERT statistics from %s %02d RX: %d Mbit  Errors: %d", &tmp_str[0], r0, tmp, r1);
				if (r1)
				{
					fprintf((FILE *)fp, "  BER: %.2e\n", ber);
				}
				else
				{
					fprintf((FILE *)fp, "  BER < %.2e (95%% C.L.)\n", ber);
				}
			}
		}

		// Is it a prefix for 4-bit content?
		else if ((*p & PFX_4_BIT_CONTENT_MASK) == PFX_START_OF_EVENT_MINOS)
		{
			r0 = GET_EVENT_TYPE(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "-- Start of Event (Type %01d) --\n", r0);
			}
			i++;
			p++;
			sz_rd+=2;

			// Time Stamp lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;

			// Time Stamp middle 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;

			// Time Stamp upper 16-bit
			r2 = *p;
			i++;
			p++;
			sz_rd+=2;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "Time 0x%04x 0x%04x 0x%04x\n", r2, r1, r0);
			}

			// If the flag telling if the event builder is used or not is not set, we can now determine that the event builder is not used
			if (f->is_eb_active == -1)
			{
				f->is_eb_active = 0;
			}
			f->tot_fem_soe++;

			// Initialisation when event builder is inactive
			if (f->is_eb_active == 0)
			{
				f->cur_event_size = 0;
				f->cur_highest_ch_hit_count = 0;
				f->got_first_fragment = 0;

				// Clear the number of non-empty channel for each ASIC
				for (k = 0; k<MAX_NB_OF_FEMINOS; k++)
				{
					for (j = 0; j<ASIC_PER_FEMINOS; j++)
					{
						f->cur_ne_ch[k][j] = 0;
					}
				}
			}

			// Is this the first fragment we see for this event
			if (f->got_first_fragment == 0)
			{
				// Do we have an event before this one with a known time stamp
				if (!((f->cur_ts_h == 0) && (f->cur_ts_m == 0) && (f->cur_ts_l == 0)))
				{
					// OK so we can compute the inter-event time
					tstamp = ((__int64) r2 << 32) | ((__int64) r1 << 16) | (__int64) (r0);
					prev_ts = ((__int64 )f->cur_ts_h << 32) | ((__int64) f->cur_ts_m << 16) | ((__int64) f->cur_ts_l);
					tstamp = (tstamp - prev_ts) / 10000; // Convert in 100 us units

					// And append it to the inter event time histogram
					HistoInt_AddEntry(&(f->h_inter_event_time), (unsigned int)tstamp);
				}

				// Save time stamp of the current event
				f->cur_ts_h = r2;
				f->cur_ts_m = r1;
				f->cur_ts_l = r0;
				

				// We have to remember that we got the first fragment for this event
				f->got_first_fragment = 1;
			}
			

			// Event Count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;

			// Event Count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;

			tmp = (((unsigned int) r1) << 16) | ((unsigned int) r0);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "Event_Count 0x%08x (%d)\n", tmp, tmp);
			}
		}
		else if ((*p & PFX_4_BIT_CONTENT_MASK) == PFX_END_OF_EVENT_MINOS)
		{
			tmp = ((unsigned int) GET_EOE_SIZE(*p)) << 16;
			i++;
			p++;
			sz_rd+=2;
			tmp = tmp + (unsigned int) *p;
			i++;
			p++;
			sz_rd += 2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EVBND))
			{
				fprintf((FILE *)fp, "----- End of Event ----- (size %d bytes)\n", tmp);
			}

			f->tot_fem_eoe++;

			// Put the event size in an histogram when running without event builder, i.e. only 1 Feminos
			if (f->is_eb_active == 0)
			{
				// Convert this size to KB with rounding and add this entry to the event size histogram
				HistoInt_AddEntry(&(f->h_event_size), (unsigned int)((tmp + 512) / 1024));
				f->sum_built_event_size += tmp;

				// Update the histogram of highest number of hit channel in an ASIC seen for this event
				HistoInt_AddEntry(&(f->h_highest_ch_hit), f->cur_highest_ch_hit_count);

				// Put the count of non-empty channels per ASIC in histogram 
				for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
				{
					for (j = 0; j<ASIC_PER_FEMINOS; j++)
					{
						if (f->cur_ne_ch[i][j])
						{
							HistoInt_AddEntry(&(f->h_glo_asic_ch_ne), f->cur_ne_ch[i][j]);
						}
					}
				}
			}
			else
			{
				// Increase the size of the current event by the size of this fragment
				f->cur_event_size += tmp;
			}

			// Append the accumulated event fragment size to its histogram
			// This will commit the size of the data for the last channel of the current event
			if (f->cur_cha_data_size != -1)
			{
				HistoInt_AddEntry(&(f->h_cha_data_size), f->cur_cha_data_size);
			}
			f->cur_cha_data_size = -1; // The size is not initialized for the next event
		}
		else if ((*p & PFX_4_BIT_CONTENT_MASK) == PFX_EXTD_CARD_CHIP_LAST_CELL_READ)
		{
			r0 = GET_EXTD_CARD_CHIP_LAST_CELL_READ(*p);
			i++;
			p++;
			sz_rd += 2;
			r1 = *p;
			i++;
			p++;
			sz_rd += 2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LAST_CELL_READ))
			{
				fprintf((FILE *)fp, "Chip %01d Last_Cell_Read %03d (0x%03x)\n",
					r0,
					GET_LAST_CELL_READ(r1),
					GET_LAST_CELL_READ(r1));
			}
		}

		// Is it a prefix for 2-bit content?
		else if ((*p & PFX_2_BIT_CONTENT_MASK) == PFX_CH_HIT_CNT_HISTO)
		{
			r0 = GET_CH_HIT_CNT_HISTO_CHIP_IX(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Channel Hit Count Histogram (ASIC %d)\n", r0);
			}

			p++;
			i++;
			sz_rd += 2;

			// null word
			p++;
			i++;
			sz_rd += 2;

			ui = (unsigned int *)p;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Min Bin  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Max Bin  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Bin Width: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Bin Count: %d\n", *ui);
			}
			r0 = *ui;
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Min Value: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Max Value: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				tmp_vui = *ui;
				fprintf((FILE *)fp, "Mean     : %.2f\n", ((float)tmp_vui) / 100.0);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				tmp_vui = *ui;
				fprintf((FILE *)fp, "Std Dev  : %.2f\n", ((float)tmp_vui) / 100.0);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Entries  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			// Get all bins
			for (j = 0; j<r0; j++)
			{
				if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
				{
					fprintf((FILE *)fp, "Bin(%2d) = %9d\n", j, *ui);
				}
				ui++;
				i += 2;
				sz_rd += 4;
			}

			// Save last value of pointer
			p = (unsigned short *)ui;
		}

		// Is it a prefix for 0-bit content?
		else if ((*p & PFX_0_BIT_CONTENT_MASK) == PFX_EXTD_CARD_CHIP_CHAN_HIT_IX)
		{
			i++;
			p++;
			sz_rd += 2;

			r0 = GET_EXTD_CARD_IX(*p);
			r1 = GET_EXTD_CHIP_IX(*p);
			r2 = GET_EXTD_CHAN_IX(*p);
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HIT_CH))
			{
				fprintf((FILE *)fp, "Card %02d Chip %02d Channel %02d\n", r0, r1, r2);
			}
			i++;
			p++;
			sz_rd += 2;
			si = 0;
		}

		else if ((*p & PFX_0_BIT_CONTENT_MASK) == PFX_END_OF_FRAME)
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_FRBND))
			{
				fprintf((FILE *)fp, "----- End of Frame -----\n");
			}
			i++;
			p++;
			sz_rd+=2;
		}
		else if (*p == PFX_NULL_CONTENT)
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_NULLW))
			{
				fprintf((FILE *)fp, "null word (2 bytes)\n");
			}
			i++;
			p++;
			sz_rd+=2;
		}
		else if ((*p == PFX_DEADTIME_HSTAT_BINS) || (*p == PFX_EVPERIOD_HSTAT_BINS))
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				if (*p == PFX_DEADTIME_HSTAT_BINS)
				{
					fprintf((FILE *)fp, "Dead-time Histogram\n");
				}
				else
				{
					fprintf((FILE *)fp, "Inter Event Time Histogram\n");
				}
			}

			p++;
			i++;
			sz_rd += 2;

			// null word
			p++;
			i++;
			sz_rd += 2;

			ui = (unsigned int *)p;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Min Bin  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Max Bin  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Bin Width: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Bin Count: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Min Value: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Max Value: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				tmp_vui = *ui;
				fprintf((FILE *)fp, "Mean     : %.2f\n", ((float)tmp_vui) / 100.0);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				tmp_vui = *ui;
				fprintf((FILE *)fp, "Std Dev  : %.2f\n", ((float)tmp_vui) / 100.0);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Entries  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			// Save last value of pointer
			p = (unsigned short *)ui;
		}
		else if (*p == PFX_PEDESTAL_HSTAT)
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "\nPedestal Histogram Statistics\n");
			}
			p++;
			i++;
			sz_rd += 2;

			ui = (unsigned int *)p;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Min Bin  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Max Bin  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Bin Width: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Bin Count: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Min Value: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Max Value: %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Mean     : %.2f\n", ((float)*ui) / 100.0);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Std Dev  : %.2f\n", ((float)*ui) / 100.0);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Entries  : %d\n", *ui);
			}
			ui++;
			i += 2;
			sz_rd += 4;

			// Save last value of pointer
			p = (unsigned short *)ui;
		}
		else if (*p == PFX_PEDESTAL_H_MD)
		{
			p++;
			i++;
			sz_rd += 2;

			ui = (unsigned int *)p;

			mean = (float)(((float)*ui) / 100.0);
			ui++;
			i += 2;
			sz_rd += 4;

			std_dev = (float)(((float)*ui) / 100.0);
			ui++;
			i += 2;
			sz_rd += 4;

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_STAT))
			{
				fprintf((FILE *)fp, "Mean/Std_dev : %.2f  %.2f\n", mean, std_dev);
			}

			// Save last value of pointer
			p = (unsigned short *)ui;
		}
		else if (*p == PFX_SHISTO_BINS)
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_BINS))
			{
				fprintf((FILE *)fp, "Threshold Turn-on curve\n");
			}
			i++;
			p++;
			sz_rd += 2;

			for (j = 0; j<16; j++)
			{
				if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_BINS))
				{
					fprintf((FILE *)fp, "%d ", *p);
				}
				i++;
				p++;
				sz_rd += 2;
			}
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_HISTO_BINS))
			{
				fprintf((FILE *)fp, "\n\n");
			}
		}
		else if (*p == PFX_CMD_STATISTICS)
		{
			// Skip header
			i++;
			p++;
			sz_rd+=2;

			// RX command count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// RX command count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[0] = (int) ((r1 << 16) | (r0));

			// RX daq count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// RX daq count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[1] = (int) ((r1 << 16) | (r0));

			// RX daq timeout lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// RX daq timeout upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[2] = (int) ((r1 << 16) | (r0));

			// RX daq delayed lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// RX daq delayed upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[3] = (int) ((r1 << 16) | (r0));

			// Missing daq requests lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// Missing daq requests upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[4] = (int) ((r1 << 16) | (r0));

			// RX command error count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// RX command error count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[5] = (int) ((r1 << 16) | (r0));

			// TX command reply count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// TX command reply count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[6] = (int) ((r1 << 16) | (r0));

			// TX DAQ reply count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// TX DAQ reply count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[7] = (int) ((r1 << 16) | (r0));

			// TX DAQ reply re-send count lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// TX DAQ reply re-send count upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[8] = (int) ((r1 << 16) | (r0));

			if (vflg & FRAME_PRINT_ALL)
			{
				fprintf((FILE *)fp, "Server RX stat: cmd_count=%d daq_req=%d daq_timeout=%d daq_delayed=%d daq_missing=%d cmd_errors=%d\n", tmp_i[0], tmp_i[1], tmp_i[2], tmp_i[3], tmp_i[4], tmp_i[5]);
				fprintf((FILE *)fp, "Server TX stat: cmd_replies=%d daq_replies=%d daq_replies_resent=%d\n", tmp_i[6], tmp_i[7], tmp_i[8]);
			}
		}
		else if (*p == PFX_START_OF_BUILT_EVENT)
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EBBND))
			{
				fprintf((FILE *)fp, "***** Start of Built Event *****\n");
			}
			i++;
			p++;
			sz_rd+=2;
			
			f->tot_sobe++;
			f->cur_event_size = 0;
			f->cur_highest_ch_hit_count = 0;
			if (f->is_eb_active == 0)
			{
				printf("Warning: Event Builder is presumably not active but Start of Built Event was found\n");
			}
			else
			{
				f->is_eb_active = 1;
			}
			f->got_first_fragment = 0;

			// Clear the number of non-empty channel for each ASIC
			for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
			{
				for (j = 0; j<ASIC_PER_FEMINOS; j++)
				{
					f->cur_ne_ch[i][j] = 0;
				}
			}
		}
		else if (*p == PFX_END_OF_BUILT_EVENT)
		{
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EBBND))
			{
				fprintf((FILE *)fp, "***** End of Built Event *****\n\n");
			}
			i++;
			p++;
			sz_rd+=2;

			f->tot_eobe++;

			// Convert event size to KB with rounding and add this entry to the event size histogram
			HistoInt_AddEntry(&(f->h_event_size), (unsigned int)((f->cur_event_size + 512) / 1024));
			f->sum_built_event_size += f->cur_event_size;

			// Update the histogram of highest number of hit channel in an ASIC seen for this event
			HistoInt_AddEntry(&(f->h_highest_ch_hit), f->cur_highest_ch_hit_count);

			// Get prepared for the next event for which we did not receive any fragment yet
			// This is also done when start of built event is found, so it is redundant
			f->got_first_fragment = 0;

			if (f->is_eb_active == 0)
			{
				printf("Warning: Event Builder is presumably not active but End of Built Event was found\n");
			}
			else
			{
				f->is_eb_active = 1;
			}

			// Put the count of non-empty channels per ASIC in histogram 
			for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
			{
				for (j = 0; j<ASIC_PER_FEMINOS; j++)
				{
					if (f->cur_ne_ch[i][j])
					{
						HistoInt_AddEntry(&(f->h_glo_asic_ch_ne), f->cur_ne_ch[i][j]);
					}
				}
			}
		}
		else if (*p == PFX_SOBE_SIZE)
		{
			// Skip header
			i++;
			p++;
			sz_rd+=2;

			// Built Event Size lower 16-bit
			r0 = *p;
			i++;
			p++;
			sz_rd+=2;
			// Built Event Size upper 16-bit
			r1 = *p;
			i++;
			p++;
			sz_rd+=2;
			tmp_i[0] = (int) ((r1 << 16) | (r0));

			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_EBBND))
			{
				fprintf((FILE *)fp, "***** Start of Built Event - Size = %d bytes *****\n", tmp_i[0]);
			}

			// Set the size of the current event
			f->cur_event_size = tmp_i[0];

			f->cur_highest_ch_hit_count = 0;
			f->tot_sobe_size++;
			f->got_first_fragment = 0;

			if (f->is_eb_active == 0)
			{
				printf("Warning: Event Builder is presumably not active but Start of Built Event with Size was found\n");
			}
			else
			{
				f->is_eb_active = 1;
			}

			// Clear the number of non-empty channel for each ASIC
			for (i = 0; i<MAX_NB_OF_FEMINOS; i++)
			{
				for (j = 0; j<ASIC_PER_FEMINOS; j++)
				{
					f->cur_ne_ch[i][j] = 0;
				}
			}
		}
		else if (*p == PFX_LONG_ASCII_MSG)
		{
			// Skip header
			i++;
			p++;
			sz_rd += 2;

			// Get long string size (16-bit)
			r0 = *p;
			i++;
			p++;
			sz_rd += 2;

			c = (char *)p;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_ASCII))
			{
				for (j = 0; j<r0; j++)
				{
					fprintf((FILE *)fp, "%c", *c);
					c++;
				}
			}

			// Skip the null string terminating character
			r0++;
			// But if the resulting size is odd, there is another null character that we should skip
			if (r0 & 0x0001)
			{
				r0++;
			}
			p += (r0 >> 1);
			i += (r0 >> 1);
			sz_rd += r0;
		}
		else if (*p == PFX_EXTD_PEDTHR_LIST)
		{
			// Skip header
			i++;
			p++;
			sz_rd += 2;

			r0 = GET_EXTD_PEDTHR_LIST_FEM(*p);
			r1 = GET_EXTD_PEDTHR_LIST_ASIC(*p);
			r2 = GET_EXTD_PEDTHR_LIST_MODE(*p);
			r3 = GET_EXTD_PEDTHR_LIST_TYPE(*p);
			i++;
			p++;
			sz_rd += 2;
			if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LISTS))
			{
				if (r3 == 0) // pedestal entry
				{
					fprintf((FILE *)fp, "# Pedestal List for FE %02d ASIC %01d\n", r0, r1);
				}
				else
				{
					fprintf((FILE *)fp, "# Threshold List for FE %02d ASIC %01d\n", r0, r1);
				}
				if (vflg & FRAME_PRINT_LISTS_FOR_ARC)
				{
					// nothing
				}
				else
				{
					fprintf((FILE *)fp, "be sel_fe %02d\n", r0);
				}
			}

			// Determine the number of entries
			if (r2 == 0) // AGET
			{
				r2 = 71;
			}
			else // AFTER
			{
				r2 = 78;
			}
			// Get all entries
			for (j = 0; j <= r2; j++)
			{
				tmp_i[0] = (int)* ((short *)p);
				if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LISTS))
				{
					if (r3 == 0) // pedestal entry
					{
						if (vflg & FRAME_PRINT_LISTS_FOR_ARC)
						{
							sprintf(tmp_str, "ped");
						}
						else
						{
							sprintf(tmp_str, "fe ped");
						}
					}
					else
					{
						if (vflg & FRAME_PRINT_LISTS_FOR_ARC)
						{
							sprintf(tmp_str, "thr");
						}
						else
						{
							sprintf(tmp_str, "fe thr");
						}
					}
					if ((vflg & FRAME_PRINT_ALL) || (vflg & FRAME_PRINT_LISTS))
					{
						fprintf((FILE *)fp, "%s %1d %2d 0x%04x (%4d)\n", tmp_str, r1, j, *p, tmp_i[0]);
					}
				}
				i++;
				p++;
				sz_rd += 2;
			}
		}
		// No interpretable data
		else
		{
			fprintf((FILE *)fp, "word(%04d) : 0x%x (%d) unknown data\n", i, *p, *p);
			sz_rd+=2;
			p++;
		}
		
		// Check if end of packet was reached
		if (sz_rd == fr_sz)
		{
			done = 1;
		}
		else if (sz_rd > fr_sz)
		{
			fprintf((FILE *)fp, "Format error: read %d bytes but packet size is %d\n", sz_rd, fr_sz);
			done = 1;
		}
	}

	// Put an empty line
	if (vflg & FRAME_PRINT_ALL)
	{
		fprintf((FILE *)fp, "\n");
	}
}

