       IDENTIFICATION DIVISION. 
       PROGRAM-ID. Screen2. 
        
       ENVIRONMENT DIVISION. 
       CONFIGURATION SECTION. 
        
      *   The SPECIAL-NAMES paragraph that follows provides for the 
      *   capturing of the F10 function key and for positioning of the 
      *   cursor. 
        
       SPECIAL-NAMES. 
         
           CURSOR IS CURSOR-POSITION 
        
           CRT STATUS IS CRT-STATUS. 
        
       DATA DIVISION. 
       WORKING-STORAGE SECTION. 
        
      *   CURSOR-LINE specifies the line and CURSOR-COL specifies the 
      *   column of the cursor position. 
        
       01  CURSOR-POSITION. 
           02  CURSOR-LINE    PIC 99. 
           02  CURSOR-COL     PIC 99. 
        
      *   Normal termination of the ACCEPT statement will result in a value 
      *   of '0' in KEY1.  When the user presses F10, the value in KEY1 will 
      *   be '1' and FKEY-10 will be true. 
        
       01  CRT-STATUS. 
           03 KEY1            PIC X. 
           03 KEY2            PIC X. 
              88 FKEY-10      VALUE 11. 
           03 filler          PIC XX. 
        
      *   The following data items are for a "Daily Calendar."  It shows 
      *   the day's appointments and allows appointments to be made, 
      *   canceled, and printed. 
        01 DATA-STORE.        
	       03 ACCEPT-ITEM1  PIC X. 
	       03 APPT-NAME     PIC X(80). 
	       03 APPT-DESC     PIC X(80). 
	       03 APPT-DATE.
	          05 APPT-DAY      PIC 99 COMP. 
	          05 APPT-MONTH    PIC 99 COMP. 
	          05 APPT-YEAR     PIC 99 COMP. 
               03 APPT-TIME.
	          05 APPT-HOUR     PIC 99. 
	          05 APPT-MINUTE   PIC 99. 
	          05 APPT-MERIDIEM PIC XX. 
	       03 APPT-VERIFY   PIC X. 
	       03 EMPTY-LINE    PIC X(80). 
        
      *   The SCREEN SECTION designs the Daily Calendar, with a menu 
      *   screen from which the user selects an option:  to show 
      *   appointments, schedule an appointment, cancel an appointment, 
      *   and print the appointments. 
        
       SCREEN SECTION. 
        
       01 MENU-SCREEN BLANK SCREEN 
             FOREGROUND-COLOR 7 BACKGROUND-COLOR 1. 
          02 MENU-SCREEN-2. 
             03 TITLE-BAR 
                FOREGROUND-COLOR 7 BACKGROUND-COLOR 4. 
                04 LINE 1 PIC X(80) FROM EMPTY-LINE. 
                04 LINE 1 COLUMN 32 VALUE "Daily Calendar". 
        
             03 LINE 7  COLUMN 26 
                PIC X TO ACCEPT-ITEM1. 
             03 VALUE " Show appointments for a day ". 
             03 LINE 9  COLUMN 26 
                PIC X TO ACCEPT-ITEM1. 
             03 VALUE " Schedule an appointment ". 
             03 LINE 11 COLUMN 26 
                PIC X TO ACCEPT-ITEM1. 
             03 VALUE " Cancel an appointment ". 
             03 LINE 13 COLUMN 26 
                PIC X TO ACCEPT-ITEM1. 
             03 VALUE " Print your appointments ". 
             03 HELP-TEXT 
                FOREGROUND-COLOR 6 BACKGROUND-COLOR 0. 
               04 LINE 19 COLUMN 12 
                  VALUE 
           " Use the arrow keys to move the cursor among menu ite ms. ". 
               04 LINE 20 COLUMN 12 
                  VALUE 
           " Press <Return> when the cursor is at the desired item.  ". 
               04 LINE 21 COLUMN 12 
                  VALUE 
           " Press <F10> to exit.                                    ". 
        
       01 SCHEDULE-SCREEN BLANK SCREEN. 
          02 TITLE-BAR 
             FOREGROUND-COLOR 7 BACKGROUND-COLOR 4. 
             03 LINE 1 PIC X(80) FROM EMPTY-LINE. 
             03 LINE 1 COLUMN 30 VALUE "Schedule Appointment". 
        
          02 FIELDS-TEXT 
             FOREGROUND-COLOR 7 BACKGROUND-COLOR 1. 
             03 LINE 5 VALUE " Description of Appointment: ". 
             03 LINE PLUS 4 VALUE " Date of Appointment (DD/MM/YY): ". 
             03 COLUMN PLUS 5 VALUE "/  /". 
             03 LINE PLUS 2 VALUE " Time of Appointment (HH:MM mm): ". 
             03 COLUMN PLUS 5 VALUE ":". 
        
          02 FIELDS-INPUT 
             FOREGROUND-COLOR 7 BACKGROUND-COLOR 0 AUTO. 
             03 LINE 6  PIC X(80) TO APPT-NAME REQUIRED. 
             03 LINE 7  PIC X(80) TO APPT-DESC. 
             03 LINE 9  COLUMN 36 PIC 99 USING APPT-DAY FULL . 
             03 LINE 9  COLUMN 39 PIC 99 USING APPT-MONTH FULL. 
             03 LINE 9  COLUMN 42 PIC 99 USING APPT-YEAR. 
             03 LINE 11 COLUMN 36 PIC 99 USING APPT-HOUR. 
             03 LINE 11 COLUMN 39 PIC 99 USING APPT-MINUTE. 
             03 LINE 11 COLUMN 42 PIC XX USING APPT-MERIDIEM. 
        
          02 HELP-TEXT 
             FOREGROUND-COLOR 6 BACKGROUND-COLOR 0. 
             03 LINE 16 COLUMN 18 
                VALUE " Use Cursor Keys to move within the fields. ". 
             03 LINE 17 COLUMN 18 
                VALUE " Press <Tab> to enter next field.           ". 
             03 LINE 18 COLUMN 18 
                VALUE " Press <Return> when finished.              ". 
        
       01 VERIFY-SUBSCREEN FOREGROUND-COLOR 7 BACKGROUND-COLOR 1. 
          02 LINE 16 COLUMN 1 ERASE EOS. 
          02 LINE 17 COLUMN 25 VALUE " Is this entry correct? (Y/N): ". 
          02 PIC X USING APPT-VERIFY AUTO. 
        
        
       PROCEDURE DIVISION. 
       P0. 
        
           DISPLAY MENU-SCREEN. 
        
      *   The cursor position is not within an item on the screen, so the 
      *   first item in the menu will be accepted first. 
        
           MOVE 0 TO CURSOR-LINE, CURSOR-COL. 
        
      *   The user moves the cursor with the arrow keys to the 
      *   desired menu item (to show, schedule, cancel, or print 
      *   appointments) and selects the item by pressing <Return>. 
      *   If the user wishes to exit without selecting an option, 
      *   the user can press the F10 function key. 
        
           ACCEPT MENU-SCREEN. 
        
           IF KEY1 EQUAL "0" 
              PERFORM OPTION_CHOSEN 
        
           ELSE IF KEY1 EQUAL "1" AND FKEY-10 
              DISPLAY "You pressed the F10 key; exiting..." LINE 22. 
        
           STOP RUN. 
        
       OPTION_CHOSEN. 
        
      *   For brevity, the sample program includes complete code 
      *   for the "Schedule Appointment" screen only.  A complete 
      *   program for a calendar would also include code for 
      *   displaying, canceling, and printing the day's appointments. 
        
           IF CURSOR-LINE = 7 
              DISPLAY "You selected Show Appointments" LINE 22. 
        
           IF CURSOR-LINE = 9 
              MOVE "01" TO APPT-DAY 
              MOVE "01" TO APPT-MONTH 
              MOVE "94" TO APPT-YEAR 
              MOVE "12" TO APPT-HOUR 
              MOVE "00" TO APPT-MINUTE 
              MOVE "AM" TO APPT-MERIDIEM 
              DISPLAY SCHEDULE-SCREEN 
        
      *   The user types the description, date, and time of the 
      *   appointment. 
        
              ACCEPT SCHEDULE-SCREEN 
        
              MOVE "Y" TO APPT-VERIFY 
              DISPLAY VERIFY-SUBSCREEN 
        
      *   The user is asked, "Is this entry correct?"  Answer is 
      *   Y or N. 
        
              ACCEPT VERIFY-SUBSCREEN. 
        
           IF CURSOR-LINE = 11 
              DISPLAY "You selected Cancel Appointments" LINE 22. 
        
           IF CURSOR-LINE = 13 
              DISPLAY "You selected Print Appointments" LINE 22. 
        
       END PROGRAM Screen2. 

