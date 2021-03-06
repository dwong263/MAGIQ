#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//revised by jd, Oct. 21, 2009
//#include <iostream.h>
#include <iostream>
//end of revised code

//#include <variable.h>
#include "variable.h"

#include "fit.h"
#include <math.h>

//added by jd, Oct. 21, 2009
using namespace std;
//end of added code


//**************************************
//********** MAINLINE BEGINS ***********
//**************************************
 int main(int argc, char *argv[]){


//***** Flags *****
int		binary_format=0;			// flag for data format

//***** Strings ******
char	filename_data[255],				// filename for data file			(input)
		filename_constraints[255],		// filename for constraints file	(input)
		filename_guess[255],			// filename for intial guess file	(input)
		filename_guess_out[255],		// filename for final guess file	(output)
		filename_variables[255],		// filename for variables			(optional)
		filename_covariance[255]={0};	// filename for covariance matrix	(optional)


//***** Structures *****
NMR_Header	header_data;
Fit_info	fit_info=	{0,							// number peaks
						"PPM",						// shift units constraints
						"PPM",						// shift units guess
						"PPM",						// output shift units
						DEFAULT_TOLERENCE,			// tolerence
						FALSE,						// force positive amplitudes
						DEFAULT_MAXIMUM_ITERATIONS,	// maximum iterations
						DEFAULT_MINIMUM_ITERATIONS,	// minimum iterations
						DEFAULT_FIRST_POINT,		// first point to fit
						DEFAULT_LAST_POINT,			// last point to fit
						0,							// chi squared obtained from fit(..)
						{0,0},						// noise standard deviation
						"TIME_DOMAIN",
						DEFAULT_FILTER,
						0,							// first point fr range
						0,							// last point fr range
						DEFAULT_ALAMBDA_INC,		// marqu. parameter Alambda increment 
						DEFAULT_ALAMBDA_DEC,		// marqu. parameter Alambda decrement
						DEFAULT_SPIKE,
						"",							// spike removal threshold 
						DEFAULT_NOISE_POINTS,
						FALSE,
						0,
						0,
						NULL,
						DEFAULT_DWELL_TIME,
						DEFAULT_ZERO_FILL_TO,
						DEFAULT_FIXED_NOISE,
						DEFAULT_QRT_SIN_FIRST_POINT,
						DEFAULT_QRT_SIN_END_POINT,
						0,
						0,							// Number peaks guess file
						DEFAULT_MAX_CON_ALAMBDA_INC,
						"GLOBAL",                   // Output file type (other option is UNIX)
						NULL,						// Pointer to fit parameters structure constraints
						NULL,						// Pointer to fit parameters structure guess
						DEFAULT_GAUSSIAN_FILTER				// Default gaussian filter
						};				

// Initalize Fit Parameters

Fit_Parameter fit_parameter_constraints[NUMBER_FIT_PARAMETERS_CONSTRAINTS] = 
			{"NUMBER_PEAKS"				,1,{'I',0},{(double *)&(fit_info.number_peaks), NULL, NULL, NULL, NULL},
			 "SHIFT_UNITS"				,1,{'S',0},{(double *)&(fit_info.shift_units_constraints), NULL, NULL, NULL, NULL},
			 "OUTPUT_SHIFT_UNITS"		,1,{'S',0},{(double *)&(fit_info.output_shift_units), NULL, NULL, NULL, NULL},
			 "NOISE_POINTS"				,1,{'I',0},{(double *)&(fit_info.noise_points), NULL, NULL, NULL, NULL},
			 "FIXED_NOISE"				,1,{'D',0},{(double *)&(fit_info.fixed_noise), NULL, NULL, NULL, NULL},
			 "ALAMBDA_INC"				,1,{'D',0},{(double *)&(fit_info.alambda_inc), NULL, NULL, NULL, NULL},
			 "ALAMBDA_DEC"				,1,{'D',0},{(double *)&(fit_info.alambda_dec), NULL, NULL, NULL, NULL},
			 "ZERO_FILL"				,0,{'M',0},{(double *)&(fit_info.zero_fill), NULL, NULL, NULL, NULL},
			 "SPIKE"					,0,{'M',0},{(double *)&(fit_info.spike), NULL, NULL, NULL, NULL},
			 "FWHM_EXP_WEIGHTING"		,1,{'D',0},{(double *)&(fit_info.filter), NULL, NULL, NULL, NULL},
			 "TOLERENCE"				,1,{'D',0},{(double *)&(fit_info.tolerence), NULL, NULL, NULL, NULL},
			 "MAXIMUM_ITERATIONS"		,1,{'I',0},{(double *)&(fit_info.maximum_iterations), NULL, NULL, NULL, NULL},
			 "MINIMUM_ITERATIONS"		,1,{'I',0},{(double *)&(fit_info.minimum_iterations), NULL, NULL, NULL, NULL},
			 "POSITIVE_AMPLITUDES"		,0,{'M',0},{(double *)&(fit_info.positive_amplitudes), NULL, NULL, NULL, NULL},
			 "NOISE_EQUAL"				,0,{'M',0},{(double *)&(fit_info.noise_equal), NULL, NULL, NULL, NULL},
			 "RANGE"					,2,{'I',0},{(double *)&(fit_info.first_point),			(double *)&(fit_info.last_point), NULL, NULL, NULL},
			 "QRT_SIN_WEIGHTING_RANGE"	,2,{'I',0},{(double *)&(fit_info.qrt_sin_first_point),	(double *)&(fit_info.qrt_sin_last_point), NULL, NULL, NULL},
			 "FREQUENCY_RANGE"			,2,{'D',0},{(double *)&(fit_info.first_point_frequency),(double *)&(fit_info.last_point_frequency), NULL, NULL, NULL},
			 "FIX_ALL_SHIFT"			,0,{'M',SHIFT},			{(double *)&(fit_info.fix_all), NULL, NULL, NULL, NULL},
			 "FIX_ALL_DELAY_TIME"		,0,{'M',DELAY_TIME},	{(double *)&(fit_info.fix_all), NULL, NULL, NULL, NULL},
			 "FIX_ALL_L_WIDTH"			,0,{'M',WIDTH_LORENZ},	{(double *)&(fit_info.fix_all), NULL, NULL, NULL, NULL},
			 "FIX_ALL_G_WIDTH"			,0,{'M',WIDTH_GAUSS},	{(double *)&(fit_info.fix_all), NULL, NULL, NULL, NULL},
			 "FIX_ALL_PHASE"			,0,{'M',PHASE},			{(double *)&(fit_info.fix_all), NULL, NULL, NULL, NULL},
			 "FIX_ALL_AMPLITUDE"		,0,{'M',AMPLITUDE},		{(double *)&(fit_info.fix_all), NULL, NULL, NULL, NULL},
			 "DOMAIN"					,1,{'S',0},{(double *)&(fit_info.domain), NULL, NULL, NULL, NULL},
			 "OUTPUT_FILE_SYSTEM"		,1,{'S',0},{(double *)&(fit_info.output_file_system), NULL, NULL, NULL, NULL},
			 "FWHM_GAUSS_WEIGHTING"		,1,{'D',0},{(double *)&(fit_info.gaussian_filter), NULL, NULL, NULL, NULL},
};




Fit_Parameter fit_parameter_guess[NUMBER_FIT_PARAMETERS_GUESS] = 
			{"NUMBER_PEAKS"				,1,{'I',0},{(double *)&(fit_info.number_peaks_guess), NULL, NULL, NULL, NULL},
			 "SHIFT_UNITS"				,1,{'S',0},{(double *)&(fit_info.shift_units_guess), NULL, NULL, NULL, NULL}};






Point		*data=NULL;			// array structure for FID data
Peak2		*peak=NULL;			// array struct for peak parameter data
Complex		*sig=NULL;			// array structure for FID error data
Complex		*weight=NULL;       // array structure for window function
double		**covariance=NULL;	// matrix (#parameters x #parameters) passed to fit(..)

Error		error;			// error return structure

int		i,j,k,l,original_linked;
int		fit_completed=TRUE;
double	master_value, freq_sf, end_time;
int		number_points, sig_max;
int		zero_fill_to;


int		maxi;
double	maxval=0.0;
int		fid_start=1;
int		index;
//Complex sum, mean;


// Initialize fit_parameter pointer in the fit_info structure


	fit_info.fit_parameter_con		= (Fit_Parameter *)&fit_parameter_constraints;
	fit_info.fit_parameter_guess	= (Fit_Parameter *)&fit_parameter_guess;

// Display argument help if improper # of arguments

	printf("\nFitman Compiled: %s %s", __DATE__, __TIME__);
	printf("\nCopyright    John J. Potwarka and Robert Bartha");



	if(argc < 3){
		printf("\nusage: fitman <data file> <guess file> <constraints file> <guess out> [parameters]");
		printf("\n       Date: %s", __DATE__);
		printf("\n\n  Parameters\n  ----------");
		printf("\n-b                      - input data file is NMR286 binary format");
		printf("\n-t                      - input data file is NMR286 text format");
		printf("\n-fr                     - perform fit in frequency domain");
		printf("\n-ti                     - perform fit in time domain");
		printf("\n-t1                     - perform fit of T1 data");
		printf("\n-t2                     - perform fit of T2 data");
		printf("\n-z <n>                  - zero fill to point <n> (Frequency fitting only)");
		printf("\n-ew <n>                 - apply <n> Hz FWHM exponential weighting");
		printf("\n-gw <n>                 - apply <n> Hz FWHM Gaussian weighting");
		printf("\n-v <label> <n>          - set variable");
		printf("\n-vf <filename>          - read variable file");
		printf("\n-fp <paramter> [args]   - set or override fitting parameter");
		printf("\n-c <filename>           - output file for covariance matrix\n\n");
		exit(1);
	}

// Get filenames from command line
	strcpy(filename_data, argv[1]);
	strcpy(filename_guess, argv[2]);
	strcpy(filename_constraints, argv[3]);
	strcpy(filename_guess_out, argv[4]);
	strcpy(fit_info.filename_constraints, filename_constraints);

// Declare variable object	
	CVariable variable;

// Start parsing command line arguments starting at 5th argument
	i=5; 
	while(i<argc){
		if(strcmp(argv[i], "-b")==0)		// Binary file format
			binary_format = TRUE;
						
		
		if(strcmp(argv[i], "-t")==0)		// Text file format
			binary_format = FALSE;

		if(strcmp(argv[i], "-ti")==0)		// Time domain fit
			strcpy(fit_info.domain, "TIME_DOMAIN");

		if(strcmp(argv[i], "-fr")==0)		// Frequency domain fit
			strcpy(fit_info.domain, "FREQUENCY_DOMAIN");

		if(strcmp(argv[i], "-t1")==0)		// T1 fit
			strcpy(fit_info.domain, "T1_DOMAIN");

		if(strcmp(argv[i], "-z")==0)		// Zero Fill
			fit_info.zero_fill = (int)(atoi(argv[++i]));

		if(strcmp(argv[i], "-t2")==0)		// T2 fit
			strcpy(fit_info.domain, "T2_DOMAIN");
		
		if(strcmp(argv[i], "-vf")==0){		// Get variable filename
			strcpy(filename_variables, argv[++i]);
		 	error = read_guess_file(filename_variables, &peak, &fit_info, &variable);
			if(error.code){
				error_message("Error VARIABLE File:\n", WARNING);
			}

		}

		if(strcmp(argv[i], "-c")==0){		// Get variable filename
			strcpy(filename_covariance, argv[++i]);


		}

		if(strcmp(argv[i], "-ew")==0){		// Filter
			fit_info.filter = double(atof(argv[++i]));
			continue;
		}


		if(strcmp(argv[i], "-gw")==0){		// Filter
			fit_info.gaussian_filter = double(atof(argv[++i]));
			continue;
		}

		if(strcmp(argv[i], "-v")==0){		// Variable name and value passed
			i++;
			while( i<argc && argv[i][0] != '-'){
				variable.put_variable(string_to_upper(argv[i]), atof(argv[i+1]));
				i+=2;
			}
			continue;
		}
		i++;
	}

//*****************************
//***** Read Data file ********
//*****************************
	if(binary_format){
		if(!read_nmr_binary(filename_data, &data, &header_data))
			error_message("Error reading Binary data file\n", FATAL);
	}	
	else{ // else Text format
		if(!read_nmr_text(filename_data, &data, &header_data))
			error_message("Error reading Text data file\n", FATAL);

	}

	number_points = header_data.si/2;
	fit_info.dwell_time = header_data.dw;
	zero_fill_to = 0;

/******************************/
/* Image Data X initialization*/
/******************************/
	// if Image data then set up axis

	if(!strcmp(fit_info.domain, "T1_DOMAIN") || !strcmp(fit_info.domain, "T2_DOMAIN")){
		for(i=0;i<number_points;i++)
		   data[i].x = ((double)i-(double)number_points/2.0)*header_data.dw;
	}

/**********************************/
/* Initialize Internal variables  */
/**********************************/
	variable.put_variable("REFERENCE_FREQUENCY", header_data.sf);
	variable.put_variable("DWELL_TIME", header_data.dw);
	variable.put_variable("NUMBER_POINTS", (double)header_data.si/2);
	variable.put_variable("PPM", 0.0);
	variable.put_variable("HERTZ", 0.0);
	// initialize default fit range
	fit_info.last_point = header_data.si/2;


//******************************
//*** Read Constraints file ****
//******************************
	error = read_constraints_file(filename_constraints, &peak, &fit_info, &variable);
	
// If error condition returned
	if(error.code){
		error_message("Error Constraints File:", WARNING);
		error_trap(error, fit_info);  // report error end exit
	}


//***************************************************************************
//**** Reparse command line args to override constraint's fit parameters ****
//***************************************************************************
	i=5; 
	while(i<argc){
		if(strcmp(argv[i++], "-fp")==0){		// Fit Paramter setting
		//******* Read Fit Parameters *********
			index=-1;
			string_to_upper(argv[i]);
			while(index < NUMBER_FIT_PARAMETERS_CONSTRAINTS && strcmp(argv[i],fit_info.fit_parameter_con[++index].label));

			// If Parameter Label Found update fit info data
				if(index < NUMBER_FIT_PARAMETERS_CONSTRAINTS){
					if(fit_info.fit_parameter_con[index].argument_type[0] == 'M'){
						*(fit_info.fit_parameter_con[index].pointer.i[0]) |= 1 << (int)fit_info.fit_parameter_con[index].argument_type[1];
						i++;
					}
					else{
						for(j=0;j<fit_info.fit_parameter_con[index].number_arguments;j++){
							string_to_upper(argv[++i]);
							switch (fit_info.fit_parameter_con[index].argument_type[0]){
								case 'I' :	parse_item(argv[i], &variable, *(fit_info.fit_parameter_con[index].pointer.i[j]));
											break;
								case 'S' :	strcpy(fit_info.fit_parameter_con[index].pointer.s[j], argv[i]);
											break;
								case 'D' :	parse_item(argv[i], &variable, *(fit_info.fit_parameter_con[index].pointer.d[j]));
											break;
						}
					}
				}
			}
		}
	}


	if(fit_info.zero_fill)
		zero_fill_to = (int)pow(2.0,ceil(log(fit_info.zero_fill-fit_info.first_point+1)/log(2.0)));
	else
		zero_fill_to = (int)pow(2.0,ceil(log(fit_info.last_point-fit_info.first_point+1)/log(2.0)));
	
	if (zero_fill_to >= fit_info.last_point){
		freq_sf = 1.0/(double)sqrt(zero_fill_to);
	}else{
		error_message("Zero_fill incorrect..set to last_point:", WARNING);
		zero_fill_to = fit_info.last_point;
		freq_sf = 1.0/(double)sqrt(zero_fill_to);
	}

//******************************
//***** Read Guess file ********
//******************************
 	error = read_guess_file(filename_guess, &peak, &fit_info, &variable);
	
// If error condition returned
	if(error.code){
		error_message("Error Guess File:", WARNING);
		error_trap(error, fit_info);  // report error end exit
	}

//***************************************
//***** Determine Scale Function ********
//***************************************

// Initialize the weighting function for time domain fitting
	weight = new Complex[zero_fill_to];

	for(i=0; i<zero_fill_to; i++){
		weight[i].real = 1.0;
		weight[i].imag = 1.0;
	}

	if(fit_info.qrt_sin_last_point != 0 && !strcmp(fit_info.domain, "TIME_DOMAIN")){
		if (fit_info.qrt_sin_last_point <= fit_info.qrt_sin_first_point){
			error_message("\nFATAL ERROR Quarter sine weighting: \nLast point must be greater than first point",FATAL);
		}
		
		if (fit_info.qrt_sin_first_point < 1){
			error_message("\nFATAL ERROR Quarter sine weighting: \nFirst point must be greater than 0",FATAL);
		}

		end_time = (fit_info.qrt_sin_last_point-fit_info.qrt_sin_first_point)*
					fit_info.dwell_time;

		for(i=0; i<fit_info.qrt_sin_last_point; i++){
			if(i < fit_info.qrt_sin_first_point-1){
				weight[i].real = 0;
				weight[i].imag = 0;
			}else{
				weight[i].real = sin((2*PI*fit_info.dwell_time*i)/(4*end_time))
									* sin((2*PI*fit_info.dwell_time*i)/(4*end_time));
				weight[i].imag = sin((2*PI*fit_info.dwell_time*i)/(4*end_time))
									* sin((2*PI*fit_info.dwell_time*i)/(4*end_time));
			}
		}
	}

//***************************
//***** Initialize sig ******
//***************************
	if(zero_fill_to > fit_info.last_point && !strcmp(fit_info.domain, "FREQUENCY_DOMAIN")){
		sig = new Complex[zero_fill_to];
		sig_max = zero_fill_to;
	}else{
		sig = new Complex[fit_info.last_point];
		sig_max = fit_info.last_point;
	}

	if(fit_info.fixed_noise){
		fit_info.noise_std.real = fit_info.fixed_noise;
		fit_info.noise_std.imag = fit_info.fixed_noise;
	}else{
		if(fit_info.noise_points && fit_info.noise_points < number_points)
			fit_info.noise_std = standard_deviation((header_data.si/2)-fit_info.noise_points,(header_data.si/2)-1, data);
		else{
			fit_info.noise_std.real = 1.0;
			fit_info.noise_std.imag = 1.0;
		}
	}

	if(fit_info.noise_equal){
		fit_info.noise_std.real = (fit_info.noise_std.real +fit_info.noise_std.imag) / 2.0;
		fit_info.noise_std.imag = fit_info.noise_std.real;

	}

	//// Set Noise for image channels
	if(!strcmp(fit_info.domain, "T2_DOMAIN")){
		fit_info.noise_std.real = 1.0;
		fit_info.noise_std.imag = 1.0;
	}

	for(i=0;i<sig_max;i++)
		sig[i] = fit_info.noise_std;

	// Adjust weighting function for effects of any exponential filter which may 
	// be applied to the data in the time domain

	if(fit_info.filter != 0.0 && !strcmp(fit_info.domain, "TIME_DOMAIN")){
		for(i=0; i< sig_max; i++){
			/*sig[i].real = sig[i].real * (float)exp(-1*PI*(double)i 
				* header_data.dw * fit_info.filter);
			sig[i].imag = sig[i].imag * (float)exp(-1*PI*(double)i 
				* header_data.dw * fit_info.filter);*/
			weight[i].real = weight[i].real * (float)exp(-1*PI*(double)i 
				* header_data.dw * fit_info.filter);
			weight[i].imag = weight[i].imag * (float)exp(-1*PI*(double)i 
				* header_data.dw * fit_info.filter);

		}
	}

	if(fit_info.gaussian_filter != 0.0 && !strcmp(fit_info.domain, "TIME_DOMAIN")){
		for(i=0; i< sig_max; i++){
			weight[i].real = weight[i].real * (float)exp(-1*PI*(PI/(4*log(2)))*(double)i 
				* header_data.dw * (double)i * header_data.dw * fit_info.gaussian_filter);
			weight[i].imag = weight[i].imag * (float)exp(-1*PI*(PI/(4*log(2)))*(double)i 
				* header_data.dw * (double)i * header_data.dw * fit_info.gaussian_filter);

		}
	}



	// Adjust sig for effects of window function

/*	if(fit_info.qrt_sin_last_point != 0 && !strcmp(fit_info.domain, "TIME_DOMAIN")){
		for(i=0; i< sig_max; i++){
			sig[i].real = sig[i].real * window[i].real;
			sig[i].imag = sig[i].imag * window[i].imag;
		}
	}*/


/****	if(fit_info.filter != 0.0 && !strcmp(fit_info.domain, "FREQUENCY_DOMAIN")){
 
		// First calculate what the standard deviation would be in the
		// frequency domain.  This is the same as the standard deviation in the
		// time domain across all points after the filter has been added.  The
		// filter in the time domain has the effect of "scaling" the noise in the 
		// frequency domain.
		
		sum.real = 0.0;
		sum.imag = 0.0;****/

		// Calculate the total signal for the points used to determine the noise
		
		/*for(i=number_points-fit_info.noise_points;i<=number_points-1;i++){
			sum.real += data[i].y.real;
			sum.imag += data[i].y.imag;
		}

		// Divide by the number of points to obtain the average (should be zero 
		// unless there is a DC offset)
		
		mean.real = sum.real /(double)((number_points-1)-(number_points-fit_info.noise_points)+1);
		mean.imag = sum.imag /(double)((number_points-1)-(number_points-fit_info.noise_points)+1);

		sum.real = 0.0;
		sum.imag = 0.0;*/

		/****mean.real = 0.0;
		mean.imag = 0.0;

		// Calculate the standard deviation based on the formul sig^2 = sum[(point-mean)^2]/N
		
		for(i=fit_info.first_point-1; i< sig_max; i+=2){
			sum.real += pow((fit_info.noise_std.real*exp(-1*i*PI*header_data.dw * fit_info.filter)-mean.real),2);
			sum.imag += pow((fit_info.noise_std.imag*exp(-1*i*PI*header_data.dw * fit_info.filter)-mean.imag),2);
			sum.real += pow((-1.0*fit_info.noise_std.real*exp(-1*(i+1)*PI*header_data.dw * fit_info.filter)-mean.real),2);
			sum.imag += pow((-1.0*fit_info.noise_std.imag*exp(-1*(i+1)*PI*header_data.dw * fit_info.filter)-mean.imag),2);
		}

		// Take square root to determine the variance sig

		sum.real = sqrt(sum.real/(sig_max-(fit_info.first_point-1)));
		sum.imag = sqrt(sum.imag/(sig_max-(fit_info.first_point-1)));

		// In the frequency domain, each point will have the same variance...therefore
		// assign this variance to each element in the array sig
		
		for(i=fit_info.first_point-1; i< sig_max; i++){
			sig[i].real = sum.real;
			sig[i].imag = sum.imag;
		}
	}	****/

	// If fitting is done in the frequency domain...the data must be scaled 
	// following the FFT by the factor 1/sqrt(N), where N is the total # of 
	// points included in the FFT.  However in this routine the noise is 
	// calculated in the time domain.  Therefore it must NOT be scaled, since it 
	// is already the correct amplitude.  If the noise level were calculated in
	// the frequency domain prior to data scaling, it would have to be scaled.  
	// After the FFT the data is scaled to match the time domain data...therefore 
	// it is being scaled to match the noise level.
	

/**	if(!strcmp(fit_info.domain, FREQUENCY_DOMAIN)){
		sig_temp_real = sig[1].real;
		sig_temp_imag = sig[1].imag;
		for(i=fit_info.first_point-1; i< sig_max; i++){
			sig[i].real = sig_temp_real * 1/sqrt(zero_fill_to/fit_info.last_point);
			sig[i].imag = sig_temp_imag * 1/sqrt(zero_fill_to/fit_info.last_point);
		}
	} **/
	

//************************************
// Check if the data is to be filtered
//************************************
	if(fit_info.filter != 0.0 && !strcmp(fit_info.domain, "FREQUENCY_DOMAIN")){
		error.code = filter(data, number_points, header_data.dw, fit_info.filter);
		
		// If error condition returned
		if(error.code){
			error_message("Error filtering data.", WARNING);
			error_trap(error, fit_info);  // report error end exit
		}
	}	

//********************************************
// Check if the data is to be further windowed
//********************************************
	
/*	if (fit_info.qrt_sin_last_point!=0 && fit_info.domain == TIME_DOMAIN){
		for(i=0; i<number_points; i++){
			data[i].y.real=data[i].y.real * window[i].real;
			data[i].y.imag=data[i].y.imag * window[i].imag;
		}
	}*/

//************************************
// Check for spike removal
//************************************
	if(fit_info.spike > 0.0){
		error.code = spike(data, fit_info.last_point, fit_info.spike);
		
		output_trap(5, "spikes were removed from data.", error.code, 
				 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
		}	

//******************************
// Check Domain in which to Fit
//******************************
	if(!strcmp(fit_info.domain, "FREQUENCY_DOMAIN")){

		fid_start = fit_info.first_point;
		//eliminate the Gaussian width parameter and adjust delay time
		for (i=0; i<fit_info.number_peaks; i++){
			peak[i].parameter[WIDTH_GAUSS].value = 0;
			peak[i].parameter[WIDTH_GAUSS].linked = -1;
			peak[i].parameter[DELAY_TIME].value += (fid_start-1) * header_data.dw;
		}
		
		// Set up data to FFT 
		
		/*if(fit_info.zero_fill)
			zero_fill_to = (int)pow(2.0,ceil(log(fit_info.zero_fill-fit_info.first_point+1)/log(2.0)));
		else
			zero_fill_to = (int)pow(2.0,ceil(log(fit_info.last_point-fit_info.first_point+1)/log(2.0)));
		*/
		if(zero_fill_to > number_points)
			data = (Point *)realloc(data, sizeof(Point)*zero_fill_to);
		
		for(i=fit_info.first_point;i<fit_info.first_point+zero_fill_to;i++){
			if (i <= fit_info.last_point){
				data[i-fit_info.first_point].y.real = data[i-1].y.real;
				data[i-fit_info.first_point].y.imag = data[i-1].y.imag;
			} else {
				data[i-fit_info.first_point].y.real = 0.0;
				data[i-fit_info.first_point].y.imag = 0.0;			
			}
		}

		fit_info.zero_fill_to = zero_fill_to;
		fit_info.last_point = zero_fill_to;

		if(!fft(data, zero_fill_to)){
			error_message("Error Fourier Transforming Data", FATAL);
			exit(1);
		}


		//Scale FFT data ..This was only required when the old frequency domain function
		// was used which was based on the continuous Fourier transform.  Since the
		// discrete FT has been used, this scaling is no longer necessary!

		for(i=0;i<zero_fill_to;i++){
			
			data[i].y.real *= freq_sf;
			data[i].y.imag *= freq_sf;
		}


		// Set new x values
		for(i=0;i<zero_fill_to;i++){
			data[i].x = ((1.0/2.0)-(double)i/(double)zero_fill_to) / (header_data.dw);
			if(data[i].y.real*data[i].y.real+data[i].y.imag*data[i].y.imag > maxval){
				maxval = data[i].y.real*data[i].y.real+data[i].y.imag*data[i].y.imag ;
				maxi = i;
			}
		}

		if(fit_info.first_point_frequency == 0.0 && fit_info.last_point_frequency == 0.0){
			fit_info.first_point = 1;
			fit_info.last_point = zero_fill_to;
		}
		else{
			fit_info.first_point = (int)((1.0/2.0-header_data.dw*fit_info.first_point_frequency)*zero_fill_to);
			fit_info.last_point = (int)((1.0/2.0-header_data.dw*fit_info.last_point_frequency)*zero_fill_to);

		}

		if(fit_info.first_point < 1)
			fit_info.first_point = 1;
	
		if(fit_info.last_point > zero_fill_to)
			fit_info.last_point = zero_fill_to;
	}
		
		FILE *test;


		//commented out by jd, June 29, 2010
		/*
		test=fopen("c:\\temp\\spect.txt","wt");

		for(i=(fit_info.first_point-1);i<(fit_info.last_point-1);i++)
			fprintf(test,"%f %f\n", data[i].y.real, data[i].y.imag);

		fclose(test);
		*/

//***************************
//** Initialize Covariance **
//***************************
	covariance = new double*[fit_info.number_peaks*NUMBER_PARAMETERS+1];
	for(i=0;i<=fit_info.number_peaks*NUMBER_PARAMETERS;i++)
		covariance[i] = new double[fit_info.number_peaks*NUMBER_PARAMETERS+1];

	fit_info.lista = new int[fit_info.number_peaks*NUMBER_PARAMETERS+1];


//************************************************************************
//* Initialize linked parameters with value consistent with link condition
//************************************************************************
	for(i=0;i<fit_info.number_peaks;i++){
		for(j=0;j<peak[i].number_parameters;j++){
		 // if peak parameter not linked to self and not fixed
			if(peak[i].parameter[j].linked != i && peak[i].parameter[j].linked != FIXED)
				if(peak[i].parameter[j].link_type == OFFSET)
					peak[i].parameter[j].value = peak[peak[i].parameter[j].linked].parameter[j].value
													+ peak[i].parameter[j].modifier 
													- peak[peak[i].parameter[j].linked].parameter[j].modifier;
				else // else Ratio
					peak[i].parameter[j].value = peak[peak[i].parameter[j].linked].parameter[j].value 
													* peak[i].parameter[j].modifier
													/ peak[peak[i].parameter[j].linked].parameter[j].modifier;
		}
	}


// Check fit range 
if(fit_info.first_point < 1)
	fit_info.first_point = 1;
if(fit_info.last_point > (header_data.si/2) && !fit_info.zero_fill)
	fit_info.last_point = (header_data.si/2);


//********************************
//*  Initialize T2 parameters
//********************************/


if(!strcmp(fit_info.domain, "T2_DOMAIN")){
	for(i=0;i<fit_info.number_peaks;i++){
		if(peak[i].parameter[IMAGE_M0].value > peak[i].parameter[IMAGE_M0].max_bound ||
		   peak[i].parameter[IMAGE_M0].value < peak[i].parameter[IMAGE_M0].min_bound){
			peak[i].parameter[IMAGE_M0].value = 0.0;
			peak[i].parameter[IMAGE_M0].linked = -1;
			peak[i].parameter[IMAGE_TE].linked = -1;
			peak[i].parameter[IMAGE_T2].linked = -1;
			peak[i].parameter[IMAGE_T1].linked = -1;
			peak[i].parameter[IMAGE_PHASE].linked = -1;
		}
// Invert T2 value for fit, if T2=0 then set 1/T2 = 0 and fix
		if(peak[i].parameter[IMAGE_T2].value != 0.0){
			peak[i].parameter[IMAGE_T2].value = 1.0/peak[i].parameter[IMAGE_T2].value;
		}
		else
			peak[i].parameter[IMAGE_T2].linked = -1;
		
		if(peak[i].parameter[IMAGE_TE].value > 0.0)
			peak[i].parameter[IMAGE_TE].value  = sqrt(peak[i].parameter[IMAGE_TE].value);
		else{
			printf("\n Negative TE\n");
			exit(1);
		}


	}
}


//******************************
//*  Call Fitting Routine
//******************************
	
	data--;		// shift index to start at 1
	sig--;		// shift index to start at 1
	weight--;	// shift index to start at 1

	do{
		fit_completed = TRUE;
		fit(data, sig, peak, covariance, &fit_info, weight);

// check to see if all fit parameters are within the bounds specified in the constraints file
// if they are not, adjustments are made and the data is refit.
		
		
		for (i=0; i<fit_info.number_peaks; i++){
			for (j=0; j<peak[i].number_parameters; j++){
				if(peak[i].parameter[j].value < peak[i].parameter[j].min_bound && peak[i].parameter[j].linked != -1){

					fit_completed = FALSE;
					printf("\n%s %2d %s %2d %s", "Peak ", i+1, "parameter", 
						j+1, "has exceeded minimum bounds.");
					
					if(peak[i].parameter[j].link_type == OFFSET){
						master_value = peak[i].parameter[j].min_bound-peak[i].parameter[j].modifier;					
					} else {
						master_value = peak[i].parameter[j].min_bound/peak[i].parameter[j].modifier;
					}	

    					original_linked =  peak[i].parameter[j].linked;                                     
   
					for (l=0; l<fit_info.number_peaks; l++){
						if (j == SHIFT){
							if (peak[l].parameter[j].linked == original_linked){
								peak[l].parameter[AMPLITUDE].value = 0.0;								
								for (k=0; k<peak[l].number_parameters; k++)
									peak[l].parameter[k].linked = -1;
							}
						} else {
							if (peak[l].parameter[j].linked == original_linked){
								if (peak[l].parameter[j].link_type == OFFSET){
									peak[l].parameter[j].value = master_value + peak[l].parameter[j].modifier;
									peak[l].parameter[j].linked = -1;
									if (j == WIDTH_LORENZ){
										if (peak[l].parameter[WIDTH_LORENZ].value == 0 &&
											peak[l].parameter[WIDTH_GAUSS].value == 0 &&
											peak[l].parameter[WIDTH_GAUSS].linked == -1){

											peak[l].parameter[AMPLITUDE].value = 0.0;								
											
											for (k=0; k<peak[l].number_parameters; k++)
												peak[l].parameter[k].linked = -1;
										}
									}
									if (j == WIDTH_GAUSS){
										if (peak[l].parameter[WIDTH_GAUSS].value == 0 &&
											peak[l].parameter[WIDTH_LORENZ].value == 0 &&
											peak[l].parameter[WIDTH_LORENZ].linked == -1){

											peak[l].parameter[AMPLITUDE].value = 0.0;								

											for (k=0; k<peak[l].number_parameters; k++)
												peak[l].parameter[k].linked = -1;
										}
									}								
								} else {
									peak[l].parameter[j].value = master_value * peak[l].parameter[j].modifier;
									if (master_value == 0.0){
										for (k=0; k<peak[l].number_parameters; k++)
											peak[l].parameter[k].linked = -1;
									} else {
											peak[l].parameter[j].linked = -1;
									}
								}
							}
						}
					}
				}

				if(peak[i].parameter[j].value > peak[i].parameter[j].max_bound && peak[i].parameter[j].linked != -1){

					fit_completed = FALSE;

					printf("\n%s %2d %s %2d %s", "Peak ", i+1, "parameter", 
						j+1, "has exceeded minimum bounds.");
					
					if (peak[i].parameter[j].link_type == OFFSET){
						master_value = peak[i].parameter[j].max_bound-peak[i].parameter[j].modifier;					
					} else {
						master_value = peak[i].parameter[j].max_bound/peak[i].parameter[j].modifier;
					}	

    					original_linked =  peak[i].parameter[j].linked;                                     
   
					for (l=0; l<fit_info.number_peaks; l++){
						if (j == SHIFT){
							if (peak[l].parameter[j].linked == original_linked){
								peak[l].parameter[AMPLITUDE].value = 0.0;								
								for (k=0; k<peak[l].number_parameters; k++){
									peak[l].parameter[k].linked = -1;
								}
							}
						} else {
							if (peak[l].parameter[j].linked == original_linked){
								if (peak[l].parameter[j].link_type == OFFSET){
									peak[l].parameter[j].value = master_value + peak[l].parameter[j].modifier;
									peak[l].parameter[j].linked = -1;
									if (j == WIDTH_LORENZ){
										if (peak[l].parameter[WIDTH_LORENZ].value == 0 &&
											peak[l].parameter[WIDTH_GAUSS].value == 0 &&
											peak[l].parameter[WIDTH_GAUSS].linked == -1){

											peak[l].parameter[AMPLITUDE].value = 0.0;											
											
											for (k=0; k<peak[l].number_parameters; k++)
												peak[l].parameter[k].linked = -1;
										}
									}
									if (j == WIDTH_GAUSS){
										if (peak[l].parameter[WIDTH_GAUSS].value == 0 &&
											peak[l].parameter[WIDTH_LORENZ].value == 0 &&
											peak[l].parameter[WIDTH_LORENZ].linked == -1){

											peak[l].parameter[AMPLITUDE].value = 0.0;								
											
											for (k=0; k<peak[l].number_parameters; k++)
												peak[l].parameter[k].linked = -1;
										}
									}															
								} else {
									peak[l].parameter[j].value = master_value * peak[l].parameter[j].modifier;
									if (master_value == 0.0){
										for (k=0; k<peak[l].number_parameters; k++)
											peak[l].parameter[k].linked = -1;
									} else {
											peak[l].parameter[j].linked = -1;
									}
								}
							}
						}
					}
				}
			}
		}

	
	
	
	
	
	
	}while(! fit_completed);


// If data fit in frequency domain
	if(!strcmp(fit_info.domain, "FREQUENCY_DOMAIN")){
		for (i=0; i<fit_info.number_peaks; i++)
			peak[i].parameter[DELAY_TIME].value -= (fid_start-1) * header_data.dw;
	}


// If Image data invert T2 back
	if(!strcmp(fit_info.domain, "T2_DOMAIN")){
		for (i=0; i<fit_info.number_peaks; i++){
			if(peak[i].parameter[IMAGE_T2].value != 0.0)
				peak[i].parameter[IMAGE_T2].value = 1.0/peak[i].parameter[IMAGE_T2].value;

	
  			peak[i].parameter[IMAGE_TE].value  *= peak[i].parameter[IMAGE_TE].value;
		}

	}


//***************************************
// * Write Covariance Matrix (optional)
//***************************************
	if(filename_covariance[0])
		write_covariance_file(filename_covariance, covariance, &fit_info);


//******************************
//*  Write Guess File out
//******************************
	
	write_guess_file(filename_guess_out, peak, covariance, &fit_info, &variable);


// Free Arrays and structures
	if(peak){ // if peak declared delete
		for(i=0;i<fit_info.number_peaks;i++)
			delete peak[i].parameter;
		delete peak;
	}

	if(covariance){  // if covariance declared delete
		for(i=0;i<=fit_info.number_peaks*NUMBER_PARAMETERS;i++)
			delete covariance[i];
		delete covariance;
	}
	

	delete fit_info.lista;

	if(data)	// if data declared delete
		delete (data+1);
	if(sig)		// if sig declared delete
		delete (sig+1);

	printf("\nFit Completed \a\n");

	return(TRUE);

}// end main



//*****************************************
//*********  Standard Deviation ***********
//*****************************************

Complex standard_deviation(int start, int end, Point *data){


	int i;
	Complex sum={0.0,0.0};
	Complex mean;
	Complex std;


// Calculate mean 
	for(i=start;i<=end;i++){
		sum.real += data[i].y.real;
		sum.imag += data[i].y.imag;
	}

	mean.real = sum.real /(double)(end-start+1);
	mean.imag = sum.imag /(double)(end-start+1);


// Calculate Standard Deviation
	sum.real = 0.0;
	sum.imag = 0.0;

	for(i=start;i<=end;i++){
		sum.real += pow(data[i].y.real-mean.real,2);
		sum.imag += pow(data[i].y.imag-mean.imag,2);
	}
	
	std.real = sqrt(sum.real/(double)(end-start));
	std.imag = sqrt(sum.imag/(double)(end-start));

	return(std);

} // end standard_deviation


//*****************************************
//************  Error Trap ****************
//*****************************************
void error_trap(Error error, Fit_info fit_info){

	char error_string[80];

	if(error.line < 0){
		switch(error.code){		
		case  1	:	sprintf(error_string, "Operator not recognized, %s section, Token: %d",
							(error.line==-2) ? "[Parameters]" : "[Variables]",
							error.element);
					break;
		case  2	:	sprintf(error_string, "Variable not defined %s section, Token: %d",
							(error.line==-2) ? "[Parameters]" : "[Variables]",
							error.element);
					break;
		default	:	sprintf(error_string, "Extended Error %s section, Token: %d",
							(error.line==-2) ? "[Parameters]" : "[Variables]",
							error.element);
					break;
		}
	}
	else{
		switch(error.code){

		case -1	:	sprintf(error_string,"Error opening file\n");
					break;
		case  1	:	sprintf(error_string, "Operator not recognized, Peak: %d  Parameter: %d\n",
						                   error.line+1, error.element);
					break;
		case  2	:	sprintf(error_string, "Variable not defined, Peak: %d  Parameter: %d\n",
						                   error.line+1, error.element);
					break;
		case  3	:	sprintf(error_string, "Peak # greater than maximum: %d, Peak: %d\n",
						                   fit_info.number_peaks, error.line+1);
					break;
		case  4	:	sprintf(error_string, "Exceeded maximum link variables");
					break;
		case  5	:	sprintf(error_string, "Link Operator Not Allowed Peak: %d, Parameter : %d\n",
						                   error.line+1, error.element);
					break;
		case  6	:	sprintf(error_string, "Error filtering data");
					break;
		default	:	sprintf(error_string,"extended error #%d", error.code);
					break;
		}

	}
	error_message(error_string, FATAL);
}  // end error_trap


int spike(Point *data, int number_points, double spike){

int number_spikes, i;
double test_real, test_imag;

number_spikes = 0;

	for (i=1; i<number_points-1; i++){
		
		test_real = fabs(data[i].y.real - data[i-1].y.real) + fabs(data[i].y.real - data[i+1].y.real);
		test_imag = fabs(data[i].y.imag - data[i-1].y.imag) + fabs(data[i].y.imag - data[i+1].y.imag);

		if (test_real > spike * (fabs(data[i-1].y.real) + fabs(data[i+1].y.real))){ 
			data[i].y.real = (data[i-1].y.real + data[i+1].y.real)/2;
			number_spikes++;
		}
		if (test_imag > spike * (fabs(data[i-1].y.imag) + fabs(data[i+1].y.imag))){
			data[i].y.imag = (data[i-1].y.imag + data[i+1].y.imag)/2;
			number_spikes++;
		}
	}

	return number_spikes;
}
	



//***************************
//*       Window Data
//***************************

// 	dwell_time in seconds
//  number_lb  in Hertz;


int filter(Point *data, int number_points, double dwell_time, double number_lb){

	int i;


	if(number_lb != 0){
		for(i=0; i<number_points; i++){
			data[i].y.real=data[i].y.real*(float)exp(-1*PI*(double)i
				*dwell_time*number_lb);
			data[i].y.imag=data[i].y.imag*(float)exp(-1*PI*(double)i
				*dwell_time*number_lb);
		}
		return 0;
	}
	else return 6;
}

