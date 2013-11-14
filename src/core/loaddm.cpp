/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - core/loaddm.cpp
 **/

#include "src/core/loaddm.h"

#include <istream>
  using std::istream;
#include <string>
  using std::string;

#include "src/core/multibyte.h"
#include "src/core/multispectrum.h"
#include "src/core/tag.h"

Tag readType(istream& dmfile,
             int type,
             bool byteorder,
             const string& tagname,
             bool inside14)
{
  //return a string with the read data of type
  string valuestring;
  //do this info(nnum) times in case this is inside a type 14h tag
  int ngroups=0;

  if(inside14 && type!=15)
  {
    Multibyte i4=Multibyte(4);
    i4.get(dmfile); //should be zero
    ngroups = i4.getvalue();
  }
  else
    ngroups=1;
#ifdef DEBUG_IMAGE_DETAIL
  std::cout<<"reading "<<ngroups<<" groups\n";
#endif
  switch(type)
  {
    case 2:
    {
      //i2* signed
      Multibyte i2(2);
      Multibyte i2r(2,true);
      int value;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag tag(tagname, ngroups, Tag::tagi2);

      for(int g=0; g<ngroups; ++g)
      {
        if(byteorder)
        {
          i2r.get(dmfile);
          value=i2r.getsignedvalue();
        }
        else
        {
          i2.get(dmfile);
          value=i2.getsignedvalue();
        }
#ifdef DEBUG_detail
        std::cout<<"a i2* signed info number with value:"<<value<<"\n";
        i2.debug();
        i2r.debug();
#endif
        tag.seti2(g,value);
      }
      return tag;
    }
    case 3:
    {
      //i4* signed
      Multibyte i4=Multibyte(4);
      Multibyte i4r=Multibyte(4,true);
      long value=0;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag tag(tagname,ngroups,Tag::tagi4);

      for(int g=0; g<ngroups; ++g)
      {
        if(byteorder)
        {
          i4r.get(dmfile);
          value=i4r.getsignedvalue();
        }
        else
        {
          i4.get(dmfile);
          value=i4.getsignedvalue();
        }
#ifdef DEBUG_detail
        std::cout<<"a i4* signed info number with value:"<<value<<"\n";
        i4.debug();
        i4r.debug();
#endif
        tag.seti4(g, value);
      }
      return tag;
    }
    case 4:
    {
      //i2* unsigned
      Multibyte i2=Multibyte(2);
      Multibyte i2r=Multibyte(2,true);
      int value=0;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag tag(tagname,ngroups,Tag::tagi2);

      for(int g=0; g<ngroups; ++g)
      {
        if(byteorder)
        {
          i2r.get(dmfile);
          value=i2r.getvalue();
        }
        else
        {
          i2.get(dmfile);
          value=i2.getvalue();
        }
#ifdef DEBUG_detail
        std::cout<<"a i2* unsigned info number with value:"<<value<<"\n";
        i2.debug();
        i2r.debug();
#endif
        tag.seti4(g, value); //may not fit into an signed int!
      }
      return tag;
    }
    case 5:
    {
      //i4* unsigned
      Multibyte i4=Multibyte(4);
      Multibyte i4r=Multibyte(4,true);
      long value=0;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag tag(tagname,ngroups,Tag::tagi4);

      for(int g=0; g<ngroups; ++g)
      {
        if(byteorder)
        {
          i4r.get(dmfile);
          value=i4r.getvalue();
        }
        else
        {
          i4.get(dmfile);
          value=i4.getvalue();
        }
#ifdef DEBUG_detail
        std::cout<<"a i4* unsigned info number with value:"<<value<<"\n";
        i4.debug();
        i4r.debug();
#endif
        tag.seti4(g,value);
      }
      return tag;
    }
    case 6:
    {
      //f4*
      Multibyte f4=Multibyte(4);
      Multibyte f4r=Multibyte(4,true);
      double value=0;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag thistag(tagname,ngroups,Tag::tagf4);

      for(int g=0; g<ngroups; ++g)
      {
        if(byteorder)
        {
          f4r.get(dmfile);
          value=f4r.getfloatvalue();
        }
        else
        {
          f4.get(dmfile);
          value=f4.getfloatvalue();
        }
#ifdef DEBUG_detail
        std::cout<<"a f4* info number with value:"<<value<<"\n";
        f4.debug();
        f4r.debug();
#endif
        tag.setf4(g,value);
      }
      return tag;
    }
    case 7:
    {
      //f8*
      Multibyte f8=Multibyte(8);
      Multibyte f8r=Multibyte(8,true);
      double value=0;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag thistag(tagname,ngroups,Tag::tagf8);

      for(int g=0; g<ngroups; ++g)
      {
        if(byteorder)
        {
          f8r.get(dmfile);
          value=f8r.getfloatvalue();
        }
        else
        {
          f8.get(dmfile);
          value=f8.getfloatvalue();
        }
#ifdef DEBUG_detail
        std::cout<<"a f8* info number with value:"<<value<<"\n";
        f8.debug();
        f8r.debug();
#endif
        tag.setf8(g, value);
      }
      return tag;
    }

    case 8:{
      //i1
      Multibyte i1=Multibyte(1);
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag* thistag;
      if (newtag) {
        thistag=new Tag(tagname,ngroups,Tag::tagi1);
        taglist.push_back(thistag);
      }
      else {
        thistag=taglist.back(); //take the last one
      }
      for (int g=0;g<ngroups;g++){
        i1.get(dmfile);
        const int value=i1.getvalue();
#ifdef DEBUG_detail
        std::cout<<"a i1 info number with value:"<<value<<"\n";
        i1.debug();
#endif
        thistag->seti1(g,value);
      }
      break;
    }

    case 9:{
      //a1
      char a1;
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag* thistag;
      if (newtag) {
        thistag=new Tag(tagname,ngroups,Tag::taga1);
        taglist.push_back(thistag);
      }
      else {
        thistag=taglist.back(); //take the last one
      }
      for (int g=0;g<ngroups;g++){
        dmfile.get(a1);
#ifdef DEBUG_detail
        std::cout<<"a a1 info number with value:"<<a1<<"\n";
#endif
        thistag->seta1(g,a1);
      }
      break;
    }

    case 10:{
      //i1
      Multibyte i1=Multibyte(1);
      //create a new tag and fill it out if inside tag14 only add to the current tag
      Tag* thistag;
      if (newtag) {
        thistag=new Tag(tagname,ngroups,Tag::tagi1);
        taglist.push_back(thistag);
      }
      else {
        thistag=taglist.back(); //take the last one
      }
      for (int g=0;g<ngroups;g++){
        i1.get(dmfile);
        const int value=i1.getvalue();
#ifdef DEBUG_detail
        std::cout<<"a i1 info number with value:"<<value<<"\n";
        i1.debug();
#endif
        thistag->seti1(g,value);
      }
      break;
    }


    case 15:{
      //group of data
      Multibyte i4=Multibyte(4);
      i4.get(dmfile); //should be zero
      if (i4.getvalue()!=0) throw ImageErr::load_error("Not a DM3 file:group of data should start with 0");
      i4.get(dmfile); //number in the group
      const int n=i4.getvalue();
#ifdef DEBUG_detail
      std::cout<<"a group with:"<<n<<"subtags\n";
#endif
      std::vector<int> subtype(n);
      for (int i=0;i<n;i++){
        i4.get(dmfile); //should be zero
        if (i4.getvalue()!=0) throw ImageErr::load_error("Not a DM3 file:a subtag should start with 0");
        i4.get(dmfile); //data type
        subtype[i]=i4.getvalue();
#ifdef DEBUG_detail
        std::cout<<"     member["<<i<<"] has data type"<<subtype[i]<<"\n";
#endif
      }
      int ngroups=0;
      if (inside14){
        Multibyte i4=Multibyte(4);
        i4.get(dmfile); //should be zero
        ngroups=i4.getvalue();
      }
      else{
        ngroups=1;
      }
      bool createnewtag=true;
      for (int g=0;g<ngroups;g++){
        //get the data
        for (int i=0;i<n;i++){
          //recursive call!!!
          readtype(dmfile,subtype[i],byteorder,tagname,taglist,false,createnewtag);
          createnewtag=false;
        }
      }
      break;
    }

    default:{
      throw ImageErr::load_error("Unknown type");
    }

  }


unique_ptr<Multispectrum> loadDM(const std::string& filename)
{
  if(filename.empty())
    throw std::runtime_error("loadDM called with an empty filename.");

  std::ifstream dmfile(filename.c_str(),std::ios::in|std::ios::binary);
  if(!dmfile)
    throw std::runtime_error("Can not open file for reading");

  //try to read the TOP SECRET DM 3 file structure
  //DM 3 needed a special file format to add tags to images
  //you wonder why people need TIFF ( t-a-g-g-e-d image format)
  //if GATAN is helping us out ;(
  //but anyway, we are stuck with it now...so let's open it

  Multibyte i4=Multibyte(4);
  Multibyte i4r=Multibyte(4,true); //reversed order 4byte
  Multibyte i2=Multibyte(2);
  Multibyte i2r=Multibyte(2,true); //reversed order 2byte

  //start reading header byte by byte
  i4.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4.debug(); //3
  #endif
  if (i4.getvalue()!=3) throw ImageErr::load_error("Not a DM3 file (1st header i4!=3)");

  i4.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4.debug(); //filelength-16
  #endif
  const int filelength=i4.getvalue()-16;
  if (filelength<=0) throw ImageErr::load_error("Not a DM3 file (2nd header filelength<=0)");

  i4.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4.debug(); //byte order
  #endif
  if (i4.getvalue()>1) throw ImageErr::load_error("Not a DM3 file (3rd header byte order>1)");
  const bool byteorder=i4.getvalue();

  i4r.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4r.debug(); //1?
  #endif
  if (i4r.getvalue()!=1) throw ImageErr::load_error("Not a DM3 file (4rd header !=1)");

  i2.get(dmfile);
  i2.debug(); //number of tags
  //const int ntags=i2.getvalue();

  vector<Tag> taglist;

  //start reading the tags
  char tag=1;

  while (tag!=0){ //tag=0 means end of file
    dmfile.get(tag);
    //read name of tag
    i2.get(dmfile);//length of name of tag
    const int ltname=i2.getvalue();
    #ifdef DEBUG_IMAGE_DETAIL
    std::cout<<"length of tagname:"<<ltname<<"\n";
    i2.debug();
    #endif

    if (ltname>LTMAX) throw ImageErr::load_error("An unreasonable long tag name encountered");
    char tname[LTMAX];
    //tname="";
    if (ltname>0) {
      dmfile.get(tname,ltname+1,0); //read name of the tag, make sure we don't stop for eol
    }
    else{
      strcpy(tname,"");
    }
    #ifdef DEBUG_IMAGE_DETAIL
    std::cout<<"tagname:"<<tname<<"\n";
    #endif


    //then depending on type of tag do
    switch (tag){
      case 21:{
          //a normal tag

          //read four % signs
          char c[4];
          dmfile.read(c,4); //read 4 chars and check wehter they are % signs
          if ((c[0]!='%')||(c[1]!='%')||(c[2]!='%')||(c[3]!='%')) throw ImageErr::load_error("Not a DM3 file:A tag should start with %%%%");

          i4.get(dmfile); //number of info numbers following not including data
          //const int nnum=i4.getvalue();
            i4.get(dmfile);
            const int type=i4.getvalue();
              switch(type){
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 15:{
                  //read the data of type type
                  readtype(dmfile,type,byteorder,tname,taglist,false,true);
                  break;
                  }
                case 20:{
                //multiple data or groups
                  i4.get(dmfile); //get type
                  const int subtype=i4.getvalue();
                  readtype(dmfile,subtype,byteorder,tname,taglist,true,true);
                break;
                }

              default:{
                 throw ImageErr::load_error("Not a DM3 format?: An unknown info type number was encountered");
                }
              }
        break; //of the case 21 switch
      }
      case 20:{
          //a directory of tags
          i4r.get(dmfile);
          i2.get(dmfile); //the number of tags in this directory
          #ifdef DEBUG_IMAGE_DETAIL
          std::cout<<"a tag directory\n";
          i4r.debug();
          i2.debug();
          #endif
          //create a tag with name and number of groups
          Tag* thistag=new Tag(tname,1,Tag::tagi2);
          thistag->seti2(0,i2.getvalue());
          taglist.push_back(thistag);
          break;
      }

       case 0:{
          //end of the file
          #ifdef DEBUG_IMAGE_DETAIL
          std::cout<<"reached the end\n";
          #endif
          break;
      }

      default:{
        throw ImageErr::load_error("Reached an unexpected tag identifier");
      }

    }
 #ifdef DEBUG_IMAGE_DETAIL
      //print the current taglist
      std::cout<<"The current tag list \n";
      std::cout<<"*********************\n";
      for (unsigned int j=0;j<taglist.size();j++){
        std::cout<<taglist[j]->getname()<<"\n";
      }
      std::cout<<"*********************\n";
      #endif
  }

  //energy loss axis is assumed to be x axis
  std::unique_ptr mymspec= new Multispectrum();
  mymspec->setfilename(filename);
  mymspec->set2D(b2D);
  if (b2D){
    mymspec->setstride(newwidth);
  }

  for (unsigned int y=0;y<nrofspectra;y++){
    //create a new spectrum for each y value
    Spectrum* currentspec=new Spectrum(epoints,energyoffset,energyscale);
    //create a name for this slice
    std::string result=filename;
    if (b2D){
      char indexstr1[10];
      const int id1=y/newwidth;
      sprintf(indexstr1,"%d",id1);
      char indexstr2[10];
      const int id2=y-id1*newwidth;
      sprintf(indexstr2,"%d",id2);
      result=result+" ["+indexstr1+','+indexstr2+']';
    }
    else{
      char indexstr[10];
      sprintf(indexstr,"%d",y);
      result=result+" ["+indexstr+']';
    }
    currentspec->name = result;
    //copy the spectrum into the currentspec
    for (unsigned int x=0;x<epoints;x++){
      unsigned int index=0;
      if (!flip){
        if (b2D){
          index=y+x*(nrofspectra);
        }
        else{
          index=x+width*y;
        }
      }
      else{
        index=x*height+y;
      }
      double cts=taglist[(imlist[currimage])]->getvalue(index);
      currentspec->setcounts(x,cts);
    }
    currentspec->setpppc(1.0);
    currentspec->initpoissonerror();
    //add this spectrum to the multispectrum
    mymspec->addspectrum(currentspec);
  }
  mymspec->setname(getname());

}

