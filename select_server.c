/* Copyright 2015 Anchit Jain */


#include "./select_server.h"


void sendmessage(char *message, int grno,
                 int groupinfo[][MAX_CLIENTS], int *clients) {
    int i, j;

    if (grno == -1) {
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == -1)
                break;
            ssize_t t = write(clients[i], message, sizeof(char) * MAXLINE);
        }
    } else {
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (groupinfo[grno][i] == -1)
                break;
            write(groupinfo[grno][i], message, sizeof(char)*MAXLINE);
        }
    }
}

void addtogroup(int groupinfo[][MAX_CLIENTS], int grno, int fd) {
    int i;

    for (i = 0; i < MAX_CLIENTS; i++) {
        if (groupinfo[grno][i] == fd) {
            char rep[50] = "Already in group\n";
            write(fd, rep, strlen(rep));
            break;
        }

        if (groupinfo[grno][i] != -1) {
//            if(i==MAX_CLIENTS-1)
//            {
//                char messa4[100]="No more members can be added to group\n";
//                write(fd,messa4,strlen(messa4));
//                break;
//            }
            continue;
        }

        groupinfo[grno][i] = fd;

        char rep[50] = "Added to group\n";
        write(fd, rep, strlen(rep));

        break;
    }
}

void rmfromgroup(int groupinfo[][MAX_CLIENTS], int grno, int fd) {
    int i, check = 0;

    for (i = 0; i < MAX_CLIENTS; i++) {
        if (groupinfo[grno][i] == fd) {
            check = 1;
            groupinfo[grno][i] = -1;
            char rep[50] = "Removed from group\n";
            write(fd, rep, strlen(rep));
            break;
        }
    }
    if(check == 0)
    {
        char rep[50] = "Not present in group\n";
        write(fd, rep, strlen(rep));
    }
}

void messageprocess(char *mess, int *clients,
                    int groupinfo[][MAX_CLIENTS], int *cligroup, int fd) {
    int i, j, len;
    len = strlen(mess);

    char const1[10] = "GROUP$", const2[10] = "GROUPMSG", const3[10] = "GROUPRM$";
    if ((strstr(mess, const1) - mess) == 0) {
        char grno[20];
        memset(grno, '\0', 20);
        int grp;

        if (len < 7) {
            perror("No group number entered\n");
            exit(0);
        }

        for (i = 6; i < len; i++) {
            if (mess[i] == '\r' || mess[i] == '\n' || mess[i] == '\0')
                break;
            grno[i - 6] = mess[i];
        }

        if (strlen(grno) == 0) {
            perror("No group number entered\n");
            exit(0);
        }
        grp = atoi(grno);
        if (grp >= MAX_GROUPS) {
            char rep[100] = "Please enter group number less than 100\n";
            write(fd, rep, strlen(rep));
            return;
        }

        cligroup[fd] = grp;
        addtogroup(groupinfo, grp, fd);

    } else if ((strstr(mess, const3) - mess) == 0) {
        char grno[20];
        memset(grno, '\0', 20);
        int grp;

        if (len < 9) {
            perror("No group number entered\n");
            exit(0);
        }

        for (i = 8; i < len; i++) {
            if (mess[i] == '\r' || mess[i] == '\n' || mess[i] == '\0')
                break;
            grno[i - 8] = mess[i];
        }

        if (strlen(grno) == 0) {
            perror("No group number entered\n");
            exit(0);
        }
        grp = atoi(grno);
        if (grp >= MAX_GROUPS) {
            char rep[100] = "Please enter group number less than 100\n";
            write(fd, rep, strlen(rep));
            return;
        }

        rmfromgroup(groupinfo, grp, fd);

    } else if ((strstr(mess, const2) - mess) == 0) {
        char grno[20], message[MAXLINE];

        memset(grno, '\0', 20);
        memset(message, '\0', MAXLINE);

        int grp;

        for (i = 9; i < len; i++) {
            if (mess[i] == '$')
                break;
            grno[i - 9] = mess[i];
        }

        if (strlen(grno) == 0) {
            perror("No group number entered\n");
            exit(0);
        }
        grp = atoi(grno);

        if (grp >= MAX_GROUPS) {
            char rep[100] = "Please enter group number less than 100\n";
            write(fd, rep, strlen(rep));
            return;
        }

        for (j = i + 1; j < len; j++) {
            message[j - i - 1] = mess[j];
        }

        if (strlen(message) == 0) {
            perror("No message entered\n");
//            exit(0);
        }
        sendmessage(message, grp, groupinfo, NULL);
    } else {
        sendmessage(mess, -1, NULL, clients);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc >= 3) {
        perror("Usage : select_server <port>");
        exit(-1);
    }

    struct sockaddr_in client_address, server_address;
    char buff[MAXLINE];

    int serverfd, clientfd, clilen, sockfd, clients[MAX_CLIENTS],
        groupinfo[MAX_GROUPS][MAX_CLIENTS], cligroup[1024];
    int i, j, maxi, maxfd, fdready;

    ssize_t n;
    fd_set all, read_set;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverfd < 0) {
        perror("Error in creating socket\n");
        exit(-1);
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((uint16_t)atoi(argv[1]));

    int re = bind(serverfd, (struct sockaddr *)&server_address,
                  sizeof(server_address));

    if (re == -1) {
        perror("Error in binding socket\n");
        exit(-1);
    }

    listen(serverfd, LISTENQ);

    maxfd = serverfd;
    maxi = -1;

    for (i = 0; i < MAX_CLIENTS; i++)
        clients[i] = -1;

    for (i = 0; i < MAX_GROUPS; i++)
        for (j = 0; j < MAX_CLIENTS; j++)
            groupinfo[i][j] = -1;

    for (i = 0; i < 1024; i++)
        cligroup[i] = -1;

    FD_ZERO(&all);
    FD_SET(serverfd, &all);

    while (true) {
        read_set = all;
        fdready = select(maxfd + 1, &read_set, NULL, NULL, NULL);

        if (FD_ISSET(serverfd, &read_set)) {
            clilen = sizeof(client_address);
            bzero(&client_address, sizeof(client_address));
            clientfd = accept(serverfd, (struct sockaddr *)&client_address,
                              (socklen_t *)&clilen);

            if (clientfd < 0) {
                perror("Error in connecting to client");
                exit(-1);
            }

            int ipAddr = (&client_address)->sin_addr.s_addr;
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ipAddr, ipstr, INET_ADDRSTRLEN);
            printf("new client: %s, port %d\n",
                   ipstr, ntohs(client_address.sin_port));

            char me[100] = "\nWelcome to server\nUsage:\n";
            write(clientfd, me, strlen(me));
            bzero(me, sizeof(me));
            strcpy(me, "1.To add to the group send GROUP$ID, where ID is group number between 1 & 100\n");
            write(clientfd, me, strlen(me));
            bzero(me, sizeof(me));
            strcpy(me, "2.To send message to the group send GROUPMSG$ID$MSG, where ID is group number between 1 & 100 and $MSG is the message\n");
            write(clientfd, me, strlen(me));
            bzero(me, sizeof(me));
            strcpy(me, "3.To send message all the members just type the message.\n\n");
            write(clientfd, me, strlen(me));

            for (i = 0; i < MAX_CLIENTS; i++)
                if (clients[i] < 0) {
                    clients[i] = clientfd;
                    break;
                }

            if (i == MAX_CLIENTS) {
                char me[100] = "Maximum Clients reached.Closing connection.\n";
                write(clientfd, me, 100);
                close(clientfd);
                printf("Too many clients.Connection closed to latest client.\n");
                continue;
            }

            FD_SET(clientfd, &all);
            if (clientfd > maxfd)
                maxfd = clientfd;
            if (i > maxi)
                maxi = i;

            if (--fdready <= 0)
                continue;
        }

        for (i = 0; i <= maxi; i++) {
            if ((sockfd = clients[i]) < 0)
                continue;

            if (FD_ISSET(sockfd, &read_set)) {
                memset(buff, '\0', MAXLINE);

                if ((n = read(sockfd, buff, MAXLINE)) == 0) {
                    /*connection closed by client */
                    close(sockfd);
                    FD_CLR(sockfd, &all);
                    clients[i] = -1;
                } else {
                    messageprocess(buff, clients, groupinfo, cligroup, sockfd);
                }

                if (--fdready <= 0)
                    break;      /* no more readable descriptors */
            }
        }
    }

    return 0;
}

