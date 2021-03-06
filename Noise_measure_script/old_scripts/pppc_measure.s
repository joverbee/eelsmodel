//          measure pppc
// a help script to use with EELSMODEL
// started 9-3-2006 by Jo Verbeeck
// updated and tested on 7-6-2006 Jo Verbeeck
// update on 29-6-2006 Jo Verbeeck
//		use difference between two oimages to avoid problems with gain ref 
// 		measured pppc for jeol 3000F 1K gatan Gif 2000 pppc=0.18
//		added noise power spectra measurement, important for dealing with correlated noise

//update on 8/11/06
//     corrected an error in the variance of the difference of two spectra. Should have a correction factor 2!

// EMAT
// University of Antwerp

//the script works as follows:
// a set of white images is taken in image mode
// make sure to use even illumination and a good gain correction (at least 10 times at about 80% of saturation)
// a pure noise image is created by subtracting two subsequent images taken under the same conditions
// this noise image is converted into EELS images by summing them vertically over a region that is described in the IF tags
// this fake 'white' EELS spectrum for different exposure times is used to measure the variance in the white image
// as a function of the mean number of counts
// a linear fit to this plot is made and the inverse of the slope of this curve is the pppc
// and gives a relation between the noise properties of the ccd and the number of counts
// this value can be seen as a REAL estimate of the sensitivity of your camera, as opposed to the gain factor
// that is usualy measured with a known current. The difference is that the noise in the total system makes it more difficult to distinguish particles

// in a second step a correction is made for the variance due to the different exposure times by aquiring a set of dark images
// this plot however shows that there is variance even without signal, which is not true for a poissonian noise model
// this correction is extremely low and is usually not needed (put darkcorrection=0)

// in a last step the power spectrum of the noise is measured by collecting a large number of noise
// spectra (taken as the difference of two images and summing to get an eels spectrum) is taken to approximate
// the true power spectrum of the noise 
// be carefull as this function still contains alliasing...to get rid of that and find the real noise transfer function
// see R. Meier paper 

// in general we might think of a conversion of the observed counts to a new set with poissonian noise properties
// by applying a transformation of the form y=(x-b)*pppc with b the intersection of the variance at zero intensity


// pay attention to the following:
// make a good gain reference before doing this script
// cover the viewing screen so no stray light can enter the GIF
// remove all dark references to force new one to be taken
// make sure the filter function "convert image to eels spectrum" is set up to use "simple vertical sum"
// make sure anti-blooming is off (unless you use this when recording eels spectra-the noise is higher)



void fitline(image xim,image yim, number &a,number &b,number &r)
{
	//function for linear regression
	number sumx,sumy,sumxy,sumxx,sumyy,x,y,dummy,n,i

	sumxy=0
	sumx=0
	sumy=0
	sumxx=0
	sumyy=0

	getsize(xim,n,dummy) //assume both xim and yimsame size

	for (i=1;i<=n;i++){
		x=getpixel(xim,i-1,0)
		y=getpixel(yim,i-1,0)
		sumxy=sumxy+x*y
		sumx=sumx+x
		sumy=sumy+y
		sumxx=sumxx+x*x
		sumyy=sumyy+y*y
	}
	a=(n*sumxy-sumx*sumy)/(n*sumxx-sumx*sumx) //slope
	b=(sumy-a*sumx)/n //interception
	r=(n*sumxy-sumx*sumy)/sqrt((n*sumxx-sumx*sumx)*(n*sumyy-sumy*sumy)) //the R value
}



void converttoeels(image im,image &eels,number bottom, number top,number left,number right)
{
	//function to get an EELS spectrum from the image area defined in the IF suite 
	//sum in y direction to get EELS spectrum
	number j
	Clearimage(eels)
	for (j=1;j<=(bottom-top);j++){
		eels=eels+real(im[j-1,left,j,right])
	}
}



// MAIN FUNCTION


result("\n  PPPC measuring utility...\n")
number top,bottom,left,right,binning,expt,maxexpt,minexpt,i,n,j,meanelectrons,darkcorrection,powerspectrum
image im,im2,dim,eels,varim,meanim,compareim,power


//options
darkcorrection=0 //correct for variance in dark reference, usually this is an extremely small correction
powerspectrum=1 //do you want to measure an accurate shape for the noise power spectrum?
//

	//get the area of ccd used for eels
	if(!GetPersistentRectNote("IF Suite:IFS:Area",top,left,bottom,right)){
		okdialog("IF Suite tag not found in global info")
		exit(0)
	}


n=10 //number of points
okdialog("PPPC measuring tool: make sure to do a good gain calibration before calling this script, and have a white ilumination")
getnumber("Get maximum exposure time (careful for saturation)",1,maxexpt)
getnumber("Get minimum exposure time",0.1,minexpt)

binning=1
eels:=realimage("eels",4,abs(right-left),1)
power:=realimage("noise power spectrum",4,abs(right-left),1)
varim:=realimage("varvsmean",4,n,1)
meanim:=realimage("varvsmean",4,n,1)
showimage(eels)
for (i=1;i<=n;i++){
	//capture a white image of this size and show it
	
	expt=minexpt+((i-1)/n)*(maxexpt-minexpt)

	im=SSCGainNormalizedBinnedAcquire(expt, binning, top, left, bottom, right)
	//capture a second image and calculate the difference with the previous
	//this must be pure noise cancelling out bad pixels, gain variations etc
	im2=SSCGainNormalizedBinnedAcquire(expt, binning, top, left, bottom, right)
	im=im-im2
	meanelectrons=mean(im2)*abs(top-bottom)
	//updateimage(im)
	converttoeels(im,eels,bottom,top,left,right)	
	updateimage(eels)

	//calculate variance and mean and store them	
	setpixel(varim,i-1,0,0.5*variance(eels)) //important to take 0.5 here since the variance of a difference is 2*var of the orig if they
	//are independent (should be if aftergrlow is negligible effect
	setpixel(meanim,i-1,0,meanelectrons) //for the mean we need the original image
	result("var="+variance(eels)+"	mean="+meanelectrons+"	exposuretime="+expt+"\n")
}
//calculate linear regression
number a,b,r,pppc,x,y
fitline(meanim,varim,a,b,r)
pppc=1/a
result("pppc="+pppc+"	b="+b+"	R-factor (goodness of fit, should be smaller than 1)="+r+"\n")

//calculate predicted variance and plot together with measured variance
compareim:=realimage("measured and fitted variance",4,n,2)
for (i=1;i<=n;i++){
	x=getpixel(meanim,i-1,0)
	y=getpixel(varim,i-1,0)
	number ypredict=a*x+b
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
	for (i=1;i<=n;i++){
		//capture a dark image of this size and show it
		expt=minexpt+((i-1)/n)*(maxexpt-minexpt)

		//aquire Dark image
		//	im=SSCGetDarkReference(expt, binning, top, left, bottom, right) //problem not gain corrected
		dim=SSCGainNormalizedBinnedAcquire(expt, binning, top, left, bottom, right)

		//updateimage(dim)
		converttoeels(dim,eels,bottom,top,left,right)	
		updateimage(eels)

		//calculate variance and mean and store them	
		setpixel(darkvarim,i-1,0,0.5*variance(eels)) //correct for variance of difference by factor 0.5
		setpixel(expim,i-1,0,expt)
		result("dark var="+variance(eels)+"	exposuretime="+expt+"\n")
	}

	//correct variance for dark variance
	varim=varim-darkvarim
	//fit again, this gives the pppc for zero exposure time
	fitline(meanim,varim,a,b,r)
	pppc=1/a

	result("PPPC corrected for dark counts\n")
	result("dark corrected pppc="+pppc+"	b="+b+"	R-factor (goodness of fit, should be smaller than 1)="+r+"\n")
}

result("\nThe detector has a useable Quantum efficiency for constant ilumination of "+1/pppc+" counts per electron\n")

if (powerspectrum==1){
	okdialog("Make sure the viewing screen is up\n Determination of noise power spectrum, press space when acceptable accuracy is reached")	
	number dose
	//determine power spectrum of noise
	//at highest exposure time

	//start aqcuisition loop
	expt=maxexpt
	power=0	
	showimage(power)

	//do a dummy image to start to avoid large DC component in the spectrum
	//this is related to the dark current which is dependent on the current temperature
	//of the detector. This changes whith readout since the detector is continouously aqcuiring
	//during idle exposure 
	im2=SSCGainNormalizedBinnedAcquire(expt, binning, top, left, bottom, right)
		
	while (!spacedown()){
		im=SSCGainNormalizedBinnedAcquire(expt, binning, top, left, bottom, right)
		//capture a second image and calculate the difference with the previous
		//this must be pure noise cancelling out bad pixels, gain variations etc
		im2=SSCGainNormalizedBinnedAcquire(expt, binning, top, left, bottom, right)
		im=im-im2
		converttoeels(im,eels,bottom,top,left,right)	
		//updateimage(eels)
		//calculate power spectrum of noise
		power=power+(abs(realfft(eels))**2) //add power spectrum
		dose=sum(power) //total dose in power spectrum
		updateimage(power)
	}
	power=power/dose //scale to dose of 1 count
	updateimage(power)
}