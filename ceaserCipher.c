#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MASTER 0
char alph[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int main(int argc , char** argv){
    char ch[50],type[50];
    char s[100];

    MPI_Init(&argc , &argv);

    int rank , size;
    int n;//length of string
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    

    if(rank == MASTER) {
        printf("console or file \n");
        scanf("%s", type);

        printf("encode or decode: \n");
        scanf("%s", ch);

        if (strcmp(type, "console") == 0) {
            printf("please enter a string: \n");
            scanf("%s", s);
        } else {
            FILE *fp = fopen("test.txt", "r");
            if (fp == NULL) {
                printf("Cannot open test.txt\n");
                return 1;
            }
            fscanf(fp, "%s", s);
            fclose(fp);
        }
        n = strlen(s);

        int i;
        for (i = 1; i < size; i++) {
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(s, strlen(s) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            MPI_Send(ch, strlen(ch) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    }
    else{
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(s, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(ch, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    }
    int part = n / size;
    int remain = n % size; 

    int start = rank * part + (rank < remain ? rank : remain);
    int end = start + part + (rank < remain ? 1 : 0);

    //process
    int i,j;
    for(i = start;i<end ;i++){
        for(j = 0;j<52;j++){
            if (isupper(s[i])) {
                if (strcmp(ch, "encode") == 0)
                    s[i] = 'A' + (s[i] - 'A' + 3) % 26;
                else if (strcmp(ch, "decode") == 0)
                    s[i] = 'A' + (s[i] - 'A' - 3 + 26) % 26;
            }
            else if (islower(s[i])) {
                if (strcmp(ch, "encode") == 0)
                    s[i] = 'a' + (s[i] - 'a' + 3) % 26;
                else if (strcmp(ch, "decode") == 0)
                    s[i] = 'a' + (s[i] - 'a' - 3 + 26) % 26;
            }
            break;
        }
    }

    if(rank == MASTER){
        //MASTER gathers the encoded/decoded parts
        for (int i = 1; i < size; i++) {
            int istart = i * part + (i < remain ? i : remain);
            int iend = istart + part + (i < remain ? 1 : 0);
            int len = iend - istart;
            MPI_Recv(s + istart, len, MPI_CHAR, i, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        printf("output string: %s\n", s);
    }
    else{
        //each process send its part
        int len = end - start;
        MPI_Send(s+start, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}