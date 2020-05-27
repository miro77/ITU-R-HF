#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Local includes
#include "Common.h"
#include "P533.h"
#include "ITURHFProp.h"
// End local includes

// Local prototypes
// End local prototypes

void DumpPathData(struct PathData path, struct ITURHFProp ITURHFP) {

	/*
	 * DumpPathData() - This routine prints out the contents of the structure PathData path. Most of the 
	 *		significant calculated values will be output by this program. Typically this type of output is 
	 *		valuable for point-to-point analysis or debugging purposes. The text file generated by this routine 
	 *		would not be useful to run over an area since it would generate too much data to be reasonably 
	 *		examined. This routine is one of the ways to examine control points and mode data, both of which 
	 *		are dependent on the length of the path. There are no modes calculated for paths in excess of 9000 km
	 *		Additionally the control points that are used for paths over 9000 km are a combination of those that 
	 *		appear in Table (1a P.533-12 and those dictated by the long path method a.k.a FTZ(). 
	 *		This indicates that some of the calculated values are not reported by this routine, which is indeed the 
	 *		case. But the values that are printed here are significant enough to aid in diagnosing algorithmic 
	 *		problems if necessary. From a user point of view data printed from this routine may illuminate why 
	 *		values are calculated.
	 *
	 *		INPUT
	 *			struct PathData path
	 *			struct ITURHFProp ITURHFP
	 *
	 *		OUTPUT
	 *			Printed data to the file named
	 *				PDDddmmyy-hhnnss.txt - where the path data dump file is time stamped as follows:
	 *					dd = day
	 *					mm = month
	 *					yy = year
	 *					hh = hour		
	 *					nn = minute
	 *					ss = seconds
	 *
	 */
	
	char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}; 
	// Control point names
	char *cp[] = {"     T + 1000      ","     T + d0/2      ","      MidPath      ","     R - d0/2      ","    R - 1000       ",
		          "Nearest Transmitter","     T + dM/2      ","      MidPath      ","      R - dM/2     "," Nearest Receiver  "};
    // Control point titles                                                                         
	char *cpt[] = {"  Control Point  ","  Control Point  ","  Control Point  ","  Control Point  ","  Control Point  ",
		           "Penetration Point","  Control Point  ","  Control Point  ","  Control Point  ","Penetration Point"}; 
	char *season[] = {"Winter", "Equinox", "Summer"};
	char *SorL[] = {"Short", "Long"};

	char outstr[64] = "";
	int i;
	int tz;		// time zone
	int cpk;	// offset index for control point display

	double ltime;

	FILE *fp;

	// Alias the output file for readability
	fp = ITURHFP.rptfp;

	if((ITURHFP.ifrq == 0) && (ITURHFP.ihr == 0) && (ITURHFP.ilng == 0) && (ITURHFP.ilat == 0)) {
		PrintITUHeader(ITURHFP.rptfp, asctime(ITURHFP.time), ITURHFP.P533ver, ITURHFP.P533compt, path.P372ver, path.P372compt);		
	};

	// This is a utility program to print out the PathData structure.
	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "*      DumpPathData - p533() Path data structure         *\n");
	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "*                 Input Parameters                       *\n");
	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "\t%s\n", path.name);
	fprintf(fp, "\tYear = %d\n", path.year);
	fprintf(fp, "\tMonth = %s\n", months[path.month]); 
	fprintf(fp, "\tHour  = %d (hour UTC)\n", path.hour + 1);
	fprintf(fp, "\tSSN (R12) = %d\n", path.SSN);
	fprintf(fp, "\tTx power = % 5.3lf (dB(1kW))\n", path.txpower);
	fprintf(fp, "\tTx Location %s\n", path.txname);
	fprintf(fp, "\tTx latitude  = % 5.3lf (% 5.3lf) [% d %d %d]\n", path.L_tx.lat, path.L_tx.lat*R2D, degrees(path.L_tx.lat*R2D), minutes(path.L_tx.lat*R2D), seconds(path.L_tx.lat*R2D));
	fprintf(fp, "\tTx longitude = % 5.3lf (% 5.3lf) [% d %d %d]\n", path.L_tx.lng, path.L_tx.lng*R2D, degrees(path.L_tx.lng*R2D), minutes(path.L_tx.lng*R2D), seconds(path.L_tx.lng*R2D));
	fprintf(fp, "\tRx Location %s\n", path.rxname);
	fprintf(fp, "\tRx latitude  = % 5.3lf (% 5.3lf) [% d %d %d]\n", path.L_rx.lat, path.L_rx.lat*R2D, degrees(path.L_rx.lat*R2D), minutes(path.L_rx.lat*R2D), seconds(path.L_rx.lat*R2D));
	fprintf(fp, "\tRx longitude = % 5.3lf (% 5.3lf) [% d %d %d]\n", path.L_rx.lng, path.L_rx.lng*R2D, degrees(path.L_rx.lng*R2D), minutes(path.L_rx.lng*R2D), seconds(path.L_rx.lng*R2D));
	fprintf(fp, "\tlocal time Rx   = % 02d (hour UTC)\n", path.hour + 1 + (int)(path.L_rx.lng/(15.0*D2R)));
	fprintf(fp, "\tlocal time Tx   = % 02d (hour UTC)\n", path.hour + 1 + (int)(path.L_tx.lng/(15.0*D2R)));
	fprintf(fp, "\tFrequency = % 5.3lf (MHz)\n", path.frequency);
	fprintf(fp, "\tBandwidth = % 5.3lf (Hz)\n", path.BW);
	fprintf(fp, "\tShort or Long Path = %s\n", SorL[path.SorL]);
	
	if(path.Modulation == ANALOG) {
		strcpy(outstr, "ANALOG");
		fprintf(fp, "\tModulation = %s\n", outstr); 
	}
	else {
		strcpy(outstr, "DIGITAL");
		fprintf(fp, "\tModulation = %s\n", outstr); 
	};

	fprintf(fp, "\tRequired signal-to-noise ratio = % 5.3lf\n", path.SNRr);
	fprintf(fp, "\tRequired Reliability (%%) = % d\n", path.SNRXXp);
	fprintf(fp, "\tRequired signal-to-interference ratio = % 5.3lf\n", path.SIRr);		

	if(path.noiseP.ManMadeNoise == CITY) {
		strcpy(outstr, "CITY");
		fprintf(fp, "\tMan-made noise = %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == RESIDENTIAL) {
		strcpy(outstr, "RESIDENTIAL");
		fprintf(fp, "\tMan-made noise = %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == RURAL) {
		strcpy(outstr, "RURAL");
		fprintf(fp, "\tMan-made noise = %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == QUIETRURAL) {
		strcpy(outstr, "QUIETRURAL");
		fprintf(fp, "\tMan-made noise = %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == NOISY) {
		strcpy(outstr, "NOISY");
		fprintf(fp, "\tMan-made noise = %s\n", outstr);
	}
	else if(path.noiseP.ManMadeNoise == QUIET) {
		strcpy(outstr, "QUIET");
		fprintf(fp, "\tMan-made noise = %s\n", outstr);
	}
	else {
		fprintf(fp, "\tMan-made noise = % 5.3lf (dB)\n", path.noiseP.ManMadeNoise);
	}

	if(path.Modulation == DIGITAL) {
		fprintf(fp, "\tFrequency dispersion for simple BCR (F0) = % 5.3lf (Hz)\n", path.F0);			// Frequency dispersion at a level -10 dB relative to the peak signal amplitude
		fprintf(fp, "\tTime spread for simple BCR (T0)          = % 5.3lf (mS)\n", path.T0);
		fprintf(fp, "\tRequired Amplitude ratio (A)             = % 5.3lf (dB)\n", path.A);
		fprintf(fp, "\tTime window                              = % 5.3lf (mS)\n", path.TW);	 
		fprintf(fp, "\tFrequency window                         = % 5.3lf (Hz)\n", path.FW);			
	};

	if(ITURHFP.AntennaOrientation == TX2RX) {
		fprintf(fp, "\tAntenna configuration: Transmitter main beam to receiver main beam\n");
	}
	else if(ITURHFP.AntennaOrientation == MANUAL) {
		fprintf(fp, "\tAntenna configuration: User determined\n");
	}
	else {
		fprintf(fp, "\tAntenna configuration: UNKNOWN\n");
	};

	fprintf(fp, "\tTransmit antenna %.40s\n", path.A_tx.Name);
	fprintf(fp, "\tTransmit antenna bearing = %lf\n", ITURHFP.TXBearing*R2D);
	fprintf(fp, "\tTransmit antenna gain offset = %lf\n", ITURHFP.TXGOS);
	fprintf(fp, "\tReceive antenna  %.40s\n", path.A_rx.Name);
	fprintf(fp, "\tReceive antenna bearing = %lf\n", ITURHFP.RXBearing*R2D);
	fprintf(fp, "\tReceive antenna gain offset = %lf\n", ITURHFP.RXGOS);

	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "*                 Calculated Parameters                  *\n");
	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "****************** Distances (km) ************************\n");
	fprintf(fp, "\tdistance = % 5.3lf\n", path.distance);
	fprintf(fp, "\tslant range = % 5.3lf\n", path.ptick);
	fprintf(fp, "\tdmax     = % 5.3lf\n", path.dmax);
	fprintf(fp, "************ Maximum Usable Frequencies (MHz) ************\n");
	fprintf(fp, "\tbasic MUF       = % 5.3lf\n", path.BMUF);
	fprintf(fp, "\t10%% MUF         = % 5.3lf\n", path.MUF10);
	fprintf(fp, "\t50%% MUF         = % 5.3lf\n", path.MUF50);
	fprintf(fp, "\t90%% MUF         = % 5.3lf\n", path.MUF90);
	fprintf(fp, "\tOperational MUF = % 5.3lf\n", path.OPMUF);
	fprintf(fp, "\t10%% OPMUF       = % 5.3lf\n", path.OPMUF10);
	fprintf(fp, "\t90%% OPMUF       = % 5.3lf\n", path.OPMUF90);
	fprintf(fp, "********* Lowest Order and Dominant Mode *****************\n");
	if(path.n0_F2 != 99) 
		sprintf(outstr, "%2d", path.n0_F2+1);
	else 
		strcpy(outstr, "No Mode");
	fprintf(fp, "\tlowest order F2 layer mode = %s\n", outstr);
	if(path.n0_E != 99) 
		sprintf(outstr, "%2d", path.n0_E+1);
	else 
		strcpy(outstr, "No Mode");
	fprintf(fp, "\tlowest order E layer mode = %s\n", outstr);

	// Print the dominant mode
	if(path.DMidx < 3) {
		fprintf(fp, "\tDominant mode: E layer mode %d\n", path.DMidx+1);
	}
	else if((path.DMidx >= 3) && (path.DMidx <= 8)) {
			fprintf(fp, "\tDominant mode: F2 layer mode %d\n", path.DMidx-2);
	}
	else if(path.DMidx == NODOMINANTMODE) {
		fprintf(fp, "\tNo Dominant mode for this path length\n");
	};
	
	fprintf(fp, "************************ Season ***************************\n");
	if(path.CP[MP].L.lat >= 0) // The season is defined by the midpoint control point.
		fprintf(fp, "\tseason = %s\n", season[path.season]);
	else // Seasons are the correct order in southern hemisphere.
	fprintf(fp, "\tseason = %s\n", season[2 - path.season]);

	fprintf(fp, "****** Field Strength (dB(1 uV/m)) and Rx Power (dBW) *****\n");
	fprintf(fp, "\tField Strength (7000 km > D)           = % 5.3lf\n", path.Es);
	fprintf(fp, "\tField Strength (7000 km < D < 9000 km) = % 5.3lf\n", path.Ei);
	fprintf(fp, "\tField Strength           (D > 9000 km) = % 5.3lf\n", path.El);
	fprintf(fp, "\tMedian Rx power = % 5.3lf\n", path.Pr);
	fprintf(fp, "**************** Rx Elevation Angle (degs)****************\n");
	fprintf(fp, "\tRx Elevation angle = % 5.3lf\n", path.ele*R2D);

		
	// Print the short path parameters if the distance is less than 9000
	if(path.distance < 9000.0) {
		fprintf(fp, "***************** Short Path Parameters *******************\n");
		fprintf(fp, "\t\"Not otherwise included loss\" (dB) = % 5.3lf\n", path.Lz);
	};
	
	// Print the long path parameters if the distance is greater than 7000
	if(path.distance > 7000.0) {
		fprintf(fp, "***************** Long Path Parameters *******************\n");
		fprintf(fp, "\tFree-space Field Strength 3 MW e.i.r.p. (dB(1uV/m) = % 5.3lf\n", path.E0);
		fprintf(fp, "\tIncreased Long Distance Field Strength due to Focusing (dB) = % 5.3lf\n", path.Gap);
		fprintf(fp, "\t\"Not otherwise included loss\" (dB) = % 5.3lf\n", path.Ly);
		fprintf(fp, "\tUpper Reference Frequency (MHz)      = % 5.3lf\n", path.fM);
		fprintf(fp, "\tLower Reference Frequency (MHz)      = % 5.3lf\n", path.fL);
		fprintf(fp, "\tCorrection Factor at T + dM/2        = % 5.3lf\n", path.K[0]);
		fprintf(fp, "\tCorrection Factor at R - dM/2        = % 5.3lf\n", path.K[1]);
		fprintf(fp, "\tMax Antenna Gain G_tl (0 to 8 deg)   = % 5.3lf\n", path.Gtl);
		fprintf(fp, "\tMax Antenna Gain G_w (0 to 8 deg)    = % 5.3lf\n", path.Grw);
		fprintf(fp, "\tMean gyrofrequency (T + dM/2 & R - dM/2) = % 5.3lf\n", path.fH);
		fprintf(fp, "\tScale factor f(f ,fL, fM, fH)        = % 5.3lf\n", path.F);
	};

	// Noise
	fprintf(fp, "***************** Noise Parameters (dB) *******************\n");
	fprintf(fp, "\tAtmospheric noise upper decile = % 5.3lf\n", path.noiseP.DuA); 
	fprintf(fp, "\tAtmospheric noise lower decile = % 5.3lf\n", path.noiseP.DlA);  // 
	fprintf(fp, "\tAtmospheric noise              = % 5.3lf\n", path.noiseP.FaA);	 // 
	fprintf(fp, "\tMan made noise upper decile    = % 5.3lf\n", path.noiseP.DuM);  
	fprintf(fp, "\tMan made noise lower decile    = % 5.3lf\n", path.noiseP.DlM); 
	fprintf(fp, "\tMan made noise                 = % 5.3lf\n", path.noiseP.FaM); 
	fprintf(fp, "\tGalactic noise upper decile    = % 5.3lf\n", path.noiseP.DuG); 
	fprintf(fp, "\tGalactic noise lower decile    = % 5.3lf\n", path.noiseP.DlG); 
	fprintf(fp, "\tGalactic noise                 = % 5.3lf\n", path.noiseP.FaG);
	fprintf(fp, "\tTotal noise upper decile       = % 5.3lf\n", path.noiseP.DuT); 
	fprintf(fp, "\tTotal noise lower decile       = % 5.3lf\n", path.noiseP.DlT); 
	fprintf(fp, "\tTotal noise                    = % 5.3lf\n", path.noiseP.FamT);

	fprintf(fp, "********************** SNR Parameters (dB) *************************************\n");
	fprintf(fp, "\tMonthly median resultant signal-to-noise ratio = % 5.3lf\n", path.SNR);	
	fprintf(fp, "\tUpper decile deviation signal-to-noise ratio   = % 5.3lf\n", path.DuSN);
	fprintf(fp, "\tLower decile deviation signal-to-noise ratio   = % 5.3lf\n", path.DlSN); 
	fprintf(fp, "\tSignal-to-noise exceeded for %d%% of the month  = % 5.3lf\n", path.SNRXXp, path.SNRXX);

	fprintf(fp, "********************** SIR Parameters (dB) *************************************\n");
	fprintf(fp, "\tSignal-to-interference ratio = % 5.3lf\n", path.SIR);
	fprintf(fp, "\tUpper decile deviation of the signal-to-interference ratio = % 5.3lf\n", path.DuSI);
	fprintf(fp, "\tLower decile deviation of the signal-to-interference ratio = % 5.3lf\n", path.DlSI);

	fprintf(fp, "******************** Reliability Parameters (%%) *******************************\n");
	fprintf(fp, "\tBasic Circuit Reliability                      = % 5.3lf\n", path.BCR);  // 
	fprintf(fp, "\tMultimode Interference                         = % 5.3lf\n", path.MIR);  //
	fprintf(fp, "\tOverall Circuit Reliability without scattering = % 5.3lf\n", path.OCR);  // 
	fprintf(fp, "\tOverall Circuit Reliability with scattering    = % 5.3lf\n", path.OCRs); // 
	fprintf(fp, "\tProbability of scattering                      = % 5.3lf\n", path.probocc);
	fprintf(fp, "\tProbability required SNR is achieved                      = % 5.3lf\n", path.RSN); 
	fprintf(fp, "\tProbability required time spread T0 is not exceeded       = % 5.3lf\n", path.RT);	 
	fprintf(fp, "\tProbability required frequency spread f0 is not exceeded  = % 5.3lf\n", path.RF);

	if(path.distance < 9000) { // Don't print the modes out if they are not calculated
		for(i=0; i<MAXF2MDS; i++) {
			fprintf(fp, "**********************************************************\n");
			fprintf(fp, "*                      F2 Mode %d                         *\n", i+1);
			fprintf(fp, "**********************************************************\n");
			fprintf(fp, "\tbasic MUF  = % 5.3lf (MHz)\n", path.Md_F2[i].BMUF);
			fprintf(fp, "\t10%% MUF    = % 5.3lf (MHz)\n",path.Md_F2[i].MUF10);
			fprintf(fp, "\t50%% MUF    = % 5.3lf (MHz)\n",path.Md_F2[i].MUF50); 
			fprintf(fp, "\t90%% MUF    = % 5.3lf (MHz)\n",path.Md_F2[i].MUF90); 
			fprintf(fp, "\tOPMUF      = % 5.3lf (MHz)\n",path.Md_F2[i].OPMUF); 
			fprintf(fp, "\t10%% OPMUF  = % 5.3lf (MHz)\n",path.Md_F2[i].OPMUF10); 
			fprintf(fp, "\t90%% OPMUF  = % 5.3lf (MHz)\n",path.Md_F2[i].OPMUF90); 
			fprintf(fp, "\tFprob        = % 5.3lf (%%)\n",path.Md_F2[i].Fprob); 
			fprintf(fp, "\tLower decile = % 5.3lf\n",path.Md_F2[i].deltal);
			fprintf(fp, "\tUpper decile = % 5.3lf\n",path.Md_F2[i].deltau); 
			fprintf(fp, "\tE Layer Screen Frequency  = % 5.3lf (MHz)\n", path.Md_F2[i].fs);
			fprintf(fp, "\tBasic Loss (< 7000 km)    = % 5.3lf (dB)\n", path.Md_F2[i].Lb);
			fprintf(fp, "\tMedian Field Strength     = % 5.3lf (dB(1 uV/m))\n", path.Md_F2[i].Ew);
			fprintf(fp, "\tReceiver Power            = % 5.3lf (dBW)\n", path.Md_F2[i].Prw);
			fprintf(fp, "\tDelay                     = % 5.3lf (mS)\n", path.Md_F2[i].tau*1000.0);
			fprintf(fp, "\tElevation angle   = % 5.3lf (degs)\n", path.Md_F2[i].ele*R2D);
			fprintf(fp, "\tReflection height = % 5.3lf (km)\n", path.Md_F2[i].hr);
			fprintf(fp, "\tReceiver Gain = % 5.3lf (dBi)\n", path.Md_F2[i].Grw);
		};

			for(i=0; i<MAXEMDS; i++) {
			fprintf(fp, "**********************************************************\n");
			fprintf(fp, "*                       E Mode %d                         *\n", i+1);
			fprintf(fp, "**********************************************************\n");
			fprintf(fp, "\tbasic MUF  = % 5.3lf (MHz)\n", path.Md_E[i].BMUF);
			fprintf(fp, "\t10%% MUF    = % 5.3lf (MHz)\n",path.Md_E[i].MUF10);
			fprintf(fp, "\t50%% MUF    = % 5.3lf (MHz)\n",path.Md_E[i].MUF50); 
			fprintf(fp, "\t90%% MUF    = % 5.3lf (MHz)\n",path.Md_E[i].MUF90);  
			fprintf(fp, "\tOPMUF      = % 5.3lf (MHz)\n",path.Md_E[i].OPMUF); 
			fprintf(fp, "\t10%% OPMUF  = % 5.3lf (MHz)\n",path.Md_E[i].OPMUF10); 
			fprintf(fp, "\t90%% OPMUF  = % 5.3lf (MHz)\n",path.Md_E[i].OPMUF90); 
			fprintf(fp, "\tFprob        = % 5.3lf (%%)\n",path.Md_E[i].Fprob); 
			fprintf(fp, "\tLower decile = % 5.3lf\n",path.Md_E[i].deltal);
			fprintf(fp, "\tUpper decile = % 5.3lf\n",path.Md_E[i].deltau);  
			fprintf(fp, "\tBasic Loss (< 7000 km)    = % 5.3lf (dB)\n", path.Md_E[i].Lb);
			fprintf(fp, "\tMedian Field Strength     = % 5.3lf (dB(1 uV/m))\n", path.Md_E[i].Ew);
			fprintf(fp, "\tReceiver Power            = % 5.3lf (dBW)\n", path.Md_E[i].Prw);
			fprintf(fp, "\tDelay                     = % 5.3lf (mS)\n", path.Md_E[i].tau*1000.0);
			fprintf(fp, "\tElevation angle   = % 5.3lf (degs)\n", path.Md_E[i].ele*R2D);
			fprintf(fp, "\tReflection height = % 5.3lf (km)\n", path.Md_E[i].hr);
			fprintf(fp, "\tReceiver Gain = % 5.3lf (dBi)\n", path.Md_E[i].Grw);
		};
	};
	
	// *************************** Print out the Control Points ********************************
	// Set the offset for the display of the control point title
	cpk = 0;
	if(path.distance > 9000) {
		cpk = 5;
	}
	for(i=0; i<5; i++) {
		fprintf(fp, "**********************************************************\n");
		fprintf(fp, "*          %s - %s       *\n", cpt[i+cpk], cp[i+cpk]);
		fprintf(fp, "**********************************************************\n");
		fprintf(fp, "\tLatitude\t=\t% 5.3lf\t(% 5.3lf)\t[%d %d %d]\n", path.CP[i].L.lat, path.CP[i].L.lat*R2D, degrees(path.CP[i].L.lat*R2D), minutes(path.CP[i].L.lat*R2D), seconds(path.CP[i].L.lat*R2D));
		fprintf(fp, "\tLongitude\t=\t% 5.3lf\t(% 5.3lf)\t[%d %d %d]\n", path.CP[i].L.lng, path.CP[i].L.lng*R2D, degrees(path.CP[i].L.lng*R2D), minutes(path.CP[i].L.lng*R2D), seconds(path.CP[i].L.lng*R2D));
		fprintf(fp, "\tdistance = % 5.3lf\n", path.CP[i].distance);
		fprintf(fp, "\tMagnetic dip (100 km)  = % 5.3lf (deg)\n", path.CP[i].dip[HR100km]*R2D); 
		fprintf(fp, "\tGyrofrequency (100 km) = % 5.3lf (MHz)\n", path.CP[i].fH[HR100km]);
		fprintf(fp, "\tMagnetic dip (300 km)  = % 5.3lf (deg)\n", path.CP[i].dip[HR300km]*R2D); 
		fprintf(fp, "\tGyrofrequency (300 km) = % 5.3lf (MHz)\n", path.CP[i].fH[HR300km]); 
		fprintf(fp, "\tM(3000)F2 = % 5.3lf\n", path.CP[i].M3kF2);
		fprintf(fp, "\tfoE   = % 5.3lf (MHz)\n", path.CP[i].foE);
		fprintf(fp, "\tfoF2  = % 5.3lf (MHz)\n", path.CP[i].foF2);
		// The reflection height is calculated at the mid path point so print it
		if(i == MP) fprintf(fp, "\treflection height  = % 5.3lf (km)\n", path.CP[i].hr);
		fprintf(fp, "\tsolar zenith angle = % 5.3lf (deg)\n", path.CP[i].Sun.sza*R2D); 
		fprintf(fp, "\tsolar declination  = % 5.3lf (deg)\n", path.CP[i].Sun.decl*R2D); 
		fprintf(fp, "\tsolar hour angle   = % 5.3lf (deg)\n", path.CP[i].Sun.ha*R2D); 
		fprintf(fp, "\tequation of time   = % 5.3lf (minutes)\n", path.CP[i].Sun.eot); 
		// Determine the tz of the control point
		tz = (int)(path.CP[i].L.lng/(15.0*D2R));
		ltime = fmod(path.CP[i].Sun.lsr+tz, 24);
		fprintf(fp, "\tlocal sunrise      = % 02d:%02d (UTC) % 02d:%02d (Local)\n", hrs(path.CP[i].Sun.lsr), mns(path.CP[i].Sun.lsr), hrs(ltime), mns(ltime)); 
		ltime = fmod(path.CP[i].Sun.lsn+tz, 24);
		fprintf(fp, "\tlocal solar noon   = % 02d:%02d (UTC) % 02d:%02d (Local)\n", hrs(path.CP[i].Sun.lsn), mns(path.CP[i].Sun.lsn), hrs(ltime), mns(ltime)); 
		ltime = fmod(path.CP[i].Sun.lss+tz, 24);
		fprintf(fp, "\tlocal sunset       = % 02d:%02d (UTC) % 02d:%02d (Local)\n", hrs(path.CP[i].Sun.lss), mns(path.CP[i].Sun.lss), hrs(ltime), mns(ltime)); 
		ltime = fmod(path.CP[i].ltime+tz, 24);
		fprintf(fp, "\tlocal time         = % 02d:%02d (UTC) % 02d:%02d (Local)\n", hrs(path.CP[i].ltime), mns(path.CP[i].ltime), hrs(ltime), mns(ltime));
	};

	fprintf(fp, "**********************************************************\n");
	fprintf(fp, "*               End DumpPathData()                       *\n");
	fprintf(fp, "**********************************************************\n");

	return;

};

int degrees(double coord) { // Returns the degrees of coordinates
	return (int)coord;
};
int minutes(double coord) { // Returns the minutes of coordinates
	return abs((int)((coord - (int)coord)*60.0));
};
int seconds(double coord) { // Returns the seconds of coordinates
	return abs((int)((((coord - (int)coord)*60.0) - (int)((coord - (int)coord)*60.0))*60.0));
};
int hrs(double time) {
	return (int)time % 24;
};
int mns(double time) {
	return abs((int)((time - (int)time)*60.0));
};

void PrintITUHeader(FILE *fp, char * time, char const * P533Version, const char * P533CompT, char const * P372Version, const char * P372CompT) {

	fprintf(fp, "---------------------------------------------------------------------------\n");
	fprintf(fp, " International Telecommunications Union - Radiocommunication Sector (ITU-R)\n");
	fprintf(fp, "     ITURHFProp         Ver %s\n", ITURHFPropVER);
	fprintf(fp, "     HF Model (P533)    Ver %s\n", P533Version);
	// If the output is going to stderr then P533() has not run yet so P372.DLL has not loaded
	// so we do not know the P372.DLL version or compile time yet. If the P372Version 
	// null than don't print it out
	if (strcmp(P372Version, "")) {
		fprintf(fp, "     Noise Model (P372) Ver %s\n", P372Version);
	};
	fprintf(fp, "     Analysis Prepared  %s\n", time);
	fprintf(fp, "---------------------------------------------------------------------------\n");
	fprintf(fp, "\n");
};

void PrintITUTail(FILE *fp) {

	fprintf(fp, "Copyright  International Telecommunication Union (ITU) 2019\n");
	fprintf(fp, "All rights reserved.\n");

};

