#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;

struct PCB
{
	struct PCB *next;
	int pid;
	int allocation[3];// 현재 갖고있는 리소스들
	int max[3];// 필요 리소스의 최댓값
	int need[3];// need of 리소스
}pcb[5], pcb2[5], *tail = NULL, *head = NULL;

int make_sequence(int);
void output(int);

int main(int argc,char *argv[])
{
	int value;

	head = (struct PCB*)malloc(sizeof(struct PCB));
	head->next = NULL;

	value = make_sequence(0);// -1이면 unsafe니까 추가요청도 어차피 unsafe 될거니까

	if (value == 0)//safe면
		output(value);
	else if (value == -1)//unsafe면
		output(value);
}

int make_sequence(int rq)
{
	struct PCB *node = NULL;
	int length, i, k, j, num;
	int cpcb = -1, expcb = -1, count = 0, index, symbol = 0, rqindex, endp[5]={9,9,9,9,9}, sign = 0;
	int available[3], request[3];
	char str[15];

	fp = fopen("scenario4.txt", "r");
	if (fp == NULL)
	{
		printf("file read error!");
		exit(1);
	}

	while (fscanf(fp, "%s", str) != EOF)
	{
		length = strlen(str);

		if (length == 3)// 읽은게 available 일때
		{
			for (i = 0; i <= 2; i++)
				available[i] = str[i] - '0';
		}
		else if (length == 4)// 읽은게 추가적인 request 일때
		{// 우선 순서 정하기 전에 요청 값을 더한다음 실행 --> 프로세스입장에선 중간과정
			if (rq == 1)//단순히 현재 상태만 고려하고 그 상태가 safe였다면 rq가 1이되고 요청을
			{           //받아들이고 나면 어떻게될지를 여기서 정해줌
				for (k = 0; k <= cpcb; k++)
				{
					for (i = 0; i <= 2; i++)
					{
						if (pcb2[k].pid == str[0] - '0')
						{
							pcb2[k].max[i] += str[i + 1] - '0';
							pcb2[k].need[i] = pcb2[k].max[i] - pcb2[k].allocation[i];
						}
					}
				}
			}
		}
		else// pid  allocation  max 일때 /// 0  234  678
		{
			cpcb++;
			if (rq == 1)
			{
				for (i = 0; i <= 2; i++)
				{
					pcb2[cpcb].pid = str[0] - '0';
					pcb2[cpcb].allocation[i] = str[i + 1] - '0';
					pcb2[cpcb].max[i] = str[i + 4] - '0';
					pcb2[cpcb].need[i] = pcb2[cpcb].max[i] - pcb2[cpcb].allocation[i];
				}
			}
			else
			{
				for (i = 0; i <= 2; i++)
				{
					pcb[cpcb].pid = str[0] - '0';
					pcb[cpcb].allocation[i] = str[i + 1] - '0';
					pcb[cpcb].max[i] = str[i + 4] - '0';
					pcb[cpcb].need[i] = pcb[cpcb].max[i] - pcb[cpcb].allocation[i];
				}
			}
		}
	}//while 안에서 모든 정보를 저장했음
	while (cpcb != expcb || symbol <= 15)
	{
		for (k = 0; k <= cpcb; k++)
		{
			for (i = 0; i <= 2; i++)//need와 available 하나씩 비교
			{
				for (j = 0; j <= 4; j++)
				{
					if (endp[j] == k)
					{
						sign = 1;
						break;
					}
				}
				if (sign == 1)
					break;
				if (rq == 1)
				{
					if (available[i] >= pcb2[k].need[i])
					{
						count++;
						index = k;
					}
					else
					{
						count = 0;
						break;
					}
				}
				else
				{
					if (available[i] >= pcb[k].need[i])
					{
						count++;
						index = k;
					}
					else
					{
						count = 0;
						break;
					}
				}
			}
			sign = 0;

			if (count == 3)// need <= avilable 만족했으면
			{
				expcb++;
				endp[expcb] = index;
				node = (struct PCB*)malloc(sizeof(struct PCB));
				if (node == NULL)
				{
					printf("malloc failed!");
					exit(1);
				}
				node->next = NULL;
				if (rq == 1)
					node = &pcb2[index];
				else
					node = &pcb[index];

				if (head->next == NULL)//
				{
					head->next = node;
					tail = node;
				}
				else
				{
					tail->next = node;
					tail = node;
				}
				for (i = 0; i <= 2; i++)//순서 정해졌으면 리소스 available로 반납
				{
					available[i] += node->max[i];
				}
				count = 0;
				if (cpcb == expcb)
					break;
			}
		}
		symbol++;
		if (symbol > 15)
		{
			fclose(fp);
			return -1;// --> unsafe
		}
		else if (cpcb == expcb)
		{
			fclose(fp);
			return 0;// --> safe
		}
	}
}

void output(int n)
{
	struct PCB *temp = head->next;
	int value, i, k;

	if (n == -1)
	{
		printf("scenario.dat\ncurrent state: unsafe!\n\n");
	}
	else if (n == 0)
	{
		printf("scenario.dat\ncurrent state: ");
		while (temp != NULL)
		{
			if (temp->next == NULL)
			{
				printf("P%d\n", temp->pid);
				temp = temp->next;
			}
			else
			{
				printf("P%d -> ", temp->pid);
				temp = temp->next;
			}
		}
		head->next = NULL;
		value = make_sequence(1);
		if (value == -1)
			printf("After arriving request: unsafe!\n\n");
		else if (value == 0)
		{
			temp = head->next;
			printf("After arriving request: ");
			while (temp != NULL)
			{
				if (temp->next == NULL)
				{
					printf("P%d\n\n", temp->pid);
					temp = temp->next;
				}
				else
				{
					printf("P%d -> ", temp->pid);
					temp = temp->next;
				}
			}
		}
	}
}
