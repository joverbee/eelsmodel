######################################################################
# Automatically generated by qmake (2.01a) Mon 11. May 15:43:47 2009
######################################################################

CONFIG+=exceptions
CONFIG+=rtti
QMAKE_CXXFLAGS_RELEASE+=-O3
QMAKE_CXXFLAGS_RELEASE-=-fno-exceptions
win32:RC_FILE=eelsmodelrc.rc
LIBS+=-lfftw3-3
LIBS+=-lgsl
LIBS+=-lgslcblas
QMAKE_CXXFLAGS+=-DGSL_DLL
QMAKE_CXXFLAGS+=-Ic:\gsl\include
LIBS+=-Lc:\gsl\lib
LIBS+=-Lc:\fftw_3_1
#LIBS+=-LC:\atlas\ATLAS3.8.0\WIN32P3\lib
LIBS+=-LC:\atlas\ATLAS3.8.0\WIN32\lib
LIBS+=-llapack
LIBS+=-lcblas
LIBS+=-lf77blas
LIBS+=-latlas
TEMPLATE = app
TARGET = eelsmodel
DEPENDPATH += . .metadata\.plugins
INCLUDEPATH += .

# Input
HEADERS += Atomchooser.h \
           bandedmatrix.h \
           chisquare.h \
           component.h \
           componentmaintenance.h \
           componentselector.h \
           cslice_iter.h \
           curvematrix.h \
           debug_new.h \
           detectorchooser.h \
           dieleclinear.h \
           dielecmodel.h \
           dos.h \
           doslifetime.h \
           doslifetimespline.h \
           drudelorentz.h \
           Drudeoptions.h \
           edgefile.h \
           eelsmodel.h \
           eelsmodelproject_private.h \
           energies.h \
           equalizer.h \
           eshift.h \
           expbackground.h \
           fftw++.h \
           fftw3.h \
           fileopener.h \
           fitter.h \
           fitter_dialog.h \
           fitterchooser.h \
           fixedpattern.h \
           fowler.h \
           gaussian.h \
           gdos.h \
           GDOSoptions.h \
           getfilename.h \
           getgraphptr.h \
           gettopspectrum.h \
           graph.h \
           hello.h \
           hsedge.h \
           image.h \
           imagedisplay.h \
           integerinput.h \
           interactive_parameter.h \
           invert_matrix.hpp \
           kedge.h \
           kedgerel.h \
           ledge.h \
           levenbergmarquardt.h \
           line.h \
           logpoly.h \
           lorentz.h \
           lsqfitter.h \
           mendeleev.h \
           menu.h \
           mlfitter.h \
           mlfitter_gsl.h \
           model.h \
           monitor.h \
           monitorchooser.h \
           mscatter.h \
           mscatterexpfft.h \
           mscatterfft.h \
           multibyte.h \
           multispectrum.h \
           mycomponents.h \
           parameter.h \
           pearsonIV.h \
           plasmon.h \
           powerlaw.h \
           pseudovoigt.h \
           realinput.h \
           saysomething.h \
           slice_iter.h \
           spectrum.h \
           spinsplitedge.h \
           spinsplitedgegap.h \
           spinsplitedgehyd.h \
           tag.h \
           wlsqfitter.h \
           icons/locked.xpm \
           icons/unlock.xpm \
           icons/linked.xpm \
           icons/add.xpm \
           icons/remove.xpm \
           icons/info.xpm \
           icons/ok.xpm \
           icons/monitor.xpm \
           icons/atomwiz.xpm \
           welcomescreen/eelsmodel_3_0.xpm \
           licence.txt \
           icons/filesave.xpm \
           icons/fileopen.xpm \
           icons/filenew.xpm \
           icons/previous.xpm \
           icons/rectangle.xpm \
           icons/zoom.xpm \
           icons/gohome.xpm \
           icons/connect_established.xpm
SOURCES += Atomchooser.cpp \
           chisquare.cpp \
           component.cpp \
           componentmaintenance.cpp \
           componentselector.cpp \
           cslice_iter.cpp \
           curvematrix.cpp \
           debug_new.cpp \
           detectorchooser.cpp \
           dieleclinear.cpp \
           dielecmodel.cpp \
           dos.cpp \
           doslifetime.cpp \
           doslifetimespline.cpp \
           drudelorentz.cpp \
           Drudeoptions.cpp \
           edgefile.cpp \
           eelsmodel.cpp \
           equalizer.cpp \
           eshift.cpp \
           expbackground.cpp \
           fftw++.cpp \
           fileopener.cpp \
           fitter.cpp \
           fitter_dialog.cpp \
           fitterchooser.cpp \
           fixedpattern.cpp \
           fowler.cpp \
           gaussian.cpp \
           gdos.cpp \
           GDOSoptions.cpp \
           getfilename.cpp \
           getgraphptr.cpp \
           gettopspectrum.cpp \
           graph.cpp \
           hello.cpp \
           hsedge.cpp \
           image.cpp \
           imagedisplay.cpp \
           integerinput.cpp \
           interactive_parameter.cpp \
           kedge.cpp \
           kedgerel.cpp \
           ledge.cpp \
           levenbergmarquardt.cpp \
           line.cpp \
           logpoly.cpp \
           lorentz.cpp \
           lsqfitter.cpp \
           main.cpp \
           menu.cpp \
           mlfitter.cpp \
           mlfitter_gsl.cpp \
           model.cpp \
           monitor.cpp \
           monitorchooser.cpp \
           mscatter.cpp \
           mscatterexpfft.cpp \
           mscatterfft.cpp \
           multibyte.cpp \
           multispectrum.cpp \
           parameter.cpp \
           pearsonIV.cpp \
           plasmon.cpp \
           powerlaw.cpp \
           pseudovoigt.cpp \
           realinput.cpp \
           saysomething.cpp \
           slice_iter.cpp \
           spectrum.cpp \
           spinsplitedge.cpp \
           spinsplitedgehyd.cpp \
           tag.cpp \
           wlsqfitter.cpp
