#include <mpi.h>
#include <stdio.h>

int main(int argc,char** argv){
    MPI_Init(NULL,NULL);
    int myRank,mySize,partition_size;
    MPI_Comm_size(MPI_COMM_WORLD,&mySize);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    if(myRank==0){
        printf("Hello from master process\n");
        printf("number of slave processes is %d\n",(mySize-1));
        printf("please enter the size of an array:\n");
        int n;
        scanf("%d",&n);
        int array[n];
        printf("please enter the elements of the array\n");
        for(int i=0;i<n;i++){
            scanf("%d",&array[i]);
        }

        int base_count=n/(mySize-1);
        int remainder=n%(mySize-1);
        

        int start=0;
        for(int i=1;i<mySize;i++){
            int count=base_count;
            if(i<=remainder){
                count+=1;
            }
            MPI_Send(&count,1,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Send(&array[start],count,MPI_INT,i,0,MPI_COMM_WORLD);
            MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
            start+=count;
        }

        int maxValue=-1e9;
        int maxIndex=-1;
        for(int i=1;i<mySize;i++){
            int localMax,localIndex;
            MPI_Recv(&localMax,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Recv(&localIndex,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if(localMax>maxValue){
                maxValue=localMax;
                maxIndex=localIndex;
            }
        }
        printf("Master process announce the final max which is %d and it's index is %d.",maxValue,maxIndex);

    }
    else{
        int count;
        MPI_Recv(&count,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        int subarray[count];
        MPI_Recv(subarray,count,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        int start_index;
        MPI_Recv(&start_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int mx=subarray[0],index=0;
        for(int i=0;i<count;i++){
            if(subarray[i]>mx){
                mx=subarray[i];
                index=i;
            }
        }
        int global_index=start_index+index;
        printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n",myRank,mx,index);
        MPI_Send(&mx,1,MPI_INT,0,0,MPI_COMM_WORLD);
        MPI_Send(&global_index,1,MPI_INT,0,0,MPI_COMM_WORLD);
        
    }


    MPI_Finalize();
    return 0;
}
