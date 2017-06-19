#include<iostream>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <windows.h>
#include <fstream>

using namespace std;

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

int nGlobalCount = 0;
int thread_index = 0;
int *num_of_thr= new int;

int **a ;	//array A
int **b ;	//array B
int **c ;	//array result

int position;
int i , j , k;

struct v {
  int i; /*row*/
  int j; /*column*/
};

/*
	taken from url :http://stackoverflow.com/questions/6066816/matrix-multiplication-using-win32-threads
*/

DWORD ThreadProc (LPVOID lpdwThreadParam ) {
   struct v *input = (struct v *)lpdwThreadParam ;
   int avg=4*4/9;
   for(int n=0; n<avg; n++) {
      int sum=0;
      for ( k = 0 ; k < position; k++) {
        sum=sum+((a[input->i][k])*(b[k][input->j]));
        c[input->i][input->j]=sum;
        if(j<3 && avg!=1)
            j=j+1;
        else if (j==3 && avg!=1 && (avg-n)!=1)
            i=i+1;

        }
    }

cout<<"the number of the thread "<<thread_index<<endl;
return 0;

}

///////////	The main	////////////

int main() {
	system("COLOR FC");
	cout << "\n \t welecome in my program (assignment 2 ) \n \n";

	int arow , acolumn , brow , bcolumn ; 
	//	array A
	cout<<"enter number of row for matrix A : \n ";
	cin>>arow;
	cout<<"enter number of column for matrix A : \n ";
	cin>>acolumn;
	a = new int*[arow];
	for(int i=0 ; i<arow ;i++ )
		a[i] = new int[acolumn];
	//	array B
	cout<<"enter number of row for matrix B : \n ";
	cin>>brow;
	cout<<"enter number of column for matrix B : \n ";
	cin>>bcolumn;
	b = new int*[brow];
	for(int i=0 ; i<brow ;i++ )
		b[i] = new int[bcolumn];
	//	array C
	c = new int*[arow];
	for(int i=0 ; i<arow ;i++ )
		c[i] = new int[bcolumn];
	for(int i=0 ;i<arow ;i++)
		for(int j=0 ;j<bcolumn ;j++)
			c[i][j]=0;
	//////////////////////////////////////////	Read Data from files	/////////////////////////////
	cout<<"enter name of file for matrix A : \n ";
	wstring afile;
	wcin>>afile;
	//
	cout<<"enter name of file for matrix B : \n ";
	wstring bfile;
	wcin>>bfile;
	///////////////////////////////////////////

	//////////	read from file matrix A

	HANDLE hIn, hOut;	//used in kernel mode
	DWORD nIn, nOut;	//unsigned integer 4 byte------------->1)number of byte read 2)number of bute writed
	char Buffer[2];	//array of size 256 of char{1 byte}

	hIn = CreateFile	//function return handle of file is created
		(afile.data(),		//name of file to read
		GENERIC_READ,             // open for reading
		0,                        // do not share
		NULL,                     // no security
		OPEN_EXISTING,            // existing file only
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template
	 
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Input file error:%x\n",
			GetLastError());

	}

	for(int i=0 ;i<arow ;i++)
		for(int j=0 ; j<=acolumn ;j++)
		{
			ReadFile(hIn/*handle of file to read*/, Buffer/*array of char to stored data*/, 2/*size of array*/, &nIn/*number of byte read*/, NULL);
			a[i][j] = atoi( Buffer );
		}
	CloseHandle(hIn);	//it important to closs all handles becuase are to reuse again from another function or progra
	
	//////////////	read from file matrix B

	hIn = CreateFile	//function return handle of file is created
		(bfile.data(),		//name of file to read
		GENERIC_READ,             // open for reading
		0,                        // do not share
		NULL,                     // no security
		OPEN_EXISTING,            // existing file only
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template
	 
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Input file error:%x\n",
			GetLastError());

	}

	for(int i=0 ;i<brow ;i++)
		for(int j=0 ; j<=bcolumn ;j++)
		{
			ReadFile(hIn/*handle of file to read*/, Buffer/*array of char to stored data*/, 2/*size of array*/, &nIn/*number of byte read*/, NULL);
			b[i][j] = atoi( Buffer );
		}
	CloseHandle(hIn);	//it important to closs all handles becuase are to reuse again from another function or progra
	

int *numThreads = new int;
*numThreads =arow * bcolumn;
int n = *numThreads;

DWORD *ThreadIds = new DWORD[n];
HANDLE *ThreadHandles = new HANDLE[n];
struct v* data = new struct v[n];

position = arow;	//Global

for ( int i=0 ; i < arow; i++) {
    for(int j=0 ; j <arow; j++) {
        data[thread_index].i = i;
        data[thread_index].j = j;


        ThreadHandles[thread_index] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadProc, &data[thread_index], 0,&ThreadIds[thread_index]);

        thread_index++;


        }
    }

*num_of_thr = n;

WaitForMultipleObjects(*num_of_thr, ThreadHandles, TRUE, INFINITE);

cout << string(2, '\n');
system("COLOR 0A");

cout<<"			The resultant matrix is writen in file ( c.txt ) "<<endl;

ofstream cFile("c.txt");

for ( i = 0 ; i < arow; i++) {
    for ( j = 0 ; j < bcolumn ; j++)
		cFile<<c[i][j]<<" ";
	cFile<<endl;
    }
cFile.close();
cout << string(2, '\n');

for (int i=0; i<n; i++) CloseHandle(ThreadHandles[i]);
return 0;
}

