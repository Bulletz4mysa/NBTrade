# makefile for external study dll
# copyright Bloomberg L.P. $Date: 2002/11/04 19:50:44 $
# $Header: /home/cvs2/external/makefile,v 1.11 2002/11/04 19:50:44 cmorgan Exp $

# CCwrap is a perl script which filters out known harmless warnings.
# Use -V to turn on the verbose flag (passed through to the compiler too)
#CC=../tools/CCwrap -V
CC=../tools/CCwrap
# Compile this way to regenerate the dependencies (alldeps.txt)
#CC=../tools/CCdepend
# Use the compiler directly if there seem to be problems with CCwrap.
#CC=CC

DEBUG=
INC=-I../include -I../mfcemu -I../win32emu -I../common -I../unicode -I. -I../atlemu
CCOPT=$(INC) -c -DSTRICT -Kpic -DBLPUNIX -DNO_BBUNICODE -DNOMINMAX -mt $(DEBUG)

all:	../lib ../lib/libblpGraphStudy.so
	@true

cvs:
	cvs update

OBJS=BLPStudyDll.o BLPStudyDll2.o BLPStudyDllTD.o StdAfx.o blpGraphStudyDll.o blpstudy.o dlldatax.o guids.o legacystudies.o UserCtrlObject.o fstring.o blpinterface.o demarkstudies.o TDMavgStudy.o TDREIStudy.o TDChannelStudy.o TDRangeProjectionStudy.o TDLinesStudy.o TDRetracementStudy.o TDSequentialComboStudy.o TDDemarkerStudy.o TDTrendFactorStudy.o TDPressureStudy.o CTDAntiDifferential.o CTDDifferential.o CTDReverseDifferential.o TDMavgEXStudy.o TDROCStudy.o TDDWaveStudy.o hurst.o trace.o CTDAlignment.o

#blpGraphStudyDll_i.o \


include alldeps.txt
include ../workshop_stuff

clean:
	rm -rf $(OBJS) *~ ;\
	rm -rf Templates.DB ;\
	rm -rf SunWS_cache

debug:
	$(MAKE) -e DEBUG='-g'

../lib/libblpGraphStudy.so: $(OBJS) mapfile ../lib
	$(CC) -G -o ../lib/libblpGraphStudy.so -M mapfile -L../lib -Kpic -lmfcemu -lwin32emu -mt $(OBJS)

BLPStudyDll.o: $(BLPSTUDYDLL_DEPS)
	$(CC) $(CCOPT) BLPStudyDll.cpp

BLPStudyDll2.o: $(BLPSTUDYDLL2_DEPS)
	$(CC) $(CCOPT) BLPStudyDll2.cpp

BLPStudyDllTD.o: $(BLPSTUDYDLLTD_DEPS)
	$(CC) $(CCOPT) BLPStudyDllTD.cpp

StdAfx.o: $(STDAFX_DEPS)
	$(CC) $(CCOPT) StdAfx.cpp

blpGraphStudyDll.o: $(BLPGRAPHSTUDYDLL_DEPS)
	$(CC) $(CCOPT) blpGraphStudyDll.cpp

blpGraphStudyDll_i.o: $(BLPGRAPHSTUDYDLL_DEPS)
	$(CC) $(CCOPT) blpGraphStudyDll_i.c

blpstudy.o: $(BLPSTUDY_DEPS)
	$(CC) $(CCOPT) blpstudy.cpp

dlldatax.o: $(DLLDATAX.DEPS)
	$(CC) $(CCOPT) dlldatax.c

guids.o: $(GUIDS_DEPS)
	$(CC) $(CCOPT) guids.cpp

legacystudies.o: $(LEGACYSTUDIES_DEPS)
	$(CC) $(CCOPT) legacystudies.cpp

UserCtrlObject.o: $(BLFLOAT_DEPS)
	$(CC) $(CCOPT) ../common/UserCtrlObject.cpp

fstring.o: $(FSTRING_DEPS)
	$(CC) $(CCOPT) ../common/fstring.cpp

blpinterface.o: $(BLPINTERFACE_DEPS)
	$(CC) $(CCOPT) ../common/blpinterface.cpp

demarkstudies.o: $(DEMARKSTUDIES_DEPS)
	$(CC) $(CCOPT) demarkstudies.cpp

TDMavgStudy.o: $(TDMAVGSTUDY_DEPS)
	$(CC) $(CCOPT) TDMavgStudy.cpp

TDREIStudy.o: $(TDREISTUDY_DEPS)
	$(CC) $(CCOPT) TDREIStudy.cpp

TDChannelStudy.o: $(TDCHANNELSTUDY_DEPS)
	$(CC) $(CCOPT) TDChannelStudy.cpp

TDRangeProjectionStudy.o: $(TDRANGEPROJECTIONSTUDY_DEPS)
	$(CC) $(CCOPT) TDRangeProjectionStudy.cpp

TDLinesStudy.o: $(TDLINESSTUDY_DEPS)
	$(CC) $(CCOPT) TDLinesStudy.cpp

TDRetracementStudy.o: $(TDRETRACEMENTSTUDY_DEPS)
	$(CC) $(CCOPT) TDRetracementStudy.cpp

TDSequentialComboStudy.o: $(TDSEQUENTIALCOMBOSTUDY_DEPS)
	$(CC) $(CCOPT) TDSequentialComboStudy.cpp

TDDemarkerStudy.o: $(TDDEMARKERSTUDY_DEPS)
	$(CC) $(CCOPT) TDDemarkerStudy.cpp

TDTrendFactorStudy.o: $(TDTRENDFACTORSTUDY_DEPS)
	$(CC) $(CCOPT) TDTrendFactorStudy.cpp

TDPressureStudy.o: $(TDPRESSURESTUDY_DEPS)
	$(CC) $(CCOPT) TDPressureStudy.cpp

CTDAntiDifferential.o: $(CTDANTIDIFFERENTIAL_DEPS)
	$(CC) $(CCOPT) CTDAntiDifferential.cpp

CTDDifferential.o: $(CTDDIFFERENTIAL_DEPS)
	$(CC) $(CCOPT) CTDDifferential.cpp

CTDReverseDifferential.o: $(CTDREVERSEDIFFERENTIAL_DEPS)
	$(CC) $(CCOPT) CTDReverseDifferential.cpp

TDMavgEXStudy.o: $(TDMAVGEXSTUDY_DEPS)
	$(CC) $(CCOPT) TDMavgEXStudy.cpp

TDROCStudy.o: $(TDROCSTUDY_DEPS)
	$(CC) $(CCOPT) TDROCStudy.cpp

TDDWaveStudy.o: $(TDDWAVESTUDY_DEPS)
	$(CC) $(CCOPT) TDDWaveStudy.cpp

CTDAlignment.o: $(TDALIGNMENTCPP_DEPS)
	$(CC) $(CCOPT) CTDAlignment.cpp

hurst.o: $(HURST_DEPS)
	$(CC) $(CCOPT) hurst.cpp

trace.o: $(TRACE_DEPS)
	$(CC) $(CCOPT) trace.cpp

../lib:
	mkdir ../lib
