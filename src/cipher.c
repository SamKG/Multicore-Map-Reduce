

int main(int argc, char **argv) {
	int key;
	if(argc != 4){
		return 0;
	}
	if(strcmp(argv[1], "key") == 0) {
		key = atoi(argv[2]);
		if(key < 0 || key > 25){
			key = (abs(key) % 26);
		}
	}
	FILE *ft;
	int ch;
	ft = fopen(argv[3], "r+");
	if (ft == NULL)
	{
	   fprintf(stderr, "cannot open target file %s\n", name);
	   exit(1);
	}
	while ((ch = fgetc(ft)) != EOF)
	{
	   if (ch >= 'a' && <= 'z')
	   {
	       fseek(ft, -1, SEEK_CUR);
	       fputc( 'a' + ( (ch - 'a' + key) % 26 )), ft);
	       fseek(ft, 0, SEEK_CUR);
	   }
	   if (ch >= 'A' && <= 'Z')
	   {
	       fseek(ft, -1, SEEK_CUR);
	       fputc( 'A' + ( (ch - 'A' + key) % 26 ), ft);
	       fseek(ft, 0, SEEK_CUR);
	   }
	}
	fclose(ft);
	return 0;

}
