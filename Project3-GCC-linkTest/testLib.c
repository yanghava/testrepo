extern int printf (const char *__restrict __format, ...);

int libFuncPrint() {
	printf("This is printed by libFuncPrint\n");
	return 0;
}
