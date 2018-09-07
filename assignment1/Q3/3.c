#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void chkPrime(int n)
{
	int i, flag = 0;
	for(i=2; i<=n/2; ++i)
	{
		if(n%i==0)
		{
			flag=1;
			break;
		}
	}

	if (flag==0)printf("Prime\n");
	else printf("Not prime\n");
}

int isPrime(int n)
{
	int i, flag = 0;
	for(i=2; i<=n/2; ++i)
	{
		if(n%i==0)
		{
			flag=1;
			break;
		}
	}

	if (flag==0)return 1;
	else return 0;
}

int main(int argc, char *argv[])
{
	
	srand(time(NULL));
	int fd[2];int n;
	printf("Enter n followed by n integers\n");
	scanf("%d",&n);
	int val = 0,xx,yy,s=0,k=0;
	int a[n];
	for(int i=0;i<n;i++)
	{
		scanf("%d",&a[i]);
	}
	int v[n];
	for (int i = 0; i < n; ++i)
	{
		v[i]=0;
	}
	
	// int x[10000];int xc=0;
	// int y[10000];int yc=0;

	// int flag=1;

	// while(flag)
	// {
	// 	xx = rand() % n;
	// 	yy = rand() % n;
	// 	while(xx == yy)
	// 		yy = rand() % n;
	// 	x[xc++]=a[xx];
	// 	y[yc++]=a[yy];
	// 	if(v[xx]==0)
	// }

	pipe(fd);
	int pid=fork();
	int c=0;
	if (pid != 0)
	{
		// parent: writing only, so close read-descriptor.
		close(fd[0]);

		BEG:
		// send the value on the write-descriptor.
		
		
		xx = rand() % n;
		yy = rand() % n;
		while(xx == yy)
			yy = rand() % n;
		
		val = yy;
		
		write(fd[1], &val, sizeof(val));
		if(v[yy] == 0)
		{
			v[yy] = 1;
			c++;
		}
		
		printf("X=%d Y=%d\n",a[xx],a[yy]);
		//printf("Parent(%d) sending value: %d\n", getpid(), a[val]);

		// for (int i = 0; i < n; ++i)
		// {
		// 	printf("%d ",v[i] );
		// }
		// printf("\n");


		//printf("count %d\n",c );
		//printf("Parent sleeping for %ds\n", a[xx]/3);
		if(a[xx]/3 > 1)
		sleep(a[xx]/3);
		else sleep(2);
		if(c<n)
			goto BEG;
		close(fd[1]);
		exit(0);
		// close the write descriptor
		
	}
	else
	{   
		close(fd[1]);

		GEB:
		

		read(fd[0], &val, sizeof(val));
		if(v[val] == 0)
		{
			v[val] = 1;
			c++;
		}
		//printf("Child(%d) received value: %d\n", getpid(), a[val]);
		chkPrime(a[val]);
		if(isPrime(a[val]))
		{
			s+=a[val];
		}
		printf("Sum = %d\n\n",s);
		//printf("count %d\n\n",c );
		//printf("Child sleeping for %ds\n\n", a[val]%3);
		if(a[val]%3 > 1)
		sleep(a[val]%3);
		else sleep(2);
		if(c<n)
			goto GEB;

		close(fd[0]);
		exit(0);
	}
	return 0;
}
