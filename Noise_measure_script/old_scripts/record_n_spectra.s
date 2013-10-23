//          record n eels spectra
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

// started on 2-3-2007
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Jo Verbeeck
// Institut fuer Festkoerperphysik, TU Wien, Austria
// EMAT, University of Antwerp, BE
// eelsmodel@ua.ac.be
/////////////////////////////////////////////////////////////////////////////////////////////////////

void converttoeels(image im,image &eels)
{
	//function to get an EELS spectrum from the image area defined in the IF suite 
	//sum in y direction to get EELS spectrum
	number w,h
	get2dsize(im,w,h)
	number j
	Clearimage(eels)
	for (j=1;j<=h;j++){
		eels=eels+real(im[j-1,0,j,w])
	}
}

image getimage(number expt,number binning, number top, number left, number bottom, number right)
{
image im
	//aqcuire an image from ccd
	//if no ccd is present then create a fake noise image
	try{
	im=SSCGainNormalizedBinnedAcquire(expt,binning, round(top/binning), round(left/binning), round(bottom/binning), round(right/binning))
	}
	catch
	{
	//make a dummy image
	result("using a fake image for testing...you should not see this when working at microscope\n")
	im= RealImage( "Fake",4,abs(right-left), abs(bottom-top ))

	im=PoissonRandom(1000*expt)	
	//simulate beam variation
	number dcoffset=PoissonRandom(10)
	number Gt=2 //the true (signal) gain of the detector MTF(0)
	im=im*Gt+dcoffset

	//now correlate them		

	number sig=1
	image gauss=im
	gauss=exp(-(real(iradius)/sig)**2)
	gauss=gauss/sum(gauss)	
	im=abs(realifft(realfft(im)*realfft(gauss))) //convolve with gaussian to simulate psf of ccd

	  //show the final result that we should obtain
		//image eels:=realimage("projected gauss",4,abs(right-left),1)
		//converttoeels(gauss,eels,bottom,top,left,right)
		//eels=abs(realfft(eels))**2
		//showimage(eels)	

	if (binning==2){
		image im2= RealImage( "Fake",4,abs(right-left)/2, abs(bottom-top )/2)		
		im2=im[2*icol,2*irow]+im[2*icol+1,2*irow]+im[2*icol,2*irow+1]+im[2*icol+1,2*irow+1]		
		return im2
	}
	if (binning==4){
		image im2= RealImage( "Fake",4,abs(right-left)/2, abs(bottom-top )/2)		
		image im4= RealImage( "Fake",4,abs(right-left)/4, abs(bottom-top )/4)				
		im2=im[2*icol,2*irow]+im[2*icol+1,2*irow]+im[2*icol,2*irow+1]+im[2*icol+1,2*irow+1]		
		im4=im2[2*icol,2*irow]+im2[2*icol+1,2*irow]+im2[2*icol,2*irow+1]+im2[2*icol+1,2*irow+1]		
		return im4	
	}
	
	return im
	}
return im
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION


result("\n  Recording a series of EELS spectra...\n")

number top,bottom,left,right,binning,expt,maxexpt,minexpt,i,n,j,meanelectrons,darkcorrection,powerspectrum,i1,i2,i4,i1max,i2max,i4max
image im,im1,im2,dim,dim1,dim2,eels,varim,meanim,compareim,power,power2,power4,eels2,eels4

number varfactor=1/sqrt(2) //correction factor because we take the noise signal to be the difference of two spectra 

//options
darkcorrection=0 //correct for variance in dark reference, usually this is an extremely small correction
powerspectrum=1 //do you want to measure an accurate shape for the noise power spectrum?

i1max=50 //number of itterations for power spectrum at binning 1
i2max=100 //number of itterations for power spectrum at binning 2
i4max=300 //number of itterations for power spectrum at binning 4
//

//get the area of ccd used for eels
if(!GetPersistentRectNote("IF Suite:IFS:Area",top,left,bottom,right)){
	okdialog("IF Suite tag not found in global info")
	exit(0)
}



n=20 //number of points used for var vs mean
okdialog("PPPC measuring tool: make sure to do a good gain calibration before calling this script, and have a white ilumination")
getnumber("Get maximum exposure time (careful for saturation)",1,maxexpt)
getnumber("Get minimum exposure time",0.1,minexpt)

getnumber("Amount of exposures for variance vs. mean",n,n)

getnumber("Amount of exposures at 1x binning for power spectrum",i1max,i1max)
getnumber("Amount of exposures at 2x binning for power spectrum",i2max,i2max)
getnumber("Amount of exposures at 4x binning for power spectrum",i4max,i4max)

binning=1

eels:=realimage("eels",4,abs(right-left),1)
eels2:=realimage("eels2",4,abs(right-left)/2,1)
eels4:=realimage("eels4",4,abs(right-left)/4,1)
power:=realimage("noise power spectrum",4,abs(right-left),1)
power2:=realimage("noise power spectrum binning 2",4,abs(right-left)/2,1)
power4:=realimage("noise power spectrum binning 4",4,abs(right-left)/4,1)



varim=realimage("varvsmean",4,n,1)
meanim=realimage("varvsmean",4,n,1)
showimage(eels)

for (i=1;i<=n;i++){
	//capture a white image of this size and show it
	
	expt=minexpt+((i-1)/n)*(maxexpt-minexpt)

	im1=getimage(expt, binning, top, left, bottom, right)
	//capture a second image and calculate the difference with the previous
	//this must be pure noise cancelling out bad pixels, gain variations etc
	im2=getimage(expt, binning, top, left, bottom, right)
	
	im=varfactor*(im1-im2) //correct for variance of a sum is sum of variances
	
	meanelectrons=mean(im2)*abs(top-bottom)
	converttoeels(im,eels)	
	updateimage(eels)

	//calculate variance and mean and store them	
	setpixel(varim,i-1,0,variance(eels))
	//are independent (should be if aftergrlow is negligible effect)
	setpixel(meanim,i-1,0,meanelectrons) //for the mean we need the original image
	result("var="+variance(eels)+"	mean="+meanelectrons+"	exposuretime="+expt+"\n")
}

//calculate linear regression
number Gvar,b,r,pppc,x,y
fitline(meanim,varim,Gvar,b,r)
result("Gvar="+Gvar+"	b="+b+"	R-factor (goodness of fit, should be smaller than 1)="+r+"\n")

//calculate predicted variance and plot together with measured variance
compareim:=realimage("measured and fitted variance",4,n,2)
for (i=1;i<=n;i++){
	x=getpixel(meanim,i-1,0)
	y=getpixel(varim,i-1,0)
	number ypredict=Gvar*x+b
	setpixel(compareim,i-1,0,y)
	setpixel(compareim,i-1,1,ypredict)
}
setdisplaytype(compareim,4) //set to line plot
showimage(compareim)

	


if (darkcorrection==1){
	okdialog("Lower the viewing screen to aquire dark images with exaclty the same settings")
	//now determine correction for dark counts
	//determine variance as a function of exposure time for a dark image
	image darkvarim:=realimage("varvsmean",4,n,1)
	image expim:=realimage("varvsmean",4,n,1)

	//first a dummy exposure to get rid of temp. artefacts
	dim2=getimage(expt, binning, top, left, bottom, right)
	//now the first real exposure
	dim2=getimage(expt, binning, top, left, bottom, right)
	for (i=1;i<=n;i++){
		//capture a dark image of this size and show it
		expt=minexpt+((i-1)/n)*(maxexpt-minexpt)

		//aquire Dark image
		//	im=SSCGetDarkReference(expt, binning, top, left, bottom, right) //problem not gain corrected
		dim1=getimage(expt, binning, top, left, bottom, right)

		//capture a second image and calculate the difference with the previous
		//this must be pure noise cancelling out bad pixels, gain variations etc
		dim2=getimage(expt, binning, top, left, bottom, right)
		dim=varfactor*(dim1-dim2) //correct for variance of sum		

		//updateimage(dim)
		converttoeels(dim,eels)	
		updateimage(eels)

		//calculate variance and mean and store them	
		setpixel(darkvarim,i-1,0,variance(eels))
		setpixel(expim,i-1,0,expt)
		result("dark var="+variance(eels)+"	exposuretime="+expt+"\n")
	}

	//correct variance for dark variance
	varim=varim-darkvarim
	//fit again, this gives the pppc for zero exposure time
	fitline(meanim,varim,Gvar,b,r)

	result("dark corrected Gvar="+Gvar+"	b="+b+"	R-factor (goodness of fit, should be smaller than 1)="+r+"\n")
}


if (powerspectrum==1){
	okdialog("Make sure the viewing screen is up\n Determination of noise power spectrum\n")	
	
	number dose
	//determine power spectrum of noise
	//at highest exposure time

	number mid=(right-left)/2

	//start aqcuisition loop
	expt=maxexpt
	power=0	
	power2=0
	power4=0
	
	showimage(power)

	//do a dummy image to start to avoid large DC component in the spectrum
	//this is related to the dark current which is dependent on the current temperature
	//of the detector. This changes whith readout since the detector is continouously aqcuiring
	//during idle exposure 
	

	im2=getimage(expt, binning, top, left, bottom, right)
	//second one should be better already
	im2=getimage(expt, binning, top, left, bottom, right)
	//third one should be better already
	
	

	im2=getimage(expt, binning, top, left, bottom, right)
	for (i1=0;i1<i1max;i1++){
		im1=getimage(expt, binning, top, left, bottom, right)
		//capture a second image and calculate the difference with the previous
		//this must be pure noise cancelling out bad pixels, gain variations etc
		im=varfactor*(im1-im2) //correct for difference is sqrt(2) sigma
		im2=im1 //remember for next step
		converttoeels(im,eels)
		
		//updateimage(eels)
		//calculate power spectrum of noise
		power=power+(abs(realfft(eels))**2) //add power spectrum		

		//correct for high DC spike in experiment
		number midpos=round(abs(left-right)/(2*binning))
		setpixel(power,midpos,0,(getpixel(power,midpos-1,0)+getpixel(power,midpos+1,0))/2)

		updateimage(power)		

	}

	showimage(power2)

	//now swith to 2x binning
	binning=2

	image im12,im22,im02
	result("Switching to binning 2 to get better snr on the low frequencies\n")
	expt=expt/4

	//do a dummy image to start to avoid large DC component in the spectrum
	//this is related to the dark current which is dependent on the current temperature
	//of the detector. This changes whith readout since the detector is continouously aqcuiring
	//during idle exposure 
	im12=getimage(expt, binning, top, left, bottom, right)

	//second one should be better already
	im22=getimage(expt, binning, top, left, bottom, right)	
	//third one should be better already
	showimage(power2)
	//showimage(eels2)
	im22=getimage(expt, binning, top, left, bottom, right)	

	for (i2=0;i2<i2max;i2++){
		im12=getimage(expt, binning, top, left, bottom, right)
		//capture a second image and calculate the difference with the previous
		//this must be pure noise cancelling out bad pixels, gain variations etc
		im02=varfactor*(im12-im22) //correct for difference is sqrt(2) sigma		
		im22=im12 //remember for next step
		converttoeels(im02,eels2)		
		//updateimage(eels2)
		//calculate power spectrum of noise
		power2=power2+(abs(realfft(eels2))**2) //add power spectrum	
		
		//correct for high DC spike in experiment
		number midpos=round(abs(left-right)/(2*binning))
		setpixel(power2,midpos,0,(getpixel(power2,midpos-1,0)+getpixel(power2,midpos+1,0))/2)	
		
		updateimage(power2)
	}

	
	showimage(power4)

	//now swith to 4x binning
	binning=4
	image im14,im24,im04
	result("Switching to binning 4 to get better snr on the low frequencies\n")
	expt=expt/4

	//do a dummy image to start to avoid large DC component in the spectrum
	//this is related to the dark current which is dependent on the current temperature
	//of the detector. This changes whith readout since the detector is continouously aqcuiring
	//during idle exposure 
	im14=getimage(expt, binning, top, left, bottom, right)

	//second one should be better already
	im24=getimage(expt, binning, top, left, bottom, right)
	
	//third one should be better already
	showimage(power4)
	//showimage(eels4)


	for (i4=0;i4<i4max;i4++){
		im14=getimage(expt, binning, top, left, bottom, right)
		//capture a second image and calculate the difference with the previous
		//this must be pure noise cancelling out bad pixels, gain variations etc
		im04=varfactor*(im14-im24) //correct for difference is sqrt(2) sigma
		im24=im14 //remember for next step
		converttoeels(im04,eels4)			
		//updateimage(eels4)
		//calculate power spectrum of noise
		power4=power4+(abs(realfft(eels4))**2) //add power spectrum	

		//correct for high DC spike in experiment
		number midpos=round(abs(left-right)/(2*binning))
		setpixel(power4,midpos,0,(getpixel(power4,midpos-1,0)+getpixel(power4,midpos+1,0))/2)	
	
		updateimage(power4)
	}


	//normalise them to same total noise power
	power=power/i1max
	power2=4*power2/i2max
	power4=16*power4/i4max


	updateimage(power)
	updateimage(power2)
	updateimage(power4)
	

	//now combine the power spectra to 1 power spectrum with smooth low frequencies	
	//make use of this wonderful scripting language with non-existent documentation
	//and counterintuitive indexing....I love it!
	

	//temp=front[icol, ysize-irow] //trick to flip
	//ShiftCenter() //like fftshift in matlab

	number pi=2*asin(1)	

	

	//correct for effect of binning and alliasing on power spectra
	//see test in ntfoversample.m
	// this is the correct way to remove aliasing effects,
	// subtle details here...
	number L=2 //oversampling factor		

	////////////////////////////////for binning2
	image h=power
	h=(1/L)*(sin(pi*(icol/iwidth-0.5)*L)/sin(pi*(icol/iwidth-0.5)));
	image h2=power2
	h2=(1/L)*(sin(pi*(0.5*(icol/iwidth-0.5))*L)/sin(pi*(0.5*(icol/iwidth-0.5))));
	//correct for 0/0 in center
	number w=abs(left-right)
	setpixel(h,w/2,0,1)
	setpixel(h2,w/4,0,1)


	setname(h,"h")
	setname(h2,"h2")

	//showimage(h)
	//showimage(h2)
	
	image temp2=power
	temp2=power*(abs(h)**2) //simulate 2x binning effect on 1x binning data, but apply n abs(fft()) and not on the power spectrum
	Shiftcenter(temp2)
	image corrector=temp2[0,mid/2,1,3*mid/2] //take the right part of the binning1 power spectrum 
	image power2corr=(power2-corrector)/(abs(h2)**2); //subtract flipped right part of power to correct for aliasing

	setname(power2corr,"power2 corrected")
	showimage(power2corr)
	showimage(corrector)

	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////for binning4
	image h3=power2
	h3=(1/L)*(sin(pi*(icol/iwidth-0.5)*L)/sin(pi*(icol/iwidth-0.5)));
	image h4=power4
	h4=(1/L)*(sin(pi*(0.5*(icol/iwidth-0.5))*L)/sin(pi*(0.5*(icol/iwidth-0.5))));
	image h5=power4
	h5=(1/L)*(sin(pi*(0.25*(icol/iwidth-0.5))*L)/sin(pi*(0.25*(icol/iwidth-0.5))));
	//correct for 0/0 in center

	setpixel(h3,w/4,0,1)
	setpixel(h4,w/8,0,1)
	setpixel(h5,w/8,0,1)


	setname(h3,"h3")
	setname(h4,"h4")
	setname(h5,"h5")

	//showimage(h3)
	//showimage(h4)
	//showimage(h5)
	
	image temp3=power2
	temp3=power2*(abs(h3)**2) //simulate 2x binning effect on 1x binning data, but apply n abs(fft()) and not on the power spectrum
	Shiftcenter(temp3)
	image corrector2=temp3[0,mid/4,1,3*mid/4] //take the right part of the binning1 power spectrum 
	image power4corr=(power4-corrector2)/((abs(h4)*abs(h5))**2); //subtract flipped right part of power to correct for aliasing

	setname(power4corr,"power4 corrected")
	showimage(power4corr)
	//showimage(corrector2)

	////////////////////////////////////////////////////////////////////////////////////

	
	//now build power from the subregions
	image powerc=power
	powerc[0,mid-mid/2,1,mid+mid/2]=power2corr
	powerc[0,mid-mid/4,1,mid+mid/4]=power4corr
		
	showimage(powerc)

	setname(powerc,"power spectrum using oversampling")

	



	//now continue to work with the power spectrum obtained from oversampling

	//smooth power spectrum
	image powersmooth

	image gauss=power
	number sig=30
	gauss=exp(-(iradius/sig)**2)
	gauss=gauss/sum(gauss)
	powersmooth=realifft(realfft(powerc)*realfft(gauss))
	shiftcenter(powersmooth)

	//	powersmooth=MedianFilter(powerc,0,30) //source,type,size
	showimage(powersmooth)

	powersmooth=powersmooth/max(powersmooth[0,mid-mid/8,1,mid+mid/8]) //scale zero frequency to one
	number C=abs(right-left)/sum(powersmooth)
	setname(powersmooth,"smoothed power spectrum")
	showimage(powersmooth)

	number G=C*Gvar
	//report on outcome

	result("/n")
	result("A correlation factor C="+C+" was found\n")
	result("Check to see whether the scaling of the smooth power spectrum at the maximum is realistic\n")
	result("Otherwise record more spectra to get a smoother result around zero spatial frequency\n")
	result("The observed correlation factor gives a total Gain for the detector G="+G+"\n")
	result("\n")
	result("Add the folowing line to the detectors.txt file in your eelsmodel distribution release directory\n")
	result("the name of my detector \t"+1/G+"\t"+C+"\n")
	result("make sure to end the last line in this file with enter\n")
	result("\n")
	result("the END...")
}