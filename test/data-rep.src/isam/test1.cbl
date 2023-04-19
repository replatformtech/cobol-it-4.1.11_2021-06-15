       IDENTIFICATION DIVISION.
       PROGRAM-ID. test1. 
       ENVIRONMENT DIVISION.
       CONFIGURATION SECTION.
       SPECIAL-NAMES.
            DECIMAL-POINT  COMMA.
       INPUT-OUTPUT SECTION.
       FILE-CONTROL.      

            SELECT STOCKCLI1
                   ASSIGN TO "stockcli_idx"
                   ORGANIZATION INDEXED
                   ACCESS MODE DYNAMIC
                   RECORD KEY STK1-INDEX1
                   ALTERNATE RECORD KEY STK1-INDEX2 WITH DUPLICATES
                   ALTERNATE RECORD KEY STK1-INDEX3 = STK1-XCLI
                                                      STK1-XART
                                                      STK1-XPOR
                                                      STK1-XNO
                   ALTERNATE RECORD KEY STK1-INDEX4 WITH DUPLICATES
                   LOCK MODE AUTOMATIC
                   FILE STATUS STK1-STAT.

     **                                                                        #
      *                                                                        #
       DATA DIVISION.
       FILE SECTION.

       FD  STOCKCLI1
           RECORD 143.
       01  E-STOCKCLI1.
	   03 STK1-INDEX1.
              05 STK1-XCLI.
                 07 STK1-CLIENT   PIC 9(4).
              05 STK1-XPOR.
                 07 STK1-POR      PIC 9(4).
              05 STK1-XART.
                 07 STK1-ART      PIC 9(4).
              05 STK1-XNO.
                 07 STK1-NO       PIC 9(4).
           03 STK1-BAR0           PIC 9(2).
	   03 STK1-INDEX2.
	      05 STK1-BARRE	  PIC 9(10).
              05 STK1-BAR                     REDEFINES STK1-BARRE.
                 07 STK1-BARS     PIC 9(2).
                 07 STK1-BARNO    PIC 9(8).
	   03 STK1-INDEX4.
              05 STK1-PUCE        PIC X(16).
              05 STK1-PUCEZ                   REDEFINES STK1-PUCE.
                 07 STK1-PUCEZ1   PIC X(6).
                 07 STK1-PUCEZ2   PIC X(10).
              05 STK1-PUCEY                   REDEFINES STK1-PUCE.
                 07 STK1-PUCEY1   PIC X(8).
                 07 STK1-PUCEY2   PIC X(8).
           03 FILLER              PIC X(14).
           03 STK1-TAILLE         PIC X(2).
	   03 STK1-DESCRIP1       PIC X(24).
           03 STK1-ORIGINE        PIC 9(4).
           03 STK1-PRE.
              05 STK1-9PRE        PIC 9.
           03 STK1-INVENTAIRE     PIC X.
	   03 STK1-DESCRIP2       PIC X(2).
	   03 STK1-MAGASIN.
              05 STK1-MAGNO       PIC 9.
              05 STK1-MAGCASE     PIC X(4).
	   03 STK1-DESCRIP3       PIC X(46).
      *

     **
      *
       WORKING-STORAGE SECTION.
       01  FUNCT-KEY-BIDON        PIC X.
       
      *

       01 DIVERS.
          03 STK-STAT             PIC XX.
          03 STK1-STAT            PIC XX.
          03 STK1-STAT-REG REDEFINES STK1-STAT.
                05  Major PIC X.
                05  Minor PIC X.


      *                                                                        #
       01  WTRAV.
      *
           03 ERRFICH.
              05 TYPERR           PIC X(12).
              05 FICERR           PIC X(12).
              05 MSERR2           PIC X(9)    VALUE "Statut : ".
              05 CODERR           PIC X(6).
              05 FILLER           PIC X       VALUE SPACE.
              05 ETQERR           PIC X(12).
           03 XSTATUT.
              05 XST1             PIC X.
              05 XST2             PIC X.
              05 NST2             REDEFINES XST2    PIC 9(2) COMP.
           03 RSTATUT             REDEFINES XSTATUT PIC 9(4) COMP.
           03 STS-GEN             REDEFINES XSTATUT.
              05 STS-G1           PIC X.
              05 STS-G2           PIC X.
              05 STS-G9           REDEFINES STS-G2  PIC 9(2) COMP.
           03 AFFST.
              05 STAT1            PIC X.
              05 ST-FIL           PIC X       VALUE "/".
              05 STAT2            PIC 9(4).
          03 SKB.
             05 SKB1        PIC X.
             05 SKB2        PIC 99 COMP.
             05 SKB3        PIC X.
             05 FILLER      PIC X.
          03 SKB4           PIC 9(4) VALUE 0.

          03 FLAG           PIC 99 COMP VALUE 1.

          03 ADISP.
             05 ADP1        PIC 99 COMP VALUE 1.
             05 ADP2        PIC X       VALUE "1".
             05 ADP3        PIC 99 COMP VALUE 0.
             05 ADP4        PIC 99 COMP VALUE 99.

          03 KBHIT.
             05 FILLER      PIC X(6)  VALUE "kbhit".
             05 FILLER      PIC X     VALUE X"00".
      *

           03 REP                 PIC X.
           03 REP-SAI             PIC X.
           03 REP-SKB             PIC 99.
           03 MESS                PIC X(80)   VALUE SPACE.

           03 ANCLI               PIC 9(4)    VALUE ZERO.
           03 ANPOR               PIC 9(4)    VALUE ZERO.

           03 NVCLI               PIC 9(4)    VALUE ZERO.
           03 NVPOR               PIC 9(4)    VALUE ZERO.

           03 NBCPT               PIC 9(2).
           03 CPT                 PIC 9(5).


           03 ECRAN.
            04 ECRAN-E1.
              05 E1.
                 07 E1-1          PIC X(16)   VALUE "MDFCLIPOR-10".
                 07 E1-2          PIC X(64)   VALUE 
                     "MODIFICATION Code CLIENT-PORTEUR Fichiers de GVT".
              05 E1.
                 07 E1-1          PIC X(80)   VALUE SPACES.
              05 E3.
                 07 E3-1          PIC X(80)   VALUE SPACES.
              05 E4.
                 07 E4-1          PIC X(80)   VALUE SPACES.
            04 ECRAN-E5.
              05 E5.
                 07 E5-1          PIC X(80)   VALUE SPACE.
              05 E6.
                 07 E6-1          PIC X(80)   VALUE SPACES.
              05 E7.
                 07 E7-1          PIC X(80)   VALUE SPACES.
              05 E8.
                 07 E8-1          PIC X(80)   VALUE SPACES.
              05 E9.
                 07 E9-1          PIC X(80)   VALUE SPACES.
              05 E10.
                 07 E10-1         PIC X(80)   VALUE SPACES.
              05 E11.
                 07 E11-1         PIC X(80)   VALUE SPACES.
              05 E12.
                 07 E12-1         PIC X(80)   VALUE SPACES.
              05 E13.
                 07 E13-1         PIC X(80)   VALUE SPACES.
              05 E14.
                 07 E14-1         PIC X(15)   VALUE SPACES.
                 07 E14-2         PIC X(11)   VALUE "Stockcli : ".
                 07 E14-3         PIC X(54)   VALUE SPACES.
              05 E15.
                 07 E15-1         PIC X(80)   VALUE SPACES.
              05 E16.
                 07 E16-1         PIC X(80)   VALUE SPACES.
            04 ECRAN-E17.
              05 E17.
                 07 E17-1         PIC X(43)   VALUE
                    "ANCIEN Code".
                 07 E17-2         PIC X(37)   VALUE
                    "NOUVEAU Code".
              05 E18.
                 07 E18-1         PIC X(43)   VALUE
                    "Client =".
                 07 E18-2         PIC X(37)   VALUE
                    "Client =".
              05 E19.
                 07 E19-1         PIC X(43)   VALUE
                    "Porteur=".
                 07 E19-2         PIC X(37)   VALUE
                    "Porteur=".
              05 E20.
                 07 E20-1         PIC X(80)   VALUE SPACE.
              05 E21.
                 07 E21-1         PIC X(80)   VALUE SPACE.

      *
       01  DTHRSYS.
           03 DATSYS.
              05 AA            PIC 99.
              05 MM            PIC 99.
              05 JJ            PIC 99.
           03 WDAT.
              05 RJJ              PIC 99.
              05 RF1              PIC X       VALUE "/".
              05 RMM              PIC 99.
              05 RF2              PIC X       VALUE "/".
              05 RAA              PIC 99.
      *
           03 HEUSYS.
              05 HEUHMS.
                 07 HH            PIC 99.
                 07 MN            PIC 99.
                 07 SS            PIC 99.
              05 CC               PIC 99.
           03 WHEU.
              05 RHH              PIC 99.
              05 RH1              PIC X       VALUE ":".
              05 RMN              PIC 99.
              05 RH2              PIC X       VALUE ":".
              05 RSS              PIC 99.
      *
      *
       01  LINK-DTHRSYS.
           03 LK-DTSYS       PIC 9(6).
           03 LK-RDTSYS      REDEFINES LK-DTSYS.
              05 LK-DTSYS-AA PIC 9(2).
              05 LK-DTSYS-MM PIC 9(2).
              05 LK-DTSYS-JJ PIC 9(2).
           03 LK-HRSYS       PIC 9(6).
           03 LK-RHRSYS      REDEFINES LK-HRSYS.
              05 LK-HRSYS-HH PIC 9(2).
              05 LK-HRSYS-MM PIC 9(2).
              05 LK-HRSYS-SS PIC 9(2).
      *
       01 ModeFlag PIC 9 VALUE 0.
         88 DoCount    VALUE 0.
         88 DoDelete   VALUE 1, 2.
         88 DoWrite    VALUE 2.

     **
      *
       PROCEDURE DIVISION.
       SEQ-000.

           MOVE 1 TO ADP1.
      *
           MOVE SPACE TO FUNCT-KEY-BIDON.
      *

           OPEN I-O   STOCKCLI1.
           IF Major OF  STK1-STAT-REG NOT = '0'
              DISPLAY "open  STOCKCLI1 ERROR " STK1-STAT 
              GO TO FIN12.

       SEQ-001.
           INITIALIZE STK1-INDEX1.
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE 8888 TO NVCLI.
           MOVE 7777 TO NVPOR.
       SEQ-040.
      * Count
           MOVE ZERO  TO CPT.
           MOVE 0 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * rewrite ->
           MOVE ZERO  TO CPT.
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE 8888 TO NVCLI.
           MOVE 7777 TO NVPOR.
           MOVE 2 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * Count
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE ZERO  TO CPT.
           MOVE 0 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * rewrite  <-
           MOVE ZERO  TO CPT.
           MOVE 0432 TO NVCLI.
           MOVE 0001 TO NVPOR.
           MOVE 8888 TO ANCLI.
           MOVE 7777 TO ANPOR.
           MOVE 2 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * Count
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE ZERO  TO CPT.
           MOVE 0 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * rewrite ->
           MOVE ZERO  TO CPT.
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE 8888 TO NVCLI.
           MOVE 7777 TO NVPOR.
           MOVE 2 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * Count
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE ZERO  TO CPT.
           MOVE 0 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * rewrite  <-
           MOVE ZERO  TO CPT.
           MOVE 0432 TO NVCLI.
           MOVE 0001 TO NVPOR.
           MOVE 8888 TO ANCLI.
           MOVE 7777 TO ANPOR.
           MOVE 2 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * Count
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE ZERO  TO CPT.
           MOVE 0 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.
      * delete
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE ZERO  TO CPT.
           MOVE 1 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.

      * Count
           MOVE 0432 TO ANCLI.
           MOVE 0001 TO ANPOR.
           MOVE ZERO  TO CPT.
           MOVE 0 TO ModeFlag.
           PERFORM SEQ-160 THRU SEQ-166.

           GO TO FINI.
        

       SEQ-160.
           MOVE ZERO  TO STK1-INDEX1.
           MOVE ANCLI TO STK1-CLIENT.
           MOVE ANPOR TO STK1-POR.
           START STOCKCLI1 KEY NOT < STK1-INDEX1
                   INVALID KEY DISPLAY "Start INV " STK1-STAT
                    GO TO SEQ-166.

           IF STK1-STAT NOT = "00"
               DISPLAY "START ERROR " STK1-STAT 
               GO TO FINI.

       SEQ-161.
           READ STOCKCLI1 NEXT AT END 
                DISPLAY "READ NEXT end " STK1-STAT 
                GO TO SEQ-166.
           IF STK1-STAT NOT = ZERO
               MOVE STK1-STAT TO XSTATUT
               DISPLAY "READ ERROR " CPT  " " STK1-INDEX1  " " STK1-STAT   
               GO TO FINI.
           UNLOCK STOCKCLI1.
           IF STK1-CLIENT NOT = ANCLI OR
               STK1-POR    NOT = ANPOR  
                  GO TO SEQ-166.
           
           ADD 1 TO CPT.
           IF DoDelete THEN
               DELETE STOCKCLI1
                      INVALID KEY 
                       DISPLAY "DELETE" STK1-STAT 
                      GO TO FINI
               IF STK1-STAT NOT = "00" then
                  DISPLAY "DELETE" STK1-STAT 
                  GO TO FINI
               END-IF 
           END-IF.
           IF DoWrite THEN
                 MOVE NVCLI TO STK1-CLIENT
                 MOVE NVPOR TO STK1-POR
                 WRITE E-STOCKCLI1
                 INVALID KEY DISPLAY STK1-STAT AT 1630
                            DISPLAY "WRITE" STK1-STAT 
                             GO TO FINI
               IF STK1-STAT NOT = "00" then
                  DISPLAY "WRITE" STK1-STAT 
                  GO TO FINI
               END-IF 
           END-IF.

           IF DoDelete THEN
               GO TO SEQ-160
           ELSE
               GO TO SEQ-161
           END-IF.




       SEQ-166.
           DISPLAY  CPT.
 

     **
      *
       FINI.
           MOVE SPACE TO REP.
       FIN11.
           CLOSE STOCKCLI1.
       FIN12.

       FIN13.
           STOP RUN.





