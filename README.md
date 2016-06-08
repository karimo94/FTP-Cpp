# FTP-Cpp
Implemented a FTP client in C++ for the CSS432 Networking course at UWB

Karim Oumghar

Css 432

Ftp flow/explanation.

Preface: Originally I had planned to use OOP and split up various components such as the socket, the connection, the command parser, etc. However, due to time constraints and the difficulty of debugging such a program, I decided to use a somewhat linear/brute force approach to it. Basically, one main class, the program logic residing in main.cpp (yes, not exactly best practice, but it worked best for me). I organized the program so that it should be simple enough to follow in that the FTP logic is inside a infinite loop, which each command logic broken down into if statements (with plenty of supporting commentary). 

I managed to get all commands as instructed on the Program 5 document working properly, so I think there's definitely room to expand for more commands and possibly better functionality and code organization. There are still just a few subtle bugs however I didn't have enough time to really debug every test case. Personally, one of the things I found odd about this assignment is the directions on commands such as 'ls' or 'get' where they say to fork a child process. Forking a child did not work for me in the least bit at all for some reason; perhaps it might be better practice but I simply never found a reason to use it. 

Flow:

    int main():

    check for correct number of arguments
    obtain the ftp url
    create a clientSd and check for errors
    initialize the hostent and connect through port 21
    read into a buffer the username login
    write back to the server (using strlen(username))
    read back from the server (username ok)
    
    while true
        prompt for password
        send the password
        read from server
        if there are no error messages, break from this loop
    poll the socket to read from the server (by now we should have the intro text)
    
    while true //main ftp logic
        print "ftp>"
        take in the command typed
        
        if cd:
            send the command CWD subdir with "\r\n"
            read feedback from the server
        if ls:
            enter passive mode first
                passively connect on the port sent back from the server
                create a new socket based on the new connection
                connect
            send LIST to the server, write using clientSd, receive using new socket
            read from the server
        if get:
            first set type to "I" (image, binary mode)
            enter passive mode 
                passively connect on the port sent back from the server
                create a new socket based on the new connection
                connect
            send RETR with the specified file
            write using clientSd
            open a file stream
            read in from the server using new socket
            write to text file based on how big the file is
            read in feedback from the server (download success)
        if put:
            open a file stream to the filename passed in
            store contents in a buffer
            first set type to "I" (image, binary mode)
            enter passive mode 
                passively connect on the port sent back from the server
                create a new socket based on the new connection
                connect
            
            write STOR with filename using clientSd
            read using clientsd
            set the clientsd to the new socket from the passive connection
            write the text file contents to the server
            close the text file and new data socket
            read in feedback from the server (upload success)
        if close:
            send the QUIT command
            read back from server
            passively shutdown clientSd
            continue looping
        if quit:
            send the QUIT command to server
            read back from the server
            break ***from the main loop, exit the ftp
        else:
            print invalid command 
        
        
        close the clientSd
    
