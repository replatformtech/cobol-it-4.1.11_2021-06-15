       IDENTIFICATION DIVISION.
       PROGRAM-ID.                      CUSTOMER0.

      ******************************************************************
       ENVIRONMENT DIVISION.
       CONFIGURATION SECTION.

       INPUT-OUTPUT SECTION.
       FILE-CONTROL.
           SELECT CUSTOMER 
                  ASSIGN TO DISK "customer"
                  ORGANIZATION IS INDEXED
                  ACCESS IS DYNAMIC
                  RECORD KEY IS CUST-RES-ID
                  ALTERNATE RECORD KEY IS FLIGHT-ID WITH DUPLICATES
                  FILE STATUS IS CUSTOMER-STATUS.

           SELECT PRINT-FILE 
                  ASSIGN TO  "PRINTER"
                  ORGANIZATION IS SEQUENTIAL
                  FILE STATUS IS PRINT-FILE-STATUS.           

      *****************************************************************
       DATA DIVISION.
       FILE SECTION.
       FD  CUSTOMER .
       01  CUSTOMER-RECORD.
           03 CUST-RES-ID.
              05 CUST-FLIGHT             PIC X(07).
              05 CUST-NO                 PIC X(03).
           03 CUST-FNAME                 PIC X(30).
           03 CUST-LNAME                 PIC X(30).
           03 CUST-DOB.
               04 DOB-MM                 PIC X(02).
               04 DOB-DD                 PIC X(02).
               04 DOB-YY                 PIC X(02).
           03 CUST-ADDRESS.
               04 STREET                 PIC X(30).
               04 CITY                   PIC X(15).
               04 STATE                  PIC X(05).
               04 ZIP                    PIC X(10).
           03 CUST-TEL                   PIC X(15).
           03 CUST-FAX                   PIC X(15).
           03 CUST-EMAIL                 PIC X(20).
           03 SEAT-TYPE                  PIC X(02).
           03 FOOD                       PIC X(15).
           03 PAYMENT-TYPE               PIC X(03).
           03 PAYMENT-AMT                PIC X(04).
           03 FLIGHT-ID                  PIC X(07).

       FD PRINT-FILE.
       01 PRINT-RECORD                   PIC X(80).

       WORKING-STORAGE SECTION.
       
       01  CUSTOMER-STATUS               PIC X(02).
           88 NOT-PRESENT                VALUE "35".
       01  PRINT-FILE-STATUS             PIC X(02).
       01  MENU-CHOICE1                  PIC X(01) VALUE SPACES.
           88 ADD-CUST                   VALUE "1".
           88 MODIFY-CUST                VALUE "2".
           88 DELETE-CUST                VALUE "3".
           88 PRINT-FLIGHT-RES           VALUE "4".
           88 EXIT-PROG                  VALUE "5".
       01  MENU-CHOICE2                  PIC X(01) VALUE SPACES.
           88 SAVE-REC                   VALUE "1".
           88 CANCEL-SAVE                VALUE "2".
           88 PRINT-REC                  VALUE "3".
       01  MENU-CHOICE3                  PIC X(01) VALUE SPACES.
           88 DELETE-REC                 VALUE "1".
           88 CANCEL-DELETE              VALUE "2".
       01  W-SYS-DATE.
           02 W-SYS-YY                   PIC X(02).
           02 W-SYS-MM                   PIC X(02).
           02 W-SYS-DD                   PIC X(02).
       01  W-CURRENT-DATE.
           02 CURR-MM                    PIC X(02).
           02 CURR-DD                    PIC X(02).
           02 CURR-YY                    PIC X(02).
       01  BLNK-LINE                     PIC X(76)  VALUE SPACES.
       01  CNTR                          PIC 9(02)  VALUE 0.
       01  W-CNTR                        PIC 9(03)  VALUE 0.
       01  RESP                          PIC X(01)  VALUE SPACES.       
       01  DATA-VALIDITY-SWITCH          PIC 9      VALUE 0.
           88  DATA-VALID                           VALUE 1.
           88  DATA-INVALID                         VALUE 0.
       01  SCUSTOMER-RECORD              PIC X(217) VALUE SPACES.
       01  HOLD-FLT-ID                   PIC X(07)  VALUE SPACES.

       01  PID                           PIC 9(9).
       LINKAGE SECTION.

       SCREEN SECTION.

      ******************************************************************
       PROCEDURE DIVISION.

       Main Section.
           CALL "C$PID" USING PID.
           DISPLAY "PID = " PID.
      *     CALL "C$DEBUG"
           ACCEPT W-SYS-DATE FROM DATE.
           MOVE W-SYS-YY                   TO  CURR-YY.
           MOVE W-SYS-MM                   TO  CURR-MM.
           MOVE W-SYS-DD                   TO  CURR-DD.
           OPEN I-O CUSTOMER.
           IF NOT-PRESENT
                OPEN output CUSTOMER
           END-IF
           CLOSE CUSTOMER.
           PERFORM MAIN-LOGIC.
           Exit Program.
           Stop Run.
           
       MAIN-LOGIC.
           PERFORM WITH TEST AFTER UNTIL 
               MENU-CHOICE1 = "5"
               DISPLAY " "                          AT LINE 17 COL 48
               PERFORM DISP-MAIN-MENU
               ACCEPT MENU-CHOICE1                  AT LINE 17 COL 48
               IF  ADD-CUST
                   PERFORM ADD-CUST-RES
               END-IF
               IF  MODIFY-CUST
                   PERFORM MODIFY-CUST-RES
               END-IF
               IF  DELETE-CUST
                   PERFORM DELETE-CUST-RES
               END-IF
               IF  PRINT-FLIGHT-RES
                   PERFORM PRINT-RESERV
               END-IF
           END-PERFORM.


       DISP-MAIN-MENU.
           PERFORM CLEAR-SCREEN.
           DISPLAY "      TRAVELS      "            AT LINE 1  COL 20. 
           DISPLAY W-SYS-MM                         AT LINE 1  COL 65.
           DISPLAY "/"                              AT LINE 1  COL 67.
           DISPLAY W-SYS-DD                         AT LINE 1  COL 68.
           DISPLAY "/"                              AT LINE 1  COL 70.
           DISPLAY W-SYS-YY                         AT LINE 1  COL 71.
           DISPLAY "CUSTOMER MAINTENANCE SCREEN"    AT LINE 3  COL 20. 
           DISPLAY "---------------------------"    AT LINE 4  COL 20.
           DISPLAY "1)ADD NEW CUSTOMER RESERVATION" AT LINE 7  COL 20.
           DISPLAY "2)MODIFY  CUSTOMER RESERVATION" AT LINE 9  COL 20.
           DISPLAY "3)DELETE  CUSTOMER RESERVATION" AT LINE 11 COL 20.
           DISPLAY "4)PRINT   FLIGHT   RESERVATION" AT LINE 13 COL 20.
           DISPLAY "5)EXIT"                         AT LINE 15 COL 20.
           DISPLAY "SELECT A MENU CHOICE(1-5):- "   AT LINE 17 COL 20.

       ADD-CUST-RES.
           PERFORM CLEAR-SCREEN.
           MOVE ALL " "                    TO  CUSTOMER-RECORD.
           PERFORM DISPLAY-LABELS.
           PERFORM DISPLAY-FIELDS.
           DISPLAY " "                              AT LINE 24 COL 52
           PERFORM ACCEPT-FIELDS
           IF SAVE-REC
              MOVE CUSTOMER-RECORD         TO  SCUSTOMER-RECORD
              OPEN I-O CUSTOMER
              PERFORM GET-CUST-RES-ID
              WRITE CUSTOMER-RECORD
              DISPLAY "***RESERVATION HAS BEEN CONFIRMED*** PRESS ANY KE
      -       "Y TO CONTINUE"                       AT LINE 25  COL 10
              ACCEPT RESP
              DISPLAY "PRESS <P> TO PRINT RESERVATION AND EXIT, ANY OTHE
      -       "R KEY TO EXIT"                       AT LINE 25  COL 1
              ACCEPT RESP
              IF RESP = "P"
                 OPEN OUTPUT PRINT-FILE
                 PERFORM PRINT-RESERVATIONS
                 CLOSE PRINT-FILE
              END-IF
              INITIALIZE CUSTOMER-RECORD
              CLOSE CUSTOMER
           END-IF.
           
       MODIFY-CUST-RES.
           PERFORM CLEAR-SCREEN.
           MOVE ALL " "                    TO  CUSTOMER-RECORD.
           PERFORM DISPLAY-LABELS.
           DISPLAY "MODIFY CUSTOMER RESERVATIONS  " AT LINE 3  COL 20.
           PERFORM DISPLAY-FIELDS.
           DISPLAY " " AT LINE 24 COL 52
           ACCEPT  CUST-RES-ID                      AT LINE 6  COL 36.
           OPEN I-O CUSTOMER.
           START CUSTOMER KEY IS = CUST-RES-ID
                 INVALID KEY 
                    DISPLAY "RESERVATION NOT FOUND.PRESS ANY KEY TO CONT
      -             "INUE"                          AT LINE 25 COL 10
                    ACCEPT RESP
                    CLOSE CUSTOMER
                 NOT INVALID KEY
                    READ CUSTOMER NEXT
                    PERFORM DISPLAY-FIELDS
                    PERFORM MODIFY-FIELDS
                    IF SAVE-REC
                       REWRITE CUSTOMER-RECORD
                        DISPLAY "**RESERVATION HAS BEEN MODIFIED** PRESS
      -                 " ANY KEY TO CONTINUE"      AT LINE 25  COL 10
                        ACCEPT RESP
                        INITIALIZE CUSTOMER-RECORD
                    END-IF
                    CLOSE CUSTOMER
           END-START.
           
       DELETE-CUST-RES.
           PERFORM CLEAR-SCREEN.
           MOVE ALL " "                    TO  CUSTOMER-RECORD.
           PERFORM DISPLAY-LABELS.
           DISPLAY "DELETE CUSTOMER RESERVATIONS  " AT LINE 3  COL 20.
           PERFORM DISPLAY-FIELDS.
           DISPLAY " " AT LINE 24 COL 52
           ACCEPT  CUST-RES-ID                      AT LINE 6  COL 36.
           OPEN I-O CUSTOMER.
           START CUSTOMER KEY IS = CUST-RES-ID
                 INVALID KEY 
                    DISPLAY "RESERVATION NOT FOUND.PRESS ANY KEY TO CONT
      -             "INUE"                          AT LINE 25 COL 10
                    ACCEPT RESP
                    CLOSE CUSTOMER
                 NOT INVALID KEY
                    READ CUSTOMER NEXT
                    PERFORM DISPLAY-FIELDS
                    DISPLAY "1)DELETE 2)CANCEL             :-"
                                                    AT LINE 24 COL 20
                    PERFORM WITH TEST AFTER UNTIL MENU-CHOICE3 = '1' OR
                                                  MENU-CHOICE3 = '2'
                    ACCEPT MENU-CHOICE3             AT LINE 24 COL 52
                    END-PERFORM
                    IF DELETE-REC
                       DELETE  CUSTOMER
                        DISPLAY "**RESERVATION HAS BEEN DELETED ** PRESS
      -                 " ANY KEY TO CONTINUE"      AT LINE 25  COL 10
                        ACCEPT RESP
                        INITIALIZE CUSTOMER-RECORD
                    END-IF
                    CLOSE CUSTOMER
           END-START.
         
       PRINT-RESERV.                              
           PERFORM CLEAR-SCREEN.
           MOVE ALL " "                    TO  CUSTOMER-RECORD.
           DISPLAY "      TRAVELS      "            AT LINE 1  COL 20. 
           DISPLAY W-SYS-MM                         AT LINE 1  COL 65.
           DISPLAY "/"                              AT LINE 1  COL 67.
           DISPLAY W-SYS-DD                         AT LINE 1  COL 68.
           DISPLAY "/"                              AT LINE 1  COL 70.
           DISPLAY W-SYS-YY                         AT LINE 1  COL 71.
           DISPLAY "PRINT RESERVATIONS         "    AT LINE 3  COL 20.
           DISPLAY "------------------         "    AT LINE 4  COL 20.
           DISPLAY "FLIGHT ID      :              " AT LINE 6  COL 20.
           ACCEPT  FLIGHT-ID                        AT LINE 6  COL 36.
           MOVE    FLIGHT-ID               TO  CUST-FLIGHT
                                               HOLD-FLT-ID.
           MOVE    LOW-VALUES              TO  CUST-NO.
           OPEN INPUT CUSTOMER.
           START CUSTOMER KEY IS >= CUST-RES-ID
                 INVALID KEY 
                     DISPLAY "FLIGHT RESERVATIONS DO NOT EXIST"
                                                    AT LINE 24 COL 10
                     ACCEPT RESP
                 NOT INVALID KEY
                 READ CUSTOMER NEXT
                 IF CUST-FLIGHT = HOLD-FLT-ID
                    OPEN OUTPUT PRINT-FILE
                    PERFORM UNTIL CUST-FLIGHT NOT = HOLD-FLT-ID
                       PERFORM PRINT-RESERVATIONS                  
                       READ CUSTOMER NEXT
                            AT END 
                            MOVE SPACES    TO  CUST-RES-ID
                            NOT AT END 
                            CONTINUE
                       END-READ
                    END-PERFORM
                    CLOSE PRINT-FILE
                    DISPLAY "FLIGHT RESERVATIONS PRINTED"
                                                    AT LINE 24 COL 10
                    ACCEPT RESP
                 ELSE
                    DISPLAY "FLIGHT RESERVATIONS DO NOT EXIST"
                                                    AT LINE 24 COL 10
                    ACCEPT RESP
                 END-IF
           END-START.
           CLOSE CUSTOMER.

       DISPLAY-LABELS.
           DISPLAY "      TRAVELS      "            AT LINE 1  COL 20. 
           DISPLAY W-SYS-MM                         AT LINE 1  COL 65.
           DISPLAY "/"                              AT LINE 1  COL 67.
           DISPLAY W-SYS-DD                         AT LINE 1  COL 68.
           DISPLAY "/"                              AT LINE 1  COL 70.
           DISPLAY W-SYS-YY                         AT LINE 1  COL 71.
           DISPLAY "ADD CUSTOMER RESERVATIONS  "    AT LINE 3  COL 20.
           DISPLAY "---------------------------"    AT LINE 4  COL 20.
           DISPLAY "RESERVATION ID :              " AT LINE 6  COL 20.
           DISPLAY "FIRST NAME     :              " AT LINE 7  COL 20.
           DISPLAY "LAST NAME      :              " AT LINE 8  COL 20.
           DISPLAY "DATE OF BIRTH  :  /  /    (MM/DD/YY)" 
                                                    AT LINE 9  COL 20.
           DISPLAY "ADDRESS                       " AT LINE 10 COL 20.
           DISPLAY "  STREET       :              " AT LINE 11 COL 20.
           DISPLAY "  CITY         :              " AT LINE 12 COL 20.
           DISPLAY "  STATE        :              " AT LINE 13 COL 20.
           DISPLAY "  ZIP          :              " AT LINE 14 COL 20.
           DISPLAY "TELEPHONE      :              " AT LINE 15 COL 20.
           DISPLAY "FAX            :              " AT LINE 16 COL 20.
           DISPLAY "E-MAIL         :              " AT LINE 17 COL 20.
           DISPLAY "SEAT-TYPE      :     (CO-COACH / FC-FIRST CLASS)" 
                                                    AT LINE 18 COL 20.
           DISPLAY "FOOD-TYPE      :              " AT LINE 19 COL 20.
           DISPLAY "PAYMENT TYPE   :     (MST-MASTER / VSA-VISA /CHK-CHE
      -    "CK)"                                    AT LINE 20 COL 20.
           DISPLAY "PAYMENT AMOUNT :     $        " AT LINE 21 COL 20.
           DISPLAY "FLIGHT ID      :              " AT LINE 22 COL 20.
           DISPLAY "1)SAVE  2)CANCEL              :-"
                                                    AT LINE 24 COL 20.
       ACCEPT-FIELDS.

           PERFORM WITH TEST AFTER UNTIL DATA-VALID
              SET DATA-VALID               TO  TRUE
              ACCEPT CUST-FNAME                     AT LINE 7  COL 36
              IF CUST-FNAME = SPACES
                 DISPLAY "FIRST NAME REQUIRED"      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.
           
           PERFORM WITH TEST AFTER UNTIL DATA-VALID
              SET DATA-VALID               TO  TRUE
              ACCEPT CUST-LNAME                     AT LINE 8  COL 36
              IF CUST-LNAME = SPACES
                 DISPLAY "LAST NAME REQUIRED "      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.

           PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT DOB-MM                         AT LINE 9  COL 36
              ACCEPT DOB-DD                         AT LINE 9  COL 39
              ACCEPT DOB-YY                         AT LINE 9  COL 42
              IF CUST-DOB = SPACES
                 DISPLAY "BIRTH DATE REQUIRED"      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.

           PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT STREET                         AT LINE 11 COL 36
              ACCEPT CITY                           AT LINE 12 COL 36
              ACCEPT STATE                          AT LINE 13 COL 36
              ACCEPT ZIP                            AT LINE 14 COL 36
              IF STREET = SPACES AND CITY = SPACES AND STATE = SPACES
                 AND ZIP = SPACES
                 DISPLAY "ADDRESS REQUIRED   "      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.

           ACCEPT CUST-TEL                          AT LINE 15 COL 36.
           ACCEPT CUST-FAX                          AT LINE 16 COL 36.
           ACCEPT CUST-EMAIL                        AT LINE 17 COL 36.

           PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT SEAT-TYPE                      AT LINE 18 COL 36
              IF SEAT-TYPE = "CO" OR SEAT-TYPE = "FC"
                 CONTINUE
              ELSE
                 DISPLAY "ENTER VALID SEAT TYPE"    AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF           
           END-PERFORM.
           
           PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT FOOD                           AT LINE 19 COL 36
              IF FOOD = SPACES
                 DISPLAY "ENTER FOOD PREFERENCE"    AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.
           
           PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT PAYMENT-TYPE                   AT LINE 20 COL 36
              IF PAYMENT-TYPE = "MST" OR PAYMENT-TYPE = "VSA" OR
                 PAYMENT-TYPE = "CHK"
                 CONTINUE
              ELSE
                 DISPLAY "ENTER VALID PAYMENT TYPE" AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.           

           ACCEPT PAYMENT-AMT                       AT LINE 21 COL 36.

           PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT FLIGHT-ID                      AT LINE 22 COL 36
              IF FLIGHT-ID = SPACES
                 DISPLAY "ENTER VALID FLIGHT ID   " AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
           END-PERFORM.           
           PERFORM WITH TEST AFTER UNTIL MENU-CHOICE2 = '1' OR
                                         MENU-CHOICE2 = '2'
             ACCEPT MENU-CHOICE2                    AT LINE 24 COL 52
           END-PERFORM.
           
       MODIFY-FIELDS.
       
           MOVE SPACES                     TO  MENU-CHOICE2.
           DISPLAY CUST-FNAME              AT LINE 7  COL 36 
                                           WITH REVERSE-VIDEO.
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELD <Q>QUIT : "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'
             MOVE SPACES                   TO  CUST-FNAME
             DISPLAY CUST-FNAME            AT LINE 7  COL 36
                                           WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID
              SET DATA-VALID               TO  TRUE
              ACCEPT CUST-FNAME                     AT LINE 7  COL 36
              IF CUST-FNAME = SPACES
                 DISPLAY "FIRST NAME REQUIRED"      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM
           END-IF.
           DISPLAY CUST-FNAME                       AT LINE 7  COL 36.


           DISPLAY CUST-LNAME                       AT LINE 8  COL 36
                                                    WITH REVERSE-VIDEO.  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELD <Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
             MOVE SPACES                   TO  CUST-LNAME
             DISPLAY CUST-LNAME                     AT LINE 8  COL 36
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID
              SET DATA-VALID               TO  TRUE
              ACCEPT CUST-LNAME                     AT LINE 8  COL 36
              IF CUST-LNAME = SPACES
                 DISPLAY "LAST NAME REQUIRED "      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM
           END-IF.
           DISPLAY CUST-LNAME                       AT LINE 8  COL 36.

           DISPLAY   DOB-MM                         AT LINE 9  COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY   DOB-DD                         AT LINE 9  COL 39
                                                    WITH REVERSE-VIDEO  
           DISPLAY   DOB-YY                         AT LINE 9  COL 42
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELD <Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
             MOVE SPACES                   TO  CUST-DOB
             DISPLAY   DOB-MM                       AT LINE 9  COL 36
                                                    WITH REVERSE-VIDEO  
             DISPLAY   DOB-DD                       AT LINE 9  COL 39
                                                    WITH REVERSE-VIDEO  
             DISPLAY   DOB-YY                       AT LINE 9  COL 42
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT DOB-MM                         AT LINE 9  COL 36
              ACCEPT DOB-DD                         AT LINE 9  COL 39
              ACCEPT DOB-YY                         AT LINE 9  COL 42
              IF CUST-DOB = SPACES
                 DISPLAY "BIRTH DATE REQUIRED"      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM
           END-IF.
           DISPLAY   DOB-MM                         AT LINE 9  COL 36.
           DISPLAY   DOB-DD                         AT LINE 9  COL 39.
           DISPLAY   DOB-YY                         AT LINE 9  COL 42.

           DISPLAY   STREET                         AT LINE 11 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY   CITY                           AT LINE 12 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY   STATE                          AT LINE 13 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY   ZIP                            AT LINE 14 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
             MOVE SPACES                   TO  CUST-ADDRESS
             DISPLAY   STREET                       AT LINE 11 COL 36
                                                    WITH REVERSE-VIDEO  
             DISPLAY   CITY                         AT LINE 12 COL 36
                                                    WITH REVERSE-VIDEO  
             DISPLAY   STATE                        AT LINE 13 COL 36
                                                    WITH REVERSE-VIDEO  
             DISPLAY   ZIP                          AT LINE 14 COL 36
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT STREET                         AT LINE 11 COL 36
              ACCEPT CITY                           AT LINE 12 COL 36
              ACCEPT STATE                          AT LINE 13 COL 36
              ACCEPT ZIP                            AT LINE 14 COL 36
              IF STREET = SPACES AND CITY = SPACES AND STATE = SPACES
                 AND ZIP = SPACES
                 DISPLAY "ADDRESS REQUIRED   "      AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM 
           END-IF.
           DISPLAY   STREET                         AT LINE 11 COL 36.
           DISPLAY   CITY                           AT LINE 12 COL 36.
           DISPLAY   STATE                          AT LINE 13 COL 36.
           DISPLAY   ZIP                            AT LINE 14 COL 36.
           
           DISPLAY CUST-TEL                         AT LINE 15 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'
              MOVE SPACES                  TO  CUST-TEL
              DISPLAY CUST-TEL                      AT LINE 15 COL 36
                                                    WITH REVERSE-VIDEO  
              ACCEPT CUST-TEL                       AT LINE 15 COL 36
           END-IF.
           DISPLAY CUST-TEL                         AT LINE 15 COL 36.
           
           DISPLAY CUST-FAX                         AT LINE 16 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
              MOVE SPACES                  TO  CUST-FAX
              DISPLAY CUST-FAX                      AT LINE 16 COL 36
                                                    WITH REVERSE-VIDEO  
              ACCEPT CUST-FAX                       AT LINE 16 COL 36
           END-IF.
           DISPLAY CUST-FAX                         AT LINE 16 COL 36.

           DISPLAY CUST-EMAIL                       AT LINE 17 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
              MOVE SPACES                  TO  CUST-EMAIL
              DISPLAY CUST-EMAIL                    AT LINE 17 COL 36
                                                    WITH REVERSE-VIDEO  
              ACCEPT CUST-EMAIL                     AT LINE 17 COL 36
           END-IF
           DISPLAY CUST-EMAIL                       AT LINE 17 COL 36.

           DISPLAY SEAT-TYPE                        AT LINE 18 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
             MOVE SPACES                   TO  SEAT-TYPE
             DISPLAY SEAT-TYPE                      AT LINE 18 COL 36
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT SEAT-TYPE                      AT LINE 18 COL 36
              IF SEAT-TYPE = "CO" OR SEAT-TYPE = "FC"
                 CONTINUE
              ELSE
                 DISPLAY "ENTER VALID SEAT TYPE"    AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF           
             END-PERFORM
           END-IF.
           DISPLAY SEAT-TYPE                        AT LINE 18 COL 36.
           
           DISPLAY FOOD                             AT LINE 19 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'
             MOVE SPACES                   TO  FOOD
             DISPLAY FOOD                           AT LINE 19 COL 36
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT FOOD                           AT LINE 19 COL 36
              IF FOOD = SPACES
                 DISPLAY "ENTER FOOD PREFERENCE"    AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM
           END-IF.
           DISPLAY FOOD                             AT LINE 19 COL 36.


           DISPLAY PAYMENT-TYPE                     AT LINE 20 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'           
             MOVE SPACES                   TO  PAYMENT-TYPE
             DISPLAY PAYMENT-TYPE                   AT LINE 20 COL 36
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT PAYMENT-TYPE                   AT LINE 20 COL 36
              IF PAYMENT-TYPE = "MST" OR PAYMENT-TYPE = "VSA" OR
                 PAYMENT-TYPE = "CHK"
                 CONTINUE
              ELSE
                 DISPLAY "ENTER VALID PAYMENT TYPE" AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM           
           END-IF.
           DISPLAY PAYMENT-TYPE                     AT LINE 20 COL 36.

           DISPLAY PAYMENT-AMT                      AT LINE 21 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'
              MOVE SPACES                  TO  PAYMENT-AMT
              DISPLAY PAYMENT-AMT                   AT LINE 21 COL 36
                                                    WITH REVERSE-VIDEO  
              ACCEPT PAYMENT-AMT                    AT LINE 21 COL 36
           END-IF.
           DISPLAY PAYMENT-AMT                      AT LINE 21 COL 36.

           DISPLAY FLIGHT-ID                        AT LINE 22 COL 36
                                                    WITH REVERSE-VIDEO  
           DISPLAY "<ENT>CONTINUE <M>MODIFY  FIELDS<Q>QUIT :  "
                                                    AT LINE 24 COL 20.
           ACCEPT RESP                              AT LINE 24 COL 60.
           IF RESP = 'Q'
              EXIT PARAGRAPH
           END-IF.                                         
           IF RESP = 'M'
             MOVE SPACES                   TO  FLIGHT-ID
             DISPLAY FLIGHT-ID                      AT LINE 22 COL 36
                                                    WITH REVERSE-VIDEO  
             PERFORM WITH TEST AFTER UNTIL DATA-VALID           
              SET DATA-VALID               TO  TRUE
              ACCEPT FLIGHT-ID                      AT LINE 22 COL 36
              IF FLIGHT-ID = SPACES
                 DISPLAY "ENTER VALID FLIGHT ID   " AT LINE 25 COL 20
                 DISPLAY "<ENT>REENTER/'Q'QUIT:"    AT LINE 25 COL 55
                 ACCEPT RESP                        AT LINE 25 COL 76
                 IF RESP = "Q"
                    MOVE "2"               TO  MENU-CHOICE2
                    SET DATA-VALID         TO  TRUE
                    EXIT PARAGRAPH
                 ELSE
                    SET DATA-INVALID       TO  TRUE
                    DISPLAY BLNK-LINE               AT LINE 25 COL 1
                 END-IF
              END-IF
             END-PERFORM
           END-IF.
           DISPLAY FLIGHT-ID                        AT LINE 22 COL 36.

           MOVE SPACES                     TO  MENU-CHOICE2.
           PERFORM WITH TEST AFTER UNTIL MENU-CHOICE2 = '1' OR
                                         MENU-CHOICE2 = '2'
           DISPLAY "1)SAVE  2)CANCEL 3)PRINT      :-                 "
                                                    AT LINE 24 COL 20
              ACCEPT MENU-CHOICE2                   AT LINE 24 COL 52
              IF PRINT-REC
                 OPEN OUTPUT PRINT-FILE
                 PERFORM PRINT-RESERVATIONS
                 CLOSE PRINT-FILE
                 DISPLAY "RESERVATION PRINTED! PRESS A KEY TO CONTINUE"
                                                    AT LINE 24 COL 20
                 ACCEPT RESP
              END-IF
           END-PERFORM.
           
       DISPLAY-FIELDS.
           DISPLAY CUST-RES-ID                      AT LINE 6  COL 36.
           DISPLAY CUST-FNAME                       AT LINE 7  COL 36.
           DISPLAY CUST-LNAME                       AT LINE 8  COL 36.
           DISPLAY DOB-MM                           AT LINE 9  COL 36.
           DISPLAY DOB-DD                           AT LINE 9  COL 39.
           DISPLAY DOB-YY                           AT LINE 9  COL 42.
           DISPLAY STREET                           AT LINE 11 COL 36.
           DISPLAY CITY                             AT LINE 12 COL 36.
           DISPLAY STATE                            AT LINE 13 COL 36.
           DISPLAY ZIP                              AT LINE 14 COL 36.
           DISPLAY CUST-TEL                         AT LINE 15 COL 36.
           DISPLAY CUST-FAX                         AT LINE 16 COL 36.
           DISPLAY CUST-EMAIL                       AT LINE 17 COL 36.
           DISPLAY SEAT-TYPE                        AT LINE 18 COL 36.
           DISPLAY FOOD                             AT LINE 19 COL 36.
           DISPLAY PAYMENT-TYPE                     AT LINE 20 COL 36.
           DISPLAY PAYMENT-AMT                      AT LINE 21 COL 36.
           DISPLAY FLIGHT-ID                        AT LINE 22 COL 36.

       GET-CUST-RES-ID.
           MOVE FLIGHT-ID                  TO  CUST-FLIGHT.      
           MOVE HIGH-VALUES                TO  CUST-NO.         
           MOVE FLIGHT-ID                  TO  HOLD-FLT-ID.      
           START CUSTOMER KEY IS <= CUST-RES-ID
                 INVALID KEY
                   MOVE SCUSTOMER-RECORD   TO  CUSTOMER-RECORD
                   MOVE FLIGHT-ID          TO  CUST-FLIGHT     
                   MOVE "001"              TO  CUST-NO          
                 NOT INVALID KEY
                   READ CUSTOMER NEXT
                   IF HOLD-FLT-ID = CUST-FLIGHT
                      MOVE CUST-NO         TO  W-CNTR
                      ADD +1               TO  W-CNTR
                      MOVE SCUSTOMER-RECORD   TO  CUSTOMER-RECORD
                      MOVE FLIGHT-ID       TO  CUST-FLIGHT      
                      MOVE W-CNTR          TO  CUST-NO          
                   ELSE
                      MOVE SCUSTOMER-RECORD   TO  CUSTOMER-RECORD
                      MOVE FLIGHT-ID       TO  CUST-FLIGHT     
                      MOVE "001"           TO  CUST-NO
                   END-IF
           END-START.
           DISPLAY CUST-RES-ID                      AT LINE 6  COL 36.

       PRINT-RESERVATIONS.

           INITIALIZE PRINT-RECORD.
           WRITE PRINT-RECORD AFTER ADVANCING 3 LINES.
           INITIALIZE PRINT-RECORD.
           STRING "          TRAVELS      " DELIMITED BY SIZE
                  "           "                     DELIMITED BY SIZE
                  W-SYS-MM                          DELIMITED BY SIZE
                  "/"                               DELIMITED BY SIZE
                  W-SYS-DD                          DELIMITED BY SIZE
                  "/"                               DELIMITED BY SIZE
                  W-SYS-YY                          DELIMITED BY SIZE
                  INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
           
           MOVE    "       CUSTOMER RESERVATION   " TO PRINT-RECORD.
           WRITE PRINT-RECORD.
           MOVE    "       --------------------   " TO PRINT-RECORD.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
           WRITE PRINT-RECORD AFTER ADVANCING 3 LINES.
                      
           STRING  "RESERVATION ID :"               DELIMITED BY SIZE
                   CUST-RES-ID                      DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
           STRING  "NAME           :"               DELIMITED BY SIZE
                   CUST-FNAME                       DELIMITED BY SPACES
                   " "                              DELIMITED BY SIZE
                   CUST-LNAME                       DELIMITED BY SPACES
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.

           STRING  "DATE OF BIRTH  :"               DELIMITED BY SIZE
                   DOB-MM                           DELIMITED BY SIZE
                   "/"                              DELIMITED BY SIZE
                   DOB-DD                           DELIMITED BY SIZE
                   "/"                              DELIMITED BY SIZE
                   DOB-YY                           DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.

           MOVE "ADDRESS"                  TO       PRINT-RECORD.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.

           STRING  "  STREET       :"               DELIMITED BY SIZE
                   STREET                           DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
                      
           STRING  "  CITY         :"               DELIMITED BY SIZE
                   CITY                             DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
           
           STRING  "  STATE        :"               DELIMITED BY SIZE
                   STATE                            DELIMITED BY SIZE
                   "  ZIP: "                        DELIMITED BY SIZE
                   ZIP                              DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
                      
           STRING  "TELEPHONE      :"               DELIMITED BY SIZE
                   CUST-TEL                         DELIMITED BY SIZE
                   " FAX: "                         DELIMITED BY SIZE
                   CUST-FAX                         DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
                      
           STRING  "E-MAIL         :"               DELIMITED BY SIZE
                   CUST-EMAIL                       DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
           
           STRING  "SEAT-TYPE      :"               DELIMITED BY SIZE
                   SEAT-TYPE                        DELIMITED BY SIZE
                   " (CO-COACH / FC-FIRST CLASS) "  DELIMITED BY SIZE
                   "        FOOD-TYPE      :"       DELIMITED BY SIZE
                   FOOD                             DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
                                         
           STRING  "PAYMENT TYPE   :"               DELIMITED BY SIZE
                   PAYMENT-TYPE                     DELIMITED BY SIZE
                   " (MST-MASTER / VSA-VISA /CHK-CHECK) "
                                                    DELIMITED BY SIZE
                   "PAYMENT AMOUNT :"               DELIMITED BY SIZE
                   PAYMENT-AMT                      DELIMITED BY SIZE
                   "$"                              DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.
           
           STRING  "FLIGHT ID      :"               DELIMITED BY SIZE
                   FLIGHT-ID                        DELIMITED BY SIZE
                   INTO PRINT-RECORD
           END-STRING.
           WRITE PRINT-RECORD.
           INITIALIZE PRINT-RECORD.                     

           
       CLEAR-SCREEN.
           MOVE 1                          TO  CNTR.
           PERFORM UNTIL CNTR > 25           
               DISPLAY BLNK-LINE AT LINE CNTR COL 1
               ADD +1                      TO  CNTR
           END-PERFORM.
           
