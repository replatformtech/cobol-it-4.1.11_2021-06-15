       IDENTIFICATION DIVISION.
       PROGRAM-ID.    RELFIX.
       ENVIRONMENT DIVISION.
       CONFIGURATION SECTION.
       SPECIAL-NAMES.
           DECIMAL-POINT IS COMMA. 
       INPUT-OUTPUT SECTION.
       FILE-CONTROL.
           SELECT VSAMFILE  ASSIGN TO "relfix_data"
                            ORGANIZATION  IS RELATIVE
                            ACCESS MODE   IS DYNAMIC
                            RELATIVE KEY IS RK
                            FILE STATUS   IS VSAM-STATUS.
      *****************************************************************
       DATA DIVISION.
       FILE SECTION.

      $if relvar=1
       FD  VSAMFILE
            recording mode is v.
      $else
       FD  VSAMFILE.
      $end  
      
       01  REC-AREA.
           03 REC-KEY       PIC S9(4).              
           03 REC-KEY-X REDEFINES REC-KEY PIC X(4).
           03 REC-X.
              05  REC-X-1         PIC X(15).
              05  REC-X-2         PIC X(10).
            03 REC-KEY-2       PIC 9(5).          
            03 REC-KEY-2X REDEFINES REC-KEY-2 PIC X(5).                  
            03 REC-COMP      PIC 9(4) COMP.
            03 REC-COMP-3    PIC 9(8) COMP-3.              
                        
       WORKING-STORAGE SECTION.     
       01 PGM PIC X(8) VALUE "RELFIX".  
       01 RK  PIC 9(8) VALUE 1.
       01 TEST-RESULT.
           03 FILLER PIC XX.
              88 TEST-OK  VALUE 'OK'.
              88 TEST-KO  VALUE 'KO'.
       01 KO   PIC X(2) VALUE 'KO'.
       01 OK   PIC X(2) VALUE 'OK'.
       01 OPER-STATUS.
          03 FILLER    PIC 9.             
             88  ON-ERROR      VALUE  1.
       01 PGM-ACTION.         
           03  FILLER                  PIC X.
             88  ANONE                VALUE ' '.
             88  AREAD                VALUE '1'.
             88  AWRITE               VALUE '2'.
             88  AREWRITE             VALUE '3'.
             88  ASTRBROWSE           VALUE '4'.
             88  READNEXT             VALUE '5'.
             88  READPREVIOUS         VALUE '6'.
             88  ADELETE         VALUE '7'.
       01 VSAM-KEY.         
           03  FILLER                  PIC X.
             88  1-KEY                VALUE '1'.
             88  2-KEY                VALUE '2'.

       01 VSAM-STATUS.         
           03  FILLER                  PIC X.
             88  VSAM-OK                VALUE '0'.
             88  VSAM-EOF               VALUE '1'.
             88  VSAM-ERROR             VALUE '2' '3' '4' '9'.
           03  FILLER                   PIC X.
       01 ERROR-HANDLED REDEFINES VSAM-STATUS.         
           03 FILLER PIC X(2).
             88  REC-NOT-FOUND        VALUE '23'.
       01 APPOKEY    PIC X(4) VALUE LOW-VALUE.
       01 REC-READ   PIC 9(4) VALUE 0.
       01 REC-WRITE  PIC 9(4) VALUE 0.
       01 REC-REWRITE  PIC 9(4) VALUE 0.
       01 REC-DELETE  PIC 9(4) VALUE 0.
       01 TOTAL-REC-WRITE  PIC 9(4) VALUE 10.       
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
          03 MSG        PIC X(40).
          03 REC-MSG    PIC ZZZ.ZZZ.ZZZ.
       01 ERROR-MSG.
          03 FILLER     PIC X(1) VALUE "(".
          03 PGM-ERR-MSG    PIC X(8) VALUE SPACE.
          03 FILLER     PIC X(2) VALUE ") ".
          03 ACTION-MSG    PIC X(40).
          03 STATUS-ERR-MSG    PIC X(2).

      *****************************************************************
       PROCEDURE DIVISION.                        
       MAIN SECTION.             
             MOVE all '-' to R-MSG
             PERFORM DISPLAY-RESULT             
             MOVE "RELATIVE FIXED" TO R-MSG                               
             PERFORM DISPLAY-RESULT
             MOVE all '-' to R-MSG
             PERFORM DISPLAY-RESULT
      * writing test
      
             PERFORM VSAM-OPEN-OUTPUT
             MOVE '1' TO PGM-ACTION            
             MOVE '0' TO OPER-STATUS 
             PERFORM VSAM-WRITE-TEST      
             PERFORM VSAM-CLOSE
             
      * strart browse test
             PERFORM VSAM-OPEN-I-O
             MOVE '4' TO PGM-ACTION  
             MOVE '0' TO OPER-STATUS     
             PERFORM VSAM-START-BROWSE-TEST

      * readnext test
             MOVE '5' TO PGM-ACTION  
             MOVE '0' TO OPER-STATUS     
             PERFORM VSAM-READNEXT-TEST
      * readprevious test
             MOVE '6' TO PGM-ACTION  
             MOVE '0' TO OPER-STATUS     
             PERFORM VSAM-READPREVIOUS-TEST

      * read direct test
             MOVE '1' TO PGM-ACTION  
             MOVE '0' TO OPER-STATUS     
             PERFORM VSAM-READ-TEST

      * rewrite direct test
             MOVE '3' TO PGM-ACTION
             MOVE '0' TO OPER-STATUS     
             PERFORM VSAM-READ-TEST

      * rewrite seq test
             MOVE '3' TO PGM-ACTION  
             MOVE '0' TO OPER-STATUS          
             PERFORM VSAM-READNEXT-TEST 

      * delete test
             MOVE '7' TO PGM-ACTION
             MOVE '0' TO OPER-STATUS     
             PERFORM VSAM-READ-TEST
             
      * check 
             PERFORM VSAM-CHECK
      * quit 
             PERFORM VSAM-CLOSE
             PERFORM QUIT-PROGRAM.
       SEQ-OPEN-INPUT SECTION.
           MOVE "Rel. open input" to ACTION-MSG
           OPEN INPUT VSAMFILE
           PERFORM VSAM-TEST-ACTION.           
       SEQ-OPEN-INPUT-EX. EXIT.
       VSAM-OPEN-OUTPUT SECTION.
             MOVE "Rel. open output" to ACTION-MSG
             OPEN OUTPUT VSAMFILE
             PERFORM VSAM-TEST-ACTION.
       VSAM-OPEN-OUTPUT-EX. EXIT.
       VSAM-OPEN-I-O SECTION.
           MOVE "Rel. open input" to ACTION-MSG
           OPEN I-O VSAMFILE
           PERFORM VSAM-TEST-ACTION.           
       VSAM-OPEN-I-O-EX. EXIT.
       VSAM-START-BROWSE-TEST SECTION.
      * Start Browse 
             MOVE 0 TO REC-READ
             MOVE KO TO TEST-RESULT
             MOVE "Rel. start browse test is : " TO R-MSG     
             MOVE "Start browse : " TO MSG                               
      * Equal             
             PERFORM VSAM-START-BROWSE-EQ
      * not less than
             PERFORM VSAM-START-BROWSE-GE
      * grater than
             PERFORM VSAM-START-BROWSE-GT
      * less than [unsupported]
      *      PERFORM VSAM-START-BROWSE-LT
      * less than or equal to [unsupported]
      *       PERFORM VSAM-START-BROWSE-LE

             IF NOT ON-ERROR 
                MOVE OK TO TEST-RESULT             
             END-IF
             PERFORM DISPLAY-RESULT.
       VSAM-START-BROWSE-TEST-EX. EXIT.
       VSAM-READNEXT-TEST SECTION.      
             MOVE 0 TO REC-READ
             MOVE KO TO TEST-RESULT
             MOVE "Rel. read next test is : " TO R-MSG     
             MOVE "Raed next: " TO MSG                               
             IF AREWRITE 
                 MOVE "Record rewrite : " TO MSG 
                 MOVE "Rel. fixed rewrite seq. test is : "
                 TO R-MSG     
             ELSE  
                 IF ADELETE
                   MOVE "Record delete : " TO MSG 
                   MOVE "Rel. fixed delete direct test is : " TO R-MSG                  
                END-IF
             END-IF    
             MOVE 1 TO RK
             PERFORM VSAM-START-BROWSE
             PERFORM VSAM-READ-NEXT
             PERFORM UNTIL VSAM-EOF
      *          DISPLAY REC-AREA
                PERFORM VSAM-READ-NEXT
             END-PERFORM                     
             MOVE REC-READ TO REC-MSG
             IF REC-READ = TOTAL-REC-WRITE 
                 MOVE OK TO TEST-RESULT             
             END-IF
             PERFORM DISPLAY-RESULT.
       VSAM-READNEXT-BROWSE-TEST-EX. EXIT.
       VSAM-READPREVIOUS-TEST SECTION.      
             MOVE 0 TO REC-READ
             MOVE KO TO TEST-RESULT
             MOVE "Rel. read previous test is : " TO R-MSG     
             MOVE "Read previous: " TO MSG                               
             MOVE TOTAL-REC-WRITE TO RK
             PERFORM VSAM-START-BROWSE
             PERFORM VSAM-READ-PREV
             PERFORM UNTIL VSAM-EOF
      *          DISPLAY REC-AREA 
               PERFORM VSAM-READ-PREV
               IF REC-READ > TOTAL-REC-WRITE 
                   MOVE '1' TO VSAM-STATUS
               END-IF
             END-PERFORM             
             MOVE REC-READ TO REC-MSG
      * Read 1 record less than readnext process
      * doesn't read again record pointed from start browse
             IF REC-READ = ( TOTAL-REC-WRITE - 1 ) 
                 MOVE OK TO TEST-RESULT             
             END-IF
             PERFORM DISPLAY-RESULT.
       VSAM-READPREVIOUS-TEST-EX. EXIT.


       VSAM-READ-TEST SECTION.
             MOVE 0 TO REC-READ
             MOVE KO TO TEST-RESULT
             MOVE "Rel. fixed direct read test is : " TO R-MSG     
             MOVE "Record read : " TO MSG     
             IF AREWRITE 
                 MOVE "Record rewrite : " TO MSG 
                 MOVE "Rel. fixed rewrite direct test is : " TO R-MSG     
             ELSE 
                 IF ADELETE
                   MOVE "Record delete : " TO MSG 
                   MOVE "Rel. fixed delete direct test is : " TO R-MSG     
                 END-IF
             END-IF  

             MOVE 1 TO RK
             PERFORM VSAM-READ             
      *       DISPLAY REC-AREA 

             MOVE TOTAL-REC-WRITE TO RK
             PERFORM VSAM-READ
      *       DISPLAY REC-AREA 

             MOVE REC-READ TO REC-MSG
             MOVE OK TO TEST-RESULT             
             PERFORM DISPLAY-RESULT.
       VSAM-READ-TEST-EX. EXIT.
       VSAM-WRITE-TEST SECTION.             
             MOVE 0 TO REC-WRITE
             MOVE KO TO TEST-RESULT
             MOVE "Indexed fixed writing test is : " TO R-MSG                  
             PERFORM UNTIL REC-WRITE >= TOTAL-REC-WRITE
                   PERFORM VSAM-WRITE
             END-PERFORM        
             MOVE REC-WRITE TO REC-MSG
             MOVE "Record written : " TO MSG                  
             MOVE OK TO TEST-RESULT             
             PERFORM DISPLAY-RESULT.
       VSAM-WRITE-TEST-EX. EXIT.
       VSAM-START-BROWSE SECTION.
             MOVE "Rel. start browse gt 1°key : " to ACTION-MSG                             
             START VSAMFILE KEY IS EQUAL TO RK
             PERFORM VSAM-TEST-ACTION.
       VSAM-START-BROWSE-EX. EXIT.

       VSAM-START-BROWSE-EQ SECTION.
             MOVE "Rel. start browse eq 1°key : " to ACTION-MSG                             
             MOVE 0 TO RK                
             START VSAMFILE KEY IS EQUAL TO RK             
             IF ASTRBROWSE AND REC-NOT-FOUND                  
                  CONTINUE
             ELSE 
                  PERFORM CHECK-ERROR                                                                          
                  PERFORM VSAM-TEST-ACTION
             END-IF.             
       VSAM-START-BROWSE-EQ-EX. EXIT.
       VSAM-START-BROWSE-GE SECTION.
             MOVE "Rel. start browse ge 1°key : " to ACTION-MSG             
             MOVE 1 TO RK
             START VSAMFILE KEY IS NOT LESS THAN RK
             PERFORM VSAM-TEST-ACTION.
             PERFORM VSAM-READ-NEXT.             
             MOVE "Vsam start browse ge 1°key : " to ACTION-MSG             
             IF REC-KEY NOT EQUAL 1                   
                PERFORM CHECK-ERROR                                                                          
             END-IF.             
       VSAM-START-BROWSE-GE-EX. EXIT.
       VSAM-START-BROWSE-GT SECTION.
             MOVE "Rel. start browse gt 1°key : " to ACTION-MSG             
             MOVE 1 TO RK
             START VSAMFILE KEY IS GREATER THAN RK             
             PERFORM VSAM-TEST-ACTION.
             PERFORM VSAM-READ-NEXT.
             MOVE "Rel. start browse gt 1°key : " to ACTION-MSG             
             IF REC-KEY NOT EQUAL 2                
                PERFORM CHECK-ERROR                                                                          
            END-IF.
       VSAM-START-BROWSE-GT-EX. EXIT.
      * VSAM-START-BROWSE-LT SECTION.
      *       IF 1-KEY             
      *          MOVE "Vsam start browse lt 1°key : " to ACTION-MSG             
      *          MOVE 0 TO REC-KEY
      *          START VSAMFILE KEY IS LESS THAN REC-KEY
      *          IF ASTRBROWSE AND REC-NOT-FOUND
      ** Record not found test ok
      *             CONTINUE
      *          ELSE                    
      *              PERFORM CHECK-ERROR                                                                          
      *              PERFORM VSAM-TEST-ACTION
      *          END-IF
      *       ELSE
      *          MOVE "Vsam start browse lt 2°key : " to ACTION-MSG             
      *          CONTINUE
      *       END-IF.
      * VSAM-START-BROWSE-LT-EX. EXIT.
      * VSAM-START-BROWSE-LE SECTION.
      *       IF 1-KEY             
      *          MOVE "Vsam start browse le 1°key : " to ACTION-MSG             
      *          MOVE 0 TO REC-KEY
      *          START VSAMFILE KEY IS LESS THAN OR EQUAL TO REC-KEY                
      *          IF ASTRBROWSE AND REC-NOT-FOUND
      * Record not found test ok
      *             CONTINUE
      *          ELSE                    
      *              PERFORM CHECK-ERROR                                                                          
      *              PERFORM VSAM-TEST-ACTION
      *          END-IF
      *       ELSE
      *          MOVE "Vsam start browse le 2°key : " to ACTION-MSG             
      *          CONTINUE
      *       END-IF.
      * VSAM-START-BROWSE-LT-EX. EXIT.


       VSAM-READ-NEXT SECTION.
             MOVE "Rel. read next record" to ACTION-MSG             
             READ VSAMFILE NEXT
             PERFORM VSAM-TEST-ACTION
             IF NOT VSAM-EOF ADD 1 TO REC-READ              
      *         DISPLAY REC-AREA
               IF AREWRITE PERFORM VSAM-REWRITE END-IF
             END-IF.             
       VSAM-READ-NEXT-EX. EXIT.
       VSAM-READ-PREV SECTION.
             MOVE "Rel. read previous record" to ACTION-MSG             
      * COBOL-IT unsupported clause in compiling phase
             READ VSAMFILE PREVIOUS
             PERFORM VSAM-TEST-ACTION
             IF NOT VSAM-EOF 
               ADD 1 TO REC-READ              
      *         DISPLAY REC-AREA
      *         IF AREWRITE PERFORM VSAM-REWRITE END-IF
             END-IF.             
       VSAM-READ-PREV-EX. EXIT.

       VSAM-READ SECTION.
             MOVE "Rel. read record" to ACTION-MSG             
             READ VSAMFILE               
             PERFORM VSAM-TEST-ACTION
             IF NOT VSAM-EOF ADD 1 TO REC-READ              
               IF AREWRITE PERFORM VSAM-REWRITE END-IF
               IF ADELETE PERFORM VSAM-DELETE END-IF
             END-IF.             
       VSAM-READ-EX. EXIT.
       VSAM-WRITE SECTION.
             MOVE "Rel. write record" to ACTION-MSG
             ADD 1 TO REC-WRITE
             MOVE REC-WRITE TO REC-KEY REC-COMP REC-COMP-3
             MOVE SPACE TO REC-X
             MOVE "REC WRITTEN" TO REC-X-1
      * second key 
             COMPUTE REC-KEY-2 = TOTAL-REC-WRITE - REC-WRITE
             WRITE REC-AREA
             ADD 1 TO RK
             PERFORM VSAM-TEST-ACTION.
       VSAM-WRITE-EX. EXIT.
       VSAM-REWRITE SECTION.
             MOVE "Rel. rewrite record" to ACTION-MSG
             ADD 1 TO REC-REWRITE             
      *      MOVE SPACE TO REC-X
             IF REC-X-2 = SPACE
                MOVE "1-REC REW" TO REC-X-2
             ELSE 
                MOVE "2-UPD YET" TO REC-X-2
             END-IF
             REWRITE REC-AREA
      *       DISPLAY REC-AREA
             PERFORM VSAM-TEST-ACTION.
       VSAM-REWRITE-EX. EXIT.
       VSAM-DELETE SECTION.
             MOVE "Rel. delete record" to ACTION-MSG
             ADD 1 TO REC-DELETE             
      *      MOVE SPACE TO REC-X
             DELETE VSAMFILE RECORD 
             PERFORM VSAM-TEST-ACTION.
       VSAM-DELETE-EX. EXIT.
       VSAM-CLOSE SECTION.  
             MOVE "Rel. close file" to ACTION-MSG
             CLOSE VSAMFILE        
             PERFORM VSAM-TEST-ACTION.
       VSAM-CLOSE-EX. EXIT.
       VSAM-TEST-ACTION SECTION.
             IF VSAM-ERROR
                MOVE VSAM-STATUS TO STATUS-ERR-MSG 
                PERFORM DISPLAY-ERROR
                PERFORM DISPLAY-RESULT
                PERFORM DUMP-PROGRAM
             END-IF.           
       VSAM-TEST-ACTION-EX. EXIT.     
       VSAM-CHECK SECTION.             
             MOVE SPACE TO TEST-RESULT
             MOVE ALL "-" TO R-MSG
             PERFORM DISPLAY-RESULT
             MOVE OK TO TEST-RESULT
             MOVE "Test on vsam fixed file is : " TO R-MSG                  
             PERFORM DISPLAY-RESULT.
       VSAM-CHECK-EX. EXIT.
       DISPLAY-RESULT SECTION.
      *
      *       MOVE PGM TO PGM-MSG PGM-R
      *       DISPLAY AREA-MSG
      *       MOVE TEST-RESULT TO R-RESULT
      *       DISPLAY RESULT-MSG
      *
             MOVE SPACE TO R-MSG 
             MOVE SPACE TO MSG.
       DISPLAY-RESULT-EX. EXIT.
       DISPLAY-ERROR SECTION.
             MOVE PGM TO PGM-ERR-MSG
             DISPLAY ERROR-MSG
             MOVE SPACE TO MSG.
       DISPLAY-ERROR-EX. EXIT.
       CHECK-ERROR SECTION.
             MOVE KO TO STATUS-ERR-MSG
             MOVE '1' TO OPER-STATUS 
             PERFORM DISPLAY-ERROR.
       CHECK-ERROR-EX. EXIT.
       DUMP-PROGRAM SECTION.
             STOP RUN 1.
       DUMP-PROGRAM-EX. EXIT.
       QUIT-PROGRAM SECTION.
             DISPLAY "OK"
             GOBACK.
       QUIT-PROGRAM-EX. EXIT.
