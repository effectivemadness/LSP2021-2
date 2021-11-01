#include <stdio.h>
#include <unistd.h>
#include "record.h"
#include <stdlib.h>
#include <string.h>
void
print_record(Record *rp)
{
	puts(rp->stud);
	puts(rp->num);
	puts(rp->dept);
}

int main(int argc, char *argv[])
{
	FILE	*fp;
	Record	rec;

	if (argc != 2)  {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		exit(1);
	}

	if ((fp = fopen(argv[1], "r+b")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	printf("-----Ordered Record List-----\n");

	while (fread(&rec, sizeof(rec), 1, fp))  {
		//rec에 fp에서 rec 타입의 크기만큼 1번 읽어서 저장.
		print_record(&rec);
	}
	rewind(fp);
	getchar();

	printf("-----Shuffled Record List (3, 6, 2, 4, 1, 5)-----\n");

	fseek(fp, sizeof(rec)*2L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*1L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	/* fseek */
	fseek(fp, sizeof(rec)*3L, SEEK_SET); // 파일의 처음 부분에서, sizeof(rec)*3만큼 뒤로 이동 = 4번째 레코드 자리부터 읽기.
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*0L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*4L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);
	getchar();

	printf("-----Updated Record List-----\n");

	fseek(fp, sizeof(rec)*2L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	strcpy(rec.dept, "Movie");
	/* fseek */
	fseek(fp, sizeof(rec)*2L, SEEK_SET); // sizeof(rec)*2L로 읽어서 갱신하려고 하므로, 다시 원래 그 자리(sizeof(rec)*2)로 이동
	fwrite(&rec, sizeof(rec), 1, fp);

	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	strcpy(rec.dept, "Baseball");
	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fwrite(&rec, sizeof(rec), 1, fp);

	rewind(fp);
	while (fread(&rec, sizeof(rec), 1, fp))  {
		print_record(&rec);
	}

	fclose(fp);
}
