1 CLEAR 400
2 DIM XA$(10),XB#(10),XC#(10),XD(10),XE$(10),XF(10),XG(10),XH(10),XI$(10),XJ$(10),XK(10),XL$(10),XM#(10)
3 SE$="":ZO=20
4 ON INTERVAL=5*(60-10*(PEEK(&H2B)\128)) GOSUB 3000
5 SCREEN 0:WIDTH 80
10 PRINT "RealADSB 0.2 for MSX"
20 PRINT "--------------------"
25 PRINT "Requires GR8NET cartridge for network access"
30 CFG$="A:REALADSB.CFG":DI$="KEWR":DA#=40.6924798:DO#=-74.1686868:DP$="192.168.1.153:5567"
40 PRINT "Loading configuration from "+CFG$+"..."
50 ON ERROR GOTO 120
60 OPEN CFG$ FOR INPUT AS #1
70 INPUT #1,DI$
80 INPUT #1,DA#
90 INPUT #1,DO#
100 INPUT #1,DP$
110 CLOSE
120 PRINT "OK"
125 PRINT "Airport ICAO code ["+DI$+"]: "
130 INPUT AI$
140 IF AI$="" THEN AI$=DI$
150 IF LEN(AI$)<4 THEN 125
155 PRINT "Your latitude ["+STR$(DA#)+"]: "
160 INPUT LA#
170 IF LA#=0 THEN LA#=DA#
175 PRINT "Your longitude ["+STR$(DO#)+"]: "
180 INPUT LO#
190 IF LO#=0 THEN LO#=DO#
195 PRINT "adsb_hub3 IP:port ["+DP$+"]: "
200 INPUT IP$
210 IF IP$="" THEN IP$=DP$
220 PRINT "Saving configuration to "+CFG$+"..."
230 OPEN CFG$ FOR OUTPUT AS #1
240 PRINT #1,AI$
250 PRINT #1,STR$(LA#)
260 PRINT #1,STR$(LO#)
270 PRINT #1,IP$
280 CLOSE
290 PRINT "OK"
500 OPEN "GRP:" AS #1
510 REM 512 x 212 16 colours
520 SCREEN 7,2:GOSUB 10000
530 COLOR 15,4,4:CLS
540 REM Draw radar
550 LINE (256-210,106)-(256+210,106),15
560 LINE (256,2)-(256,210),15
570 CIRCLE (256,106),105
580 CIRCLE (256,106),210
590 REM Left top corner
600 PSET(2,2),0
610 PRINT #1,"AIRPORT: "+AI$
620 PSET(2,12),0
630 PRINT #1,STR$(LA#)
640 PSET(2,22),0
650 PRINT #1,STR$(LO#)
660 REM Right bottom corner
670 GOSUB 2000
690 PSET(435,182),0
700 PRINT #1,"L - List"
710 PSET(435,192),0
720 PRINT #1,"M - METAR"
730 PSET(435,202),0
740 PRINT #1,"Q - Quit"
750 REM Right top corner
760 PSET(365,2),0
770 PRINT #1,IP$
780 IF LEN(SE$)>0 THEN GOSUB 4000
790 GOSUB 3000
800 INTERVAL ON
1000 A$=INKEY$
1010 REM Test keys M - show METAR Q - exit
1020 IF A$="L" OR A$="l" THEN 6000
1030 IF A$="M" OR A$="m" THEN 5000
1040 IF A$>CHR$(47) AND A$<CHR$(58) THEN SI=ASC(A$)-48:GOSUB 4000 ELSE 1050
1050 IF A$=CHR$(30) AND ZO>5 THEN ZO=ZO/2:GOSUB 2000 ELSE 1060
1060 IF A$=CHR$(31) AND ZO<160 THEN ZO=ZO*2:GOSUB 2000 ELSE 1070
1070 IF A$<>"Q" AND A$<>"q" THEN 1000
1080 INTERVAL OFF
1090 END
2000 REM Show change of zoom level
2010 PSET(435,172),0
2020 PRINT #1,"R:"+STR$(ZO)+"mi  "
2030 REM Zoom coefficient is number of pixel per 1/100th degree for X(longitude) and Y(latitude)
2040 ZX#=69.172*COS(LA#*3.14159/180)*1.05/CDBL(ZO)
2050 ZY#=69*1.05/CDBL(ZO)
2060 RETURN
3000 REM Get data from adsb_hub3 http://IP:5567/a
3010 REM ADSBHUB$="A5D570|40.782088|-74.169950|5875|UAL1496|202|277|64|A320|N475UA|3373|UAL|6.19"
3020 NA=0:GET TIME TI$
3030 PSET(405,12),0:PRINT #1,TI$
3040 URL$="http://"+IP$+"/a?lat="+STR$(LA#)+"&lon="+STR$(LO#)+"&num=10"
3050 OPEN "HTTPA:URL$" AS #1
3060 IF EOF(1) THEN CLOSE #1:GOTO 3300
3070 LINE INPUT #1, ADSBHUB$
3080 X=1:L=0:C=LEN(ADSBHUB$)
3090 N=INSTR(X,ADSBHUB$,"|"):IF N=0 AND X<C+1 THEN N=C+1
3100 K$=MID$(ADSBHUB$,X,N-X)
3110 IF LEN(K$)=0 THEN 3200
3120 IF L=0 THEN XA$(NA)=K$
3130 IF L=1 THEN XB#(NA)=(VAL(K$)*100.0-LA#*100.0)
3140 IF L=2 THEN XC#(NA)=(VAL(K$)*100.0-LO#*100.0)
3150 IF L=3 THEN XD(NA)=VAL(K$)
3160 IF L=4 THEN XE$(NA)=K$
3170 IF L=5 THEN XF(NA)=VAL(K$)
3180 IF L=6 THEN XG(NA)=VAL(K$)
3190 IF L=7 THEN XH(NA)=VAL(K$)
3200 IF L=8 THEN XI$(NA)=K$
3210 IF L=9 THEN XJ$(NA)=K$
3220 IF L=10 THEN XK(NA)=VAL(K$)
3230 IF L=11 THEN IF LEN(K$)>8 THEN XL$(NA)=LEFT$(K$,8) ELSE XL$(NA)=K$
3240 IF L=12 THEN XM#(NA)=VAL(K$)
3250 X=N+1:L=L+1
3260 IF X<C+1 THEN 3090
3270 IF XM#(NA)<ZO THEN GOSUB 3500
3280 NA=NA+1
3290 GOTO 3060 
3300 RETURN
3500 REM Draw airplane and digit for selection
3510 HE=0:IF XF(NA)>44 AND XF(NA)<136 THEN HE=1
3520 IF XF(NA)>135 AND XF(NA)<226 THEN HE=2
3530 IF XF(NA)>225 AND XF(NA)<316 THEN HE=3
3540 HC=15:IF SE$=XA$(NA) THEN HC=10
3550 PUT SPRITE NA,(120+INT(XC#(NA)*ZX#),98-INT(XB#(NA)*ZY#)),HC,HE
3560 PUT SPRITE NA+4,(120+INT(XC#(NA)*ZX#),98-INT(XB#(NA)*ZY#)),10,NA+4
3570 RETURN
4000 REM Airplane selected, index in SI
4010 SE$=XA$(SI)
4020 PSET (2,162),0:PRINT #1,XE$(SI)
4030 PSET (2,172),0:PRINT #1,XI$(SI)
4040 PSET (2,182),0:PRINT #1,XJ$(SI)
4050 PSET (2,192),0:PRINT #1,STR$(XD(SI))+"ft "+STR$(XH(SI))
4060 PSET (2,202),0:PRINT #1,STR$(XG(SI))+"kts"
4100 RETURN
5000 REM METAR
5010 INTERVAL OFF
5020 FOR XA=0 TO 9:PUT SPRITE XA,(-32,-32),15,0:NEXT XA
5030 COLOR 10,1,4
5040 LINE (20,20)-(492,192),1,BF
5050 PSET(30,25),0
5060 PRINT #1,"METAR for "+AI$
5070 LINE (30,35)-(482,35),10
5080 REM Load METAR from http://tgftp.nws.noaa.gov/data/observations/metar/stations/ICAO.TXT
5090 REM METAR$="2022/04/09 00:51"+CHR$(13)+CHR$(10)+"KEWR 090051Z 17006KT 10SM FEW060 14/M01 A2968 RMK AO2 PK WND 30028/1340 SLP088 T00891044"
5100 URL$="http://tgftp.nws.noaa.gov:80/data/observations/metar/stations/"+AI$+".TXT"
5110 OPEN "HTTPA:URL$" AS #1
5120 IF EOF(1) THEN CLOSE #1:GOTO 5240
5130 LINE INPUT #1, METAR$
5140 X=1:L=0:C=LEN(METAR$)
5150 N=INSTR(X,METAR$,CHR$(13)):IF N=0 AND X<C THEN N=C
5160 REM Loop for long (>50) lines
5170 FOR M=0 TO INT((N-X)/50)
5180 PSET(30,40+L*10),0
5190 XL=N-X:IF XL>50 THEN XL=50
5200 PRINT #1,MID$(METAR$,X,XL)
5210 X=X+XL:L=L+1
5200 NEXT M
5220 X=N+2
5230 IF X<C THEN 5150
5240 B$=INKEY$
5250 IF B$=CHR$(27) THEN 530 
5260 GOTO 5240
6000 REM List
6010 INTERVAL OFF
6020 FOR XA=0 TO 9:PUT SPRITE XA,(-32,-32),15,0:NEXT XA
6030 COLOR 10,1,4
6040 LINE (20,20)-(492,192),1,BF
6050 PSET(30,25),0
6060 PRINT #1,"Nearest airplanes"
6070 LINE (30,35)-(482,35),10
6080 PSET(30,43),0:PRINT #1,"Callsign  Type  Tail      Alt,ft     Speed,kts  Dist,mi"
6090 FOR N=0 TO NA-1
6100 PSET(30,55+N*12),0:PRINT #1,XE$(N)
6110 PSET(110,55+N*12),0:PRINT #1,XI$(N)
6120 PSET(158,55+N*12),0:PRINT #1,XJ$(N)
6130 PSET(230,55+N*12),0:PRINT #1,STR$(XD(N))+"/"+STR$(XH(N))
6140 PSET(342,55+N*12),0:PRINT #1,STR$(XG(N))
6150 PSET(422,55+N*12),0:PRINT #1,STR$(XM#(N))
6160 NEXT N
6230 B$=INKEY$
6240 IF B$=CHR$(27) THEN 530 
6250 GOTO 6230
10000 REM Load sprites https://www.msx.org/wiki/BASE()
10010 S=BASE(39)
10020 READ R$: IF R$="*" THEN RETURN ELSE VPOKE S,VAL("&H"+R$):S=S+1:GOTO 10020
10030 REM Sprite 16x16 airplane up
10040 DATA 00,01,01,01,01,01,07,1F,7F,79,01,01,01,03,07,00
10050 DATA 00,80,80,80,80,80,E0,F8,FE,9E,80,80,80,C0,E0,00
10060 REM Sprite 16x16 airplane right
10070 DATA 00,03,03,03,03,41,61,7F,7F,61,41,03,03,03,03,00
10080 DATA 00,00,00,80,80,C0,C0,FE,FE,C0,C0,80,80,00,00,00
10090 REM Sprite 16x16 airplane down
10100 DATA 00,07,03,01,01,01,79,7F,1F,07,01,01,01,01,01,00
10110 DATA 00,E0,C0,80,80,80,9E,FE,F8,E0,80,80,80,80,80,00
10120 REM Sprite 16x16 airplane left
10130 DATA 00,00,00,01,01,03,03,7F,7F,03,03,01,01,00,00,00
10140 DATA 00,C0,C0,C0,C0,82,86,FE,FE,86,82,C0,C0,C0,C0,00
10150 REM Slot 0
10160 DATA E0,A0,A0,A0,E0,00,00,00,00,00,00,00,00,00,00,00
10170 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10180 REM Slot 1
10190 DATA 40,C0,40,40,E0,00,00,00,00,00,00,00,00,00,00,00
10200 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10210 REM Slot 2
10220 DATA E0,20,E0,80,E0,00,00,00,00,00,00,00,00,00,00,00
10230 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10240 REM Slot 3
10250 DATA E0,20,E0,20,E0,00,00,00,00,00,00,00,00,00,00,00
10260 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10270 REM Slot 4
10280 DATA A0,A0,E0,20,20,00,00,00,00,00,00,00,00,00,00,00
10290 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10300 REM Slot 5
10310 DATA E0,80,E0,20,E0,00,00,00,00,00,00,00,00,00,00,00
10320 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10330 REM Slot 6
10340 DATA E0,80,E0,A0,E0,00,00,00,00,00,00,00,00,00,00,00
10350 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10360 REM Slot 7
10370 DATA E0,20,20,40,40,00,00,00,00,00,00,00,00,00,00,00
10380 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10390 REM Slot 8
10400 DATA E0,A0,E0,A0,E0,00,00,00,00,00,00,00,00,00,00,00
10410 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10420 REM Slot 9
10430 DATA E0,A0,E0,20,E0,00,00,00,00,00,00,00,00,00,00,00
10440 DATA 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
10450 DATA *