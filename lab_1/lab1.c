#include <stdio.h>

int main(int argc, char* argv[])
{
	if(argc !=3)
	{
		return -1;
	}
	FILE *src;
	FILE *dst;
	char buffer[10];

	src = fopen(argv[1], "rb");
	dst = fopen(argv[2], "wb");
	
	while (!feof(src))
	{
		int size = fread(buffer, sizeof(char), 10, src);
		fwrite(buffer, sizeof(char), size, dst);
	}

	fclose(src);
	fclose(dst);
	return 1;

}
