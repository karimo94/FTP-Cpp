#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/poll.h>     // for poll( )
#include <fcntl.h>
#include <fstream>
using namespace std;
/*
Karim Oumghar
Css 432
Spring 2016
Program 5 - Ftp client
*/

//global variables (mostly for passive mode)
int dataSd, dataSd2, dataSd3;
int a1, a2, a3, a4, p1, p2;



int socketPoll(int sd);

int main(int argc, char *argv[])
{
    //obtain the url 'ftp.tripod.com'
    //connect to the ftp server
    //enter name
    //password
    if(argc != 2)
    {
        cerr << "Usage: ftp.name.com needed" << endl;
        exit(0);
    }
    char *ftpUrl = argv[1];

    cout << "Attempting to connect to: " << ftpUrl << endl;
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSd < 0)
    {
        cerr << "Problem creating socket" << endl;
        exit(0);
    }
    struct hostent *host = gethostbyname(ftpUrl);
    struct sockaddr_in addr;
    bzero((char*)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    addr.sin_port = htons(21);
    //attempt to connect
    if(connect(clientSd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        cerr << "Error connecting to the ftp server!" << endl;
        exit(0);
    }
    char buffer[1500];
    read(clientSd, (char*)&buffer, sizeof(buffer));
    cout << buffer;//prints 220 welcome to tripod
    //user login.............................
    cout << "Name ("<< ftpUrl << ":" << getenv("USER") << "): ";
    char name[6];
    cin >> name;
    char user[11];
    strcpy(user, "USER ");
    strcat(user, name);
    strcat(user, "\r\n");
    //send the username & receive acknowledgement
    //when sending to the server, ALWAYS USE STRLEN!!! 
    //learned this the hard way...
    write(clientSd, (char*)&user, strlen(user));
    read(clientSd, (char*)&buffer, sizeof(buffer));
    cout << buffer;//username ok
    
    //now enter your password
    while(true)
    {
        cout << "Password: ";
        char pword[9];
        char pass[16];
        cin >> pword;
        strcpy(pass, "PASS ");
        strcat(pass, pword);
        strcat(pass, "\r\n");
        //send the password & receive acknowledgment
        write(clientSd, (char*)&pass, strlen(pass));
        read(clientSd, (char*)&buffer, sizeof(buffer));
        cout << buffer;//response
        //we want a correct password
        string error = "501";
        string response = buffer;
        if(!strstr(response.c_str(), error.c_str()))
        {
            break;
        }
    }
    cout << buffer;
    //poll here...
    while(socketPoll(clientSd) == 1)
    {
        read(clientSd, (char*)&buffer, sizeof(buffer));
        cout << buffer << endl;
    }
    if(socketPoll(clientSd) == -1)
    {
        cerr << "Error polling the socket" << endl;
    }


    //main program loop that handles command logic
    while(true)
    {
        cout << "ftp> ";
        string command;
        char file[30];
        char folder[30];
        cin >> command;
        if(command == "cd")
        {
            //send the command CWD subdir
            cout << "Specify directory: ";
            cin >> folder;
            char dir[30];
            char data[500];
            strcpy(dir, "CWD ");
            strcat(dir, folder);
            strcat(dir, "\r\n");
            //send...receive
            write(clientSd, (char*)&dir, strlen(dir));
            read(clientSd, (char*)&data, sizeof(data));
            cout << data;
            continue;
        }
        if(command == "ls")
        {
            //enter passive mode first
            //then send the command LIST
            char passive[30];
            char temp[100];
            strcpy(passive, "PASV");
            strcat(passive, "\r\n");
            write(clientSd, (char*)&passive, strlen(passive));
            read(clientSd, (char*)&temp, sizeof(temp));
            cout << temp;
            
            //passively connect on the port sent back
            //scan the temp
            sscanf(temp, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", 
            &a1, &a2, &a3, &a4, &p1, &p2);
            
            int servPort = (p1 * 256) + p2;
            struct sockaddr_in servAddr2;
            //clear the structure
            bzero((char*)&servAddr2, sizeof(servAddr2));
            servAddr2.sin_family = AF_INET;
            servAddr2.sin_port = htons(servPort);
            memcpy(&servAddr2.sin_addr, host->h_addr, host->h_length);
            //create a socket to connect to FTP server
            dataSd = socket(AF_INET, SOCK_STREAM, 0);
            if(dataSd < 0)
            {
                cout << "Error generating socket" << endl;
                exit(0);
            }
            //connect to the FTP server passively
            if(connect(dataSd, (struct sockaddr*)&servAddr2, sizeof(servAddr2)) < 0)
            {
                cout << "Error establishing a connection!" << endl;
            }
            
            //receive the message passive mode from server
            char list[10];
            char temp2[500];
            char feedback[200];
            string f_dir_list;
            strcpy(list, "LIST");
            strcat(list, "\r\n");
            write(clientSd, (char*)&list, strlen(list));
            
            while(read(dataSd, (char*)&temp2, sizeof(temp2)) > 0)
            {
                f_dir_list.append(temp2);
            }
            cout << f_dir_list << endl;
            //close the dataSd after we're done
            close(dataSd);
            read(clientSd, (char*)&feedback, sizeof(feedback));
            cout << feedback;
            continue;
        }
        if(command == "get")
        {
            //enter passive mode first
            //then send the command RETR file
            
            //obtain the file name
            cout << "File: ";
            cin >> file;
            
            //first set type to 'I' (image)
            char type_i[12];
            strcpy(type_i, "Type I");
            strcat(type_i, "\r\n");
            char verify[100];
            write(clientSd, (char*)&type_i, strlen(type_i));
            read(clientSd, (char*)&verify, sizeof(verify));
            cout << verify;
            
            //error checking
            string error = "200";
            string response = verify;
            if(!strstr(response.c_str(), error.c_str()))
            {   
                cerr << "Error setting up binary mode" << endl;
                break;
            }
            
            //go into passive mode and connect passively
            char passive[30];
            char temp[100];
            strcpy(passive, "PASV");
            strcat(passive, "\r\n");
            write(clientSd, (char*)&passive, strlen(passive));
            read(clientSd, (char*)&temp, sizeof(temp));
            cout << temp;
            
            //passively connect on the port sent back
            //scan the temp
            sscanf(temp, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", 
            &a1, &a2, &a3, &a4, &p1, &p2);
            
            int servPort = (p1 * 256) + p2;
            struct sockaddr_in servAddr3;
            //clear the structure
            bzero((char*)&servAddr3, sizeof(servAddr3));
            servAddr3.sin_family = AF_INET;
            servAddr3.sin_port = htons(servPort);
            memcpy(&servAddr3.sin_addr, host->h_addr, host->h_length);
            //create a socket to connect to FTP server
            dataSd2 = socket(AF_INET, SOCK_STREAM, 0);
            if(dataSd2 < 0)
            {
                cout << "Error generating socket" << endl;
                exit(0);
            }
            //connect to the FTP server passively
            if(connect(dataSd2, (struct sockaddr*)&servAddr3, sizeof(servAddr3)) < 0)
            {
                cout << "Error establishing a connection!" << endl;
            }
            
            //Let's get a file...
            
            char retrive[10];
            char temp3[1000];
            char feedback[200];
            
            strcpy(retrive, "RETR ");
            strcat(retrive, file);
            strcat(retrive, "\r\n");
            
            
            write(clientSd, (char*)&retrive, strlen(retrive));
            
            //open a file stream
            std::ofstream textFile(file, ios_base::out | ios::binary);
            
            int numOfChars;
            read(dataSd2, (char*)&temp3, sizeof(temp3));
            numOfChars = strlen(temp3);
            
            //write to the textfile based on how many characters exist
            textFile.write(temp3, numOfChars);    
            //close the text file
            //close the passive data socket
            textFile.close();
            close(dataSd2);
            read(clientSd, (char*)&feedback, sizeof(feedback));
            cout << feedback;
            cout << "File downloaded successfully..." << endl;
            continue;
        }
        if(command == "put")
        {
            //grab the file name first
            cout << "File: ";
            cin >> file;
            
            char contents[1000];
            //setup file reading tools
            FILE *textFile;
            int fileSize;
            textFile = fopen(file, "rb");
            if(textFile)
            {
                fileSize = fread(contents, sizeof(contents), 1, textFile);
            }
            else
            {
                cerr << "No such file exists locally to read from!" << endl;
                continue;
            }
            
            //setup binary "I" mode
            char type_i[12];
            strcpy(type_i, "Type I");
            strcat(type_i, "\r\n");
            char verify[100];
            write(clientSd, (char*)&type_i, strlen(type_i));
            read(clientSd, (char*)&verify, sizeof(verify));
            cout << verify;
            
            //error checking
            string error = "200";
            string response = verify;
            if(!strstr(response.c_str(), error.c_str()))
            {   
                cerr << "Error setting up binary mode" << endl;
                continue;
            }
            
            //setup passive connection
            char passive[30];
            char temp[100];
            strcpy(passive, "PASV");
            strcat(passive, "\r\n");
            write(clientSd, (char*)&passive, strlen(passive));
            read(clientSd, (char*)&temp, sizeof(temp));
            cout << temp;
            
            //passively connect on the port sent back
            //scan the temp
            sscanf(temp, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", 
            &a1, &a2, &a3, &a4, &p1, &p2);
            
            int servPort = (p1 * 256) + p2;
            struct sockaddr_in servAddr4;
            //clear the structure
            bzero((char*)&servAddr4, sizeof(servAddr4));
            servAddr4.sin_family = AF_INET;
            servAddr4.sin_port = htons(servPort);
            memcpy(&servAddr4.sin_addr, host->h_addr, host->h_length);
            //create a socket to connect to FTP server
            dataSd3 = socket(AF_INET, SOCK_STREAM, 0);
            if(dataSd3 < 0)
            {
                cout << "Error generating socket" << endl;
                exit(0);
            }
            //connect to the FTP server passively
            if(connect(dataSd3, (struct sockaddr*)&servAddr4, sizeof(servAddr4)) < 0)
            {
                cout << "Error establishing a connection!" << endl;
            }
            
            //lets put a file on the server...
            //send the STOR command
            char stor[100];
            char feedback[500];
            char feedback2[500];
            strcpy(stor, "STOR ");
            strcat(stor, file);
            strcat(stor, "\r\n");
            write(clientSd, (char*)&stor, strlen(stor));
            read(clientSd, (char*)&feedback, sizeof(feedback));
            cout << feedback;
            int tempSd = clientSd;
            clientSd = dataSd3;
            write(clientSd, (char*)&contents, strlen(contents));
            clientSd = tempSd;
            cout << "File uploaded successfully..." << endl;
            //close the file and passive data socket
            fclose(textFile);
            close(dataSd3);
            read(clientSd, (char*)&feedback2, sizeof(feedback2));
            cout << feedback2;
            continue;
        }
        if(command == "close")
        {
            //send the command close, but don't quit the FTP
            char quit[20];
            char temp[20];
            strcpy(quit, "QUIT");
            strcat(quit, "\r\n");
            write(clientSd, (char*)&quit, strlen(quit));
            read(clientSd, (char*)&temp, sizeof(temp));
            cout << temp;
            shutdown(clientSd, SHUT_WR);//passive close
            continue;
        }
        if(command == "quit")
        {
            //close the connection and quit the FTP
            char quit[10];
            char temp[20];
            strcpy(quit, "QUIT");
            strcat(quit, "\r\n");
            write(clientSd, (char*)&quit, strlen(quit));
            read(clientSd, (char*)&temp, sizeof(temp));
            cout << temp;
            break;
        }
        else
        {
            cout << "?Invalid command" << endl;
            continue;
        }
    }
    close(clientSd);
    return 0;
}
//socketPoll() - when receiving an ack from the server after password
int socketPoll(int sd) 
{
    struct pollfd pfd[1];
    pfd[0].fd = sd;             // declare I'll check the data availability of sd
    pfd[0].events = POLLIN; // declare I'm interested in only reading from sd
    pfd[0].revents = 0;

    // check it immediately and return a positive number if sd is readable,
    // otherwise return 0 or a negative number
    return poll( pfd, 1, 1000 );
}
