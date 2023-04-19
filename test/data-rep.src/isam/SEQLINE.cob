       IDENTIFICATION DIVISION.
       PROGRAM-ID.    SEQLINE.
       ENVIRONMENT DIVISION.
       CONFIGURATION SECTION.
       SPECIAL-NAMES.
           DECIMAL-POINT IS COMMA. 
       INPUT-OUTPUT SECTION.
       FILE-CONTROL.
           SELECT SEQFILE  
                  ASSIGN TO SEQFILE
                  ORGANIZATION  IS LINE SEQUENTIAL
                  FILE STATUS   IS SEQ-STATUS.
      *****************************************************************
       DATA DIVISION.
       FILE SECTION.
       FD  SEQFILE.        
       01  REC-AREA          PIC X(80).
                        
       WORKING-STORAGE SECTION.     
       01 PGM PIC X(8) VALUE "LINESEQ".  
       01 TEST-RESULT.
           03 FILLER PIC XX.
              88 TEST-OK  VALUE 'OK'.
              88 TEST-KO  VALUE 'KO'.
       01 KO   PIC X(2) VALUE 'KO'.
       01 OK   PIC X(2) VALUE 'OK'.
       01 PGM-ACTION.         
           03  FILLER                  PIC X.
             88  ANONE                VALUE ' '.
             88  AREAD                VALUE '1'.
             88  AWRITE               VALUE '2'.
             88  AREWRITE             VALUE '3'.
       01 SEQ-STATUS.         
           03  FILLER                  PIC X.
             88  SEQ-OK                VALUE '0'.
             88  SEQ-EOF               VALUE '1'.
             88  SEQ-ERROR             VALUE '2' '3' '4' '9'.
           03  FILLER                   PIC X.
       01 REC-READ   PIC 9(4) VALUE 0.
       01 REC-WRITE  PIC 9(4) VALUE 0.
       01 REC-REWRITE  PIC 9(4) VALUE 0.
       01 NUM-PAGE     PIC 9(2) VALUE 0.
       01 NUM-REC     PIC 9(4) VALUE 0.
       01 TOTAL-REC-PAGE  PIC 9(4) VALUE  6.       
       01 TOTAL-NUM-PAGE  PIC 9(4) VALUE  3.              
       01 REC-HEADER.
          03 H-CHAR        PIC X VALUE SPACE.
          03 FILLER        PIC X(70) VALUE SPACE.
          03 FILLER        PIC X(6) VALUE 'PAGE  '.
          03 H-PAGE        PIC Z9.
          03 FILLER        PIC X VALUE SPACE.
       01 REC-TITLE.
          03 T-CHAR        PIC X VALUE SPACE.
          03 FILLER        PIC X(1) VALUE '|'.
          03 FILLER        PIC X(3) VALUE SPACE.
          03 T-NUM         PIC X(6).
          03 FILLER        PIC X(1) VALUE SPACE.
          03 FILLER        PIC X(2) VALUE '| '.
          03 T-REC         PIC X(59).
          03 FILLER        PIC X(6) VALUE SPACE.
          03 FILLER        PIC X(1) VALUE '|'.

       01 REC-DETAIL.
          03 D-CHAR        PIC X VALUE SPACE.
          03 FILLER        PIC X(1) VALUE '|'.
          03 FILLER        PIC X(3) VALUE SPACE.
          03 D-NUM         PIC ZZ.ZZZ.
          03 FILLER        PIC X(1) VALUE SPACE.
          03 FILLER        PIC X(2) VALUE '| '.
          03 D-REC         PIC X(59).
          03 FILLER        PIC X(6) VALUE SPACE.
          03 FILLER        PIC X(1) VALUE '|'.
       01 REC-TRAILER.
          03 TR-CHAR       PIC X VALUE SPACE.
          03 TR-LINE       PIC X(79).
       01 RESULT-MSG.
          03 FILLER     PIC X(1) VALUE "(".
          03 PGM-R      PIC X(8) VALUE SPACE.
          03 FILLER     PIC X(2) VALUE ") ".
          03 R-MSG      PIC X(40).
          03 R-RESULT   PIC X(35).
       01 AREA-MSG.
          03 FILLER     PIC X(1) VALUE "(".
          03 PGM-MSG    PIC X(8) VALUE SPACE.
          03 FILLER     PIC X(2) VALUE ") ".
          03 MSG    PIC X(25).
          03 REC-MSG    PIC ZZZ.ZZZ.ZZZ.
       01 ERROR-MSG.
          03 FILLER     PIC X(1) VALUE "(".
          03 PGM-ERR-MSG    PIC X(8) VALUE SPACE.
          03 FILLER     PIC X(2) VALUE ") ".
          03 ACTION-MSG    PIC X(25).
          03 STATUS-ERR-MSG    PIC X(2).
       01 IND-REC   PIC S9(4) COMP VALUE 1.
       01 TABLE-REC.          
          03 TABLE-ELEM.
             05 ROW-1  PIC X(40) VALUE
             '1st record                              '.   
             05 ROW-2  PIC X(40) VALUE
             '2nd record                              '.   
             05 ROW-3  PIC X(40) VALUE
             '3rd record                              '.   
             05 ROW-4  PIC X(40) VALUE
             '4th record                              '.   
             05 ROW-5  PIC X(40) VALUE
             '5th record                              '.   
             05 ROW-6  PIC X(40) VALUE
             '6th record                              '.   
             05 ROW-7  PIC X(40) VALUE
             '7th record                              '.   
             05 ROW-8  PIC X(40) VALUE
             '8th record                              '.   
             05 ROW-9  PIC X(40) VALUE
             '9th record                              '.   
             05 ROW-10  PIC X(40) VALUE
             '10th record                             '.   
             05 ROW-11  PIC X(40) VALUE
             '11th record                             '.   
             05 ROW-12  PIC X(40) VALUE
             '12th record                             '.   
             05 ROW-13  PIC X(40) VALUE
             '13h record                              '.   
             05 ROW-14  PIC X(40) VALUE
             '14th record                             '.   
             05 ROW-15  PIC X(40) VALUE
             '15th record                             '.   
             05 ROW-16  PIC X(40) VALUE
             '16th record                             '.   
             05 ROW-17  PIC X(40) VALUE
             '17th record                             '.   
             05 ROW-18  PIC X(40) VALUE
             '18th record                             '.   
             05 ROW-19  PIC X(40) VALUE
             '19th record                             '.   
             05 ROW-20  PIC X(40) VALUE
             '20th record                             '.   
          03 ROW-REC REDEFINES TABLE-ELEM PIC X(40) OCCURS 20.

      *****************************************************************
       PROCEDURE DIVISION.               
       MAIN SECTION.             
             MOVE all '-' to R-MSG
             PERFORM DISPLAY-RESULT             
             MOVE "Line Seqeuntial" TO R-MSG                               
             PERFORM DISPLAY-RESULT
             MOVE all '-' to R-MSG
             PERFORM DISPLAY-RESULT
         
      * writing test
             MOVE '1' TO PGM-ACTION
             PERFORM SEQ-OPEN-OUTPUT
             PERFORM SEQ-WRITE-TEST
             PERFORM SEQ-CLOSE

      * reading test 
             MOVE '2' TO PGM-ACTION
             PERFORM SEQ-OPEN-INPUT
             PERFORM SEQ-READ-TEST
             PERFORM SEQ-CLOSE
      * check 
             PERFORM SEQ-CHECK
      * quit 
             PERFORM QUIT-PROGRAM.
       SEQ-OPEN-INPUT SECTION.
           MOVE "Line seq. open input" to ACTION-MSG
           OPEN INPUT SEQFILE
           PERFORM SEQ-TEST-ACTION.           
       SEQ-OPEN-INPUT-EX. EXIT.
       SEQ-OPEN-OUTPUT SECTION.
             MOVE "Line seq. open output" to ACTION-MSG
             OPEN OUTPUT SEQFILE
             PERFORM SEQ-TEST-ACTION.
       SEQ-OPEN-OUTPUT-EX. EXIT.
       SEQ-READ-TEST SECTION.
             MOVE 0 TO REC-READ
             MOVE KO TO TEST-RESULT
             MOVE "Line seq. reading test is : " TO R-MSG     
             MOVE "Record read : " TO MSG     
             PERFORM SEQ-READ             
             PERFORM UNTIL SEQ-EOF
                   PERFORM SEQ-READ
             END-PERFORM        
             MOVE REC-READ TO REC-MSG
             MOVE OK TO TEST-RESULT             
             PERFORM DISPLAY-RESULT.
       SEQ-READ-TEST-EX. EXIT.
       SEQ-WRITE-TEST SECTION.             
             MOVE 0 TO REC-WRITE
             MOVE KO TO TEST-RESULT
             MOVE "Line sequential writing test is : " TO R-MSG                               
             PERFORM UNTIL NUM-PAGE >= TOTAL-NUM-PAGE
                   PERFORM WRITE-HEADER
                   MOVE 1 TO NUM-REC 
                   PERFORM UNTIL NUM-REC > TOTAL-REC-PAGE
                      PERFORM SEQ-WRITE
                   END-PERFORM 
                   PERFORM WRITE-TRAILER
             END-PERFORM        
             MOVE REC-WRITE TO REC-MSG
             MOVE "Record written : " TO MSG                  
             MOVE OK TO TEST-RESULT             
             PERFORM DISPLAY-RESULT.
       SEQ-WRITE-TEST-EX. EXIT.
       WRITE-HEADER SECTION.             
             MOVE NUM-PAGE TO H-PAGE             
             WRITE REC-AREA FROM REC-HEADER
             PERFORM SEQ-TEST-ACTION
             ADD 1 TO REC-WRITE
             PERFORM WRITE-TRAILER             
             MOVE 'NUM' TO T-NUM
             MOVE 'RECORD' TO T-REC             
             WRITE REC-AREA FROM REC-TITLE
             PERFORM WRITE-TRAILER
             PERFORM SEQ-TEST-ACTION
             ADD 1 TO REC-WRITE
             ADD 1 TO NUM-PAGE.
       WRITE-HEADER-EX. EXIT.
       WRITE-TRAILER SECTION.
             MOVE SPACE TO REC-TRAILER
             MOVE ALL '-' TO TR-LINE             
             WRITE REC-AREA FROM REC-TRAILER 
             PERFORM SEQ-TEST-ACTION
             ADD 1 TO REC-WRITE.
       WRITE-TRAILER-EX. EXIT.
       SEQ-READ SECTION.
             MOVE "Seq read record" to ACTION-MSG             
             READ SEQFILE             
             PERFORM SEQ-TEST-ACTION
             IF NOT SEQ-EOF 
                ADD 1 TO REC-READ                             
             END-IF.             
       SEQ-READ-EX. EXIT.
       SEQ-WRITE SECTION.
             MOVE "Seq write record" to ACTION-MSG             
             ADD 1 TO REC-WRITE
             ADD 1 TO NUM-REC
             MOVE ROW-REC(IND-REC) TO D-REC
             MOVE IND-REC TO D-NUM
             ADD 1 TO IND-REC
             WRITE REC-AREA FROM REC-DETAIL
             PERFORM SEQ-TEST-ACTION.
       SEQ-WRITE-EX. EXIT.
       SEQ-CLOSE SECTION.  
             MOVE "Seq close file" to ACTION-MSG
             CLOSE SEQFILE        
             PERFORM SEQ-TEST-ACTION.
       SEQ-CLOSE-EX. EXIT.
       SEQ-TEST-ACTION SECTION.
             IF SEQ-ERROR
                MOVE SEQ-STATUS TO STATUS-ERR-MSG 
                PERFORM DISPLAY-ERROR
                PERFORM DISPLAY-RESULT
                PERFORM DUMP-PROGRAM
             END-IF.                        
       SEQ-TEST-ACTION-EX. EXIT.     
       SEQ-CHECK SECTION.             
             MOVE SPACE TO TEST-RESULT
             MOVE ALL "-" TO R-MSG
             PERFORM DISPLAY-RESULT
             MOVE KO TO TEST-RESULT
             MOVE "Test on line sequential file is : " TO R-MSG                  
             IF REC-WRITE = REC-READ THEN
                MOVE OK TO TEST-RESULT   
             ELSE
                DISPLAY "Record Written " REC-WRITE  
                DISPLAY "Record Read " REC-READ
             END-IF
             PERFORM DISPLAY-RESULT.
       SEQ-CHECK-EX. EXIT.
       DISPLAY-RESULT SECTION.
      *       MOVE PGM TO PGM-MSG PGM-R
      *       DISPLAY AREA-MSG
      *       MOVE TEST-RESULT TO R-RESULT
      *       DISPLAY RESULT-MSG
             MOVE SPACE TO R-MSG 
             MOVE SPACE TO MSG.
       DISPLAY-RESULT-EX. EXIT.
       DISPLAY-ERROR SECTION.
             MOVE PGM TO PGM-ERR-MSG
             DISPLAY ERROR-MSG
             MOVE SPACE TO MSG.
       DISPLAY-ERROR-EX. EXIT.
       DUMP-PROGRAM SECTION.
             STOP RUN 1.
       DUMP-PROGRAM-EX. EXIT.
       QUIT-PROGRAM SECTION.
             DISPLAY "OK"
             GOBACK.
       QUIT-PROGRAM-EX. EXIT.
