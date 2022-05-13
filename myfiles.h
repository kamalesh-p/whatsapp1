#pragma once

class loginId {
private:
    int wait;
    const char filename[20] = "login.txt\0";
    const char tempfile[20] = "rough_login.txt\0";
    const char idfile[20] = "id.txt\0";
    FILE* stream1, * stream2;
    errno_t err;
    void Start() {
        while (this->wait) {
            cout << "waiting..." << endl;
            Sleep(50);
        }
        this->wait = 1;
    }
    void End() {
        this->wait = 0;
        Sleep(100);
    }
public:
    loginId() {
        this->wait = 0;
    }
    int Find(char name[15], char password[15], int num = 1) {
        this->Start();
        int boolean = 0;
        char buff[DEFAULT_BUFLEN], id[DEFAULT_BUFLEN], thisname[DEFAULT_BUFLEN], thispassword[DEFAULT_BUFLEN];
        
        err = fopen_s(&stream1, this->filename, "r");
        if (err == 0) {
            while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                int num = 0;
                analyse(buff, id, thisname, thispassword, &num);
                if (strcmp(thisname, name) == 0) {
                    boolean = 1;
                    sscanf_s(id, "%d", &boolean);
                    break;
                }
            }
        }
        else cout << "Unable to open file(" << this->filename << ")" << endl;

        fclose(stream1);

        this->End();
        return boolean;
    }
    int Find(char name[15]) {
        return Find(name, NULL, 0);
    }
    void Delete(char name[15]) {
        this->Start();
        int boolean = 0;
        char emty[2] = "\0";
        char buff[DEFAULT_BUFLEN] = "\0";
        char newbuff[DEFAULT_BUFLEN];
        int num;
        char id[DEFAULT_BUFLEN], thisname[DEFAULT_BUFLEN], thispass[DEFAULT_BUFLEN];

        //copy from login file to temporary file
        err = fopen_s(&stream2, this->tempfile, "w");
        if (err == 0)  fprintf(stream2, emty);
        fclose(stream2);

        err = fopen_s(&stream1, this->filename, "r");
        if (err != 0) { cout << "1. error while opening file\n"; goto deleteLabel; }

        err = fopen_s(&stream2, this->tempfile, "a");
        if (err != 0) { cout << "2. error while opening file\n"; goto deleteLabel; }
        while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
            num = 0;
            analyse(buff, id, thisname, thispass, &num);
            if (strcmp(name, thisname) == 0) {
                cout << "Account Deleted (" << name << ")\n";
            }
            else {
                //save in temporary file
                sprintf_s(newbuff, " %s", buff);
                fprintf(stream2, newbuff);
            }
        }
        fclose(stream2);

        fclose(stream1);

        //paste from temporary file to login file
        err = fopen_s(&stream2, this->filename, "w");
        if (err == 0)  fprintf(stream2, emty);
        fclose(stream2);

        err = fopen_s(&stream1, this->tempfile, "r");
        if (err != 0) { cout << "3. error while opening file\n"; goto deleteLabel; }

        err = fopen_s(&stream2, this->filename, "a");
        if (err != 0) { cout << "4. error while opening file\n"; goto deleteLabel; }
        while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
            //save in login file
            sprintf_s(newbuff, " %s", buff);
            fprintf(stream2, newbuff);
        }
        fclose(stream2);

        fclose(stream1);

    deleteLabel:
        this->End();
    }
    void Append(char name[15], char password[15]) {
        this->Start();
        char buff[DEFAULT_BUFLEN] = "\0";
        int num = -1;

        //read ID number
        err = fopen_s(&stream2, this->idfile, "r");     //open stream2
        if (err != 0) { cout << "1. error while opening file\n"; goto appendLabel; }
        while (fscanf_s(stream2, "%s", buff, _countof(buff)) != EOF) {
            sscanf_s(buff, "%d", &num);
        }
        fclose(stream2);             //close stream2

        num++;
        //write ID number
        err = fopen_s(&stream2, this->idfile, "w");     //open stream2
        if (err != 0) { cout << "2. error while opening file\n"; goto appendLabel; }
        sprintf_s(buff, "%d ", num);
        fprintf(stream2, buff);
        fclose(stream2);             //close stream2

        //append login details
        err = fopen_s(&stream1, this->filename, "a");   //open stream1
        if (err != 0) { cout << "3. error while opening file\n"; goto appendLabel; }
        sprintf_s(buff, " %d_%s_%s", num, name, password);
        fprintf(stream1, buff);
        fclose(stream1);             //close stream1

    appendLabel:
        this->End();
    }

    void ChangePassword(char name[15], char password[15]) {
        this->Start();

        this->End();
    }
};


class Myfiles {
private:
    FILE* stream1, * stream2;
    errno_t err;
    char login[10] = "login.txt";
    char roughlogin[20] = "rough_login.txt";
    char message[12] = "message.txt";
    char roughmessage[18] = "rough_message.txt";
    int wait = 0;
    int wait1 = 0;
    loginId loginid;
public:
    void fetchmessageswhileoffline(char thistoname[15], SOCKET thisClientSocket) {
        while (this->wait1) {
            cout << "fetchmessageswhileoffline waiting...." << endl;
            Sleep(50);
        }
        this->wait1 = 1;
        char buff[1 + 15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4] = "\0";
        int wordscount = 0;
        int boolean = 0;
        err = fopen_s(&stream1, this->message, "r");
        if (err == 0) {
            // empty rough message file
            err = fopen_s(&stream2, this->roughmessage, "w");
            char emty[2] = "\0";
            if (err == 0)  fprintf(stream2, emty);
            if (stream2) fclose(stream2);
            int i;
            // open rough message file in append mode
            err = fopen_s(&stream2, this->roughmessage, "a");
            if (err == 0) {
                int whilecount = 0;
                while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                    //cout << buff << endl;
                    char message1[DEFAULT_BUFLEN] = "\0",
                        message2[1 + 15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4] = "\0";
                    char fromname[15] = "\0", toname[15] = "\0";
                    int num;
                    split(buff, toname, fromname, message1, &num);
                    char space = '\0';
                    if (whilecount)
                        space = ' ';
                    if (strcmp(thistoname, toname) == 0) {
                        if (num) {}
                        else {
                            char message3[DEFAULT_BUFLEN + 1 + 15 + 1] = "\0";
                            char message4[DEFAULT_BUFLEN] = "\0";
                            sprintf_s(message3, "%s(%s)\0", message1, fromname);
                            replaceChar(message3, '*', ' ');
                            sprintf_s(message4, "%s\0", message1);
                            replaceChar(message4, '*', ' ');
                            int value = sendd(thisClientSocket, message3, toname);
                            char sent_not_seen[4] = "\0";
                            if (value) {
                                value = client->sendonepersonfromserver(message4, fromname, toname);
                                Sleep(50);
                                if (value) {
                                    sprintf_s(sent_not_seen, "sen\0");
                                }
                                else {
                                    sprintf_s(sent_not_seen, "see\0");
                                }
                                /*
                                if (whilecount)
                                    sprintf_s(message2, " %s_%s_%s_sen", toname, fromname, message1, num);
                                else
                                    sprintf_s(message2, "%s_%s_%s_sen", toname, fromname, message1, num);
                                */
                            }
                            else {
                                sprintf_s(sent_not_seen, "not\0");
                                /*
                                if (whilecount)
                                    sprintf_s(message2, " %s_%s_%s_not", toname, fromname, message1, num);
                                else
                                    sprintf_s(message2, "%s_%s_%s_not", toname, fromname, message1, num);
                                    */
                            }
                            sprintf_s(message2, " %s_%s_%s_%s", toname, fromname, message1, sent_not_seen);
                        }
                    }
                    else if (strcmp(thistoname, fromname) == 0 && num == 2) {
                        char message4[DEFAULT_BUFLEN] = "\0";
                        sprintf_s(message4, "%s\0", message1);
                        replaceChar(message4, '*', ' ');
                        Sleep(50);
                        int value = client->sendonepersonfromserver(message4, fromname, toname);
                        char sent_not_seen[4] = "\0";
                        if (value) {
                            sprintf_s(sent_not_seen, "sen\0");
                        }
                        else {
                            sprintf_s(sent_not_seen, "see\0");
                        }
                        sprintf_s(message2, " %s_%s_%s_%s", toname, fromname, message1, sent_not_seen);
                    }
                    else {
                        sprintf_s(message2, " %s", buff);
                    }
                    whilecount++;
                    //cout << whilecount << ") " << message2 << endl;
                    fprintf(stream2, message2);
                }
            }
            if (stream2) fclose(stream2);
            if (stream1) fclose(stream1);
            boolean = 1;
        }
        else {

        }
        // roughmessage to message
        if (boolean) {
            err = fopen_s(&stream1, this->roughmessage, "r");
            if (err == 0) {
                // empty message file
                err = fopen_s(&stream2, this->message, "w");
                char emty[2] = "\0";
                if (err == 0) fprintf(stream2, emty);
                if (stream2) fclose(stream2);

                err = fopen_s(&stream2, this->message, "a");
                if (err == 0) {
                    int whilecount = 0;
                    while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                        char newbuff[DEFAULT_BUFLEN] = "\0";
                        if (whilecount) {
                            sprintf_s(newbuff, " %s\0", buff);
                        }
                        else {
                            sprintf_s(newbuff, "%s\0", buff);
                        }
                        fprintf(stream2, newbuff);
                        whilecount++;
                    }
                }
            }
        }
        if (stream2) fclose(stream2);
        if (stream1) fclose(stream1);

        Sleep(100);
        this->wait1 = 0;
    }
    int sendMessageUsingName(char thisname[15], char message[DEFAULT_BUFLEN], char fromname[15], int send_not) {
        if (strcmp(thisname, fromname) == 0) {
            return 1;
        }
        while (this->wait) {
            cout << "getname waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        char name[15];
        int i;
        char finalmessage[1 + 15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4];

        Sleep(100);
        this->wait = 0;
        boolean = loginid.Find(thisname);
        if (boolean) {
            boolean = 1;
            int j;
            for (j = 0; j < strlen(message); j++) {
                if (message[j] == ' ') {
                    message[j] = '*';
                }
            }
            message[j] = '\0';
            if (send_not)
                sprintf_s(finalmessage, " %s_%s_%s_sen\0", thisname, fromname, message);
            else
                sprintf_s(finalmessage, " %s_%s_%s_not\0", thisname, fromname, message);
            //cout << finalmessage << endl;
            //cout << endl;
            while (this->wait1) {
                cout << "getname waiting...." << endl;
                Sleep(50);
            }
            this->wait1 = 1;
            err = fopen_s(&stream1, this->message, "a");
            if (err == 0) {
                cout << "stored in file: " << finalmessage << endl;
                fprintf(stream1, finalmessage);
            }
            else {
                cout << "SendMessageUsingName " << this->message << "' not opened\n";
            }
            fclose(stream1);
            
            Sleep(100);
            this->wait1 = 0;
        }
        return boolean;
    }
    int getname(char thisname[15]) {
        while (this->wait) {
            cout << "getname waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        char name[15];
        int i;
        err = fopen_s(&stream1, login, "r");
        if (err == 0) {
            char buff[15 + 1 + 15] = "\0";
            while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                for (i = 0; i < strlen(buff) && i < 15; i++) {
                    if (buff[i] == '_') {
                        break;
                    }
                    name[i] = buff[i];
                }
                name[i] = '\0';
                if (strcmp(name, thisname) == 0) {
                    boolean = 1;
                    //cout << buff;
                    //cout << endl;
                    break;
                }
            }
            //cout << "The file '" << login << "' was opened\n";
        }
        else {
            cout << "GetName " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
        return boolean;
    }
    void deleteid(char name[15]) {
        while (this->wait) {
            cout << "getid waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        //copy from login to rough
        err = fopen_s(&stream1, login, "r");
        if (err == 0) {
            err = fopen_s(&stream2, roughlogin, "w");
            char emty[2] = "\0";
            if (err == 0)  fprintf(stream2, emty);
            if (stream2) fclose(stream2);

            err = fopen_s(&stream2, roughlogin, "a");
            if (err == 0) {
                char buff[1 + 15 + 1 + 15 + 1] = "\0";
                int whilecount = 0;
                while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                    char thisname[15], thispass[15];
                    split(buff, thisname, thispass);
                    if (strcmp(name, thisname) == 0) {
                        cout << "Account Deleted (" << name << ")\n";
                    }
                    else {
                        //save in other rough file
                        char newbuff[1 + 15 + 1 + 15 + 1];
                        if (whilecount)
                            sprintf_s(newbuff, " %s", buff);
                        else
                            sprintf_s(newbuff, "%s", buff);
                        //cout << "buffer:" << newbuff << endl;
                        fprintf(stream2, newbuff);
                        whilecount++;
                    }
                }
            }
            if (stream2) fclose(stream2);
        }
        else {
            cout << "DeleteID " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }

        //paste from rough to login
        err = fopen_s(&stream1, roughlogin, "r");
        if (err == 0) {
            err = fopen_s(&stream2, login, "w");
            char emty[2] = "\0";
            if (err == 0)  fprintf(stream2, emty);
            if (stream2) fclose(stream2);

            err = fopen_s(&stream2, login, "a");
            if (err == 0) {
                char buff[1 + 15 + 1 + 15 + 1] = "\0";
                int whilecount = 0;
                while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                    //save in login file
                    char newbuff[1 + 15 + 1 + 15 + 1];
                    if (whilecount)
                        sprintf_s(newbuff, " %s", buff);
                    else
                        sprintf_s(newbuff, "%s", buff);
                    //cout << "buffer:" << newbuff << endl;
                    fprintf(stream2, newbuff);
                    whilecount++;
                }
            }
            if (stream2) fclose(stream2);
        }
        else {
            cout << "DeleteID " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
        return;
    }
    int getid(char getbuff[15 + 1 + 15]) {
        while (this->wait) {
            cout << "getid waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        int boolean = 0;
        err = fopen_s(&stream1, login, "r");
        if (err == 0) {
            char buff[15 + 1 + 15] = "\0";
            while (fscanf_s(stream1, "%s", buff, _countof(buff)) != EOF) {
                if (strcmp(getbuff, buff) == 0) {
                    boolean = 1;
                    //cout << buff;
                    //cout << endl;
                    break;
                }
            }
            //cout << "The file '" << login << "' was opened\n";
        }
        else {
            cout << "GetID " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
        return boolean;
    }
    void setid(char buff[15 + 1 + 15]) {
        while (this->wait) {
            cout << "setid waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        char newbuff[1 + 15 + 1 + 15];
        sprintf_s(newbuff, " %s\0", buff);
        err = fopen_s(&stream1, login, "a");
        if (err == 0)
        {
            fprintf(stream1, newbuff);
            //cout << "The file '" << login << "' was opened\n";
        }
        else
        {
            cout << "SetID " << login << "' not opened\n";
        }
        if (stream1)
        {
            err = fclose(stream1);
        }
        Sleep(100);
        this->wait = 0;
    }
};