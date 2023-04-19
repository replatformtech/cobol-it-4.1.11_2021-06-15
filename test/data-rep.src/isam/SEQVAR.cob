       IDENTIFICATION DIVISION.
       PROGRAM-ID.    SEQVAR.
       ENVIRONMENT DIVISION.
       CONFIGURATION SECTION.
       SPECIAL-NAMES.
           DECIMAL-POINT IS COMMA. 
       INPUT-OUTPUT SECTION.
       FILE-CONTROL.
           SELECT SEQFILE  ASSIGN "seqvar_data"
                  ORGANIZATION  IS SEQUENTIAL
                  ACCESS MODE   IS SEQUENTIAL 
                  FILE STATUS   IS SEQ-STATUS.
      *****************************************************************
       DATA DIVISION.
       FILE SECTION.
       FD  SEQFILE     
           RECORD IS VARYING IN SIZE
           FROM 60 TO 80 CHARACTERS
           DEPENDING ON LENREC.   
       01  REC-AREA.
            03 REC-9         PIC 9(4).
            03 REC-X.
              05  REC-X-1         PIC X(15).
              05  REC-X-2         PIC X(15).
            03 REC-COMP      PIC 9(4) COMP.
            03 REC-COMP-3    PIC 9(8) COMP-3.            
            03 FILLER        PIC X(9).                       
      * variable record structure 
            03 REC-VAR-LEN    PIC 9(5).                        
            03 REC-VAR-X      PIC X(25).                        
       WORKING-STORAGE SECTION.     
       01 PGM PIC X(8) VALUE "SEQVAR".  
       01 LENREC PIC S9(4) COMP.
       01 VAR-GIV PIC 9(9) VALUE 0.
       01 VAR-REM PIC 9(9) VALUE 0.
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
       01 TOTAL-REC-WRITE  PIC 9(9) VALUE 10.       
       01 RESULT-MSG.
          03 FILLER     PIC X(1) VALUE "(".
          03 PGM-R    PIC X(8) VALUE SPACE.
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
          03 ACTION-MSG    PIC X(20).
          03 STATUS-ERR-MSG    PIC X(2).

      *****************************************************************
       PROCEDURE DIVISION.
       MAIN SECTION.  
             MOVE all '-' to R-MSG
             PERFORM DISPLAY-RESULT             
             MOVE "SEQUENTIAL VARYING" TO R-MSG                               
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
      * rewriting test on varying is unsupported
      *       MOVE '3' TO PGM-ACTION
      *       PERFORM SEQ-OPEN-I-O
      *       PERFORM SEQ-READ-TEST
      *       PERFORM SEQ-CLOSE
      * check 
             PERFORM SEQ-CHECK
      * quit 
             PERFORM QUIT-PROGRAM.
       SEQ-OPEN-INPUT SECTION.
           MOVE "Seq open input" to ACTION-MSG
           OPEN INPUT SEQFILE
           PERFORM SEQ-TEST-ACTION.           
       SEQ-OPEN-INPUT-EX. EXIT.
       SEQ-OPEN-OUTPUT SECTION.
             MOVE "Seq open output" to ACTION-MSG
             OPEN OUTPUT SEQFILE
             PERFORM SEQ-TEST-ACTION.
       SEQ-OPEN-OUTPUT-EX. EXIT.
       SEQ-OPEN-I-O SECTION.
           MOVE "Seq open input" to ACTION-MSG
           OPEN I-O SEQFILE
           PERFORM SEQ-TEST-ACTION.           
       SEQ-OPEN-I-O-EX. EXIT.
       SEQ-READ-TEST SECTION.
             MOVE 0 TO REC-READ
             MOVE KO TO TEST-RESULT
             MOVE "Sequential varying reading test is : " TO R-MSG     
             MOVE "Record read : " TO MSG     
             IF AREWRITE 
                 MOVE "Record rewrite : " TO MSG 
                 MOVE "Sequential varying rewriting test is : " TO R-MSG     
             END-IF    
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
             MOVE "Sequential varying writing test is : " TO R-MSG                  
             PERFORM UNTIL REC-WRITE >= TOTAL-REC-WRITE                   
                   PERFORM SEQ-WRITE
             END-PERFORM        
             MOVE REC-WRITE TO REC-MSG
             MOVE "Record written : " TO MSG                  
             MOVE OK TO TEST-RESULT
             
             PERFORM DISPLAY-RESULT.
       SEQ-WRITE-TEST-EX. EXIT.
       SEQ-READ SECTION.
             MOVE "Seq read record" to ACTION-MSG             
             READ SEQFILE
             PERFORM SEQ-TEST-ACTION
             IF LENREC NOT EQUAL REC-VAR-LEN
      *** error length
                MOVE "Error on rec length" TO ACTION-MSG
                PERFORM DISPLAY-ERROR
                DISPLAY "Length returned :        " LENREC
                DISPLAY "Record physical length : " REC-VAR-LEN 
                PERFORM DISPLAY-RESULT
                PERFORM DUMP-PROGRAM 
             END-IF
             IF NOT SEQ-EOF ADD 1 TO REC-READ              
               IF AREWRITE PERFORM SEQ-REWRITE END-IF
             END-IF.             
       SEQ-READ-EX. EXIT.
       SEQ-WRITE SECTION.
             MOVE "Seq write record" to ACTION-MSG
             ADD 1 TO REC-WRITE
             MOVE REC-WRITE TO REC-9 REC-COMP REC-COMP-3
             MOVE SPACE TO REC-X
             MOVE "REC WRITTEN" TO REC-X-1
             DIVIDE REC-WRITE BY 3 GIVING VAR-GIV
             REMAINDER VAR-REM
             COMPUTE LENREC = 60 + ( VAR-REM * 10 )
             MOVE LENREC TO REC-VAR-LEN
             MOVE ALL "-" TO REC-VAR-X
             WRITE REC-AREA
             PERFORM SEQ-TEST-ACTION.
       SEQ-WRITE-EX. EXIT.
       SEQ-REWRITE SECTION.
             MOVE "Seq rewrite record" to ACTION-MSG
             ADD 1 TO REC-REWRITE
             MOVE REC-REWRITE TO REC-9 REC-COMP REC-COMP-3
             
      *      MOVE SPACE TO REC-X
             MOVE "REC REWRITTEN" TO REC-X-2
             DISPLAY LENREC " " REC-AREA
             REWRITE REC-AREA
             PERFORM SEQ-TEST-ACTION.
       SEQ-REWRITE-EX. EXIT.
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
             MOVE "Test on sequential varying file is : " TO R-MSG                  
             IF REC-WRITE = REC-READ 
      * AND REC-READ = REC-REWRITE THEN
                MOVE OK TO TEST-RESULT   
             END-IF
             PERFORM DISPLAY-RESULT.
       SEQ-CHECK-EX. EXIT.
       DISPLAY-RESULT SECTION.
             MOVE PGM TO PGM-MSG PGM-R
      *       DISPLAY AREA-MSG
             MOVE TEST-RESULT TO R-RESULT
      *       DISPLAY RESULT-MSG
             MOVE SPACE TO R-MSG 
             MOVE SPACE TO MSG.
       DISPLAY-RESULT-EX. EXIT.
       DISPLAY-ERROR SECTION.
             MOVE PGM TO PGM-ERR-MSG
             DISPLAY ERROR-MSG
             MOVE SPACE TO ERROR-MSG.
       DISPLAY-ERROR-EX. EXIT.
       DUMP-PROGRAM SECTION.
             STOP RUN 1.
       DUMP-PROGRAM-EX. EXIT.
       QUIT-PROGRAM SECTION.
             DISPLAY "OK"
             GOBACK.
       QUIT-PROGRAM-EX. EXIT.
