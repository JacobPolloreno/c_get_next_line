#include "libft.h"
#include "get_next_line.h"
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>

void	simple_string(void)
{
	char	*line;
	int		out;
	int		p[2];
	char	*str;
	int		gnl_ret;

	str = (char *)malloc(1000 * 1000);
	*str = '\0';
	strcat(str, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur in leo dignissim, gravida leo id, imperdiet urna. Aliquam magna nunc, maximus quis eleifend et, scelerisque non dolor. Suspendisse augue augue, tempus");
	strcat(str, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur in leo dignissim, gravida leo id, imperdiet urna. Aliquam magna nunc, maximus quis eleifend et, scelerisque non dolor. Suspendisse augue augue, tempus");
	strcat(str, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur in leo dignissim, gravida leo id, imperdiet urna. Aliquam magna nunc, maximus quis eleifend et, scelerisque non dolor. Suspendisse augue augue, tempus");
	out = dup(1);
	pipe(p);
	dup2(p[1], 1);

	write(1, str, strlen(str));
	close(p[1]);
	dup2(out, 1);
	gnl_ret = get_next_line(p[0], &line);
	if (strcmp(line, str) != 0)
		printf("fail\n");
	if (!(gnl_ret == 0 || gnl_ret == 1))
		printf("fail\n");
}

void	test_6(void)
{
	char 		*line;
	int		out;
	int		p[2];
	int		fd;

	out = dup(1);
	pipe(p);

	fd = 1;
	dup2(p[1], fd);
	write(fd, "oiuytrew\n", 9);
	close(p[1]);
	dup2(out, fd);
	get_next_line(p[0], &line);
	assert(strcmp(line, "oiuytrew") == 0);
}

void test_16(void)
{
	char 		*line;
	int		out;
	int		p[2];
	int		fd;

	out = dup(1);
	pipe(p);

	fd = 1;
	dup2(p[1], fd);
	write(fd, "efghijkl", 8);
	close(p[1]);
	dup2(out, fd);
	get_next_line(p[0], &line);
	assert(strcmp(line, "efghijkl") == 0);
}

void	test_40(void)
{
	char 	*line;
	int 	fd;
	int 	fd2;
	int 	fd3;
	int	diff_file_size;

    	system("mkdir -p sandbox");
	/* system("openssl rand -base64 $((30 * 1000 * 3/4)) | tr -d '\n' | tr -d '\r' > sandbox/one_big_fat_line.txt"); */
	system("openssl rand -base64 $((3 * 10 * 3/4)) | tr -d '\n' | tr -d '\r' > sandbox/one_big_fat_line.txt");
	system("echo '\n' >> sandbox/one_big_fat_line.txt");

	fd = open("sandbox/one_big_fat_line.txt", O_RDONLY);
	fd2 = open("sandbox/one_big_fat_line.txt.mine", O_CREAT | O_RDWR | O_TRUNC, 0755);

	while (get_next_line(fd, &line) == 1)
	{
	    write(fd2, line, strlen(line));
	    write(fd2, "\n", 1);
	}
	if (line)
		write(fd2, line, strlen(line));
	close(fd);
	close(fd2);

	system("diff sandbox/one_big_fat_line.txt sandbox/one_big_fat_line.txt.mine > sandbox/one_big_fat_line.diff");
	fd3 = open("sandbox/one_big_fat_line.diff", O_RDONLY);
	diff_file_size = read(fd3, NULL, 10);
	close(fd3);

	assert(diff_file_size == 0);
}

void	test_41(void)
{
	char *line;
	int fd;
	int fd2;
	int fd3;
	int diff_file_size;

	system("mkdir -p sandbox");
	system("openssl rand -out sandbox/large_file.txt -base64 $((50 * 1000 * 3/4)) 2> /dev/null");

	fd = open("sandbox/large_file.txt", O_RDONLY);
	fd2 = open("sandbox/large_file.txt.mine", O_CREAT | O_RDWR | O_TRUNC, 0755);

	while (get_next_line(fd, &line) == 1)
	{
	    write(fd2, line, strlen(line));
	    write(fd2, "\n", 1);
	}

	close(fd);
	close(fd2);

	system("diff sandbox/large_file.txt sandbox/large_file.txt.mine > sandbox/large_file.diff");
	fd3 = open("sandbox/large_file.diff", O_RDONLY);
	diff_file_size = read(fd3, NULL, 10);
	close(fd3);

	assert(diff_file_size == 0);
}

int main(void)
{
	/* simple_string(); */
	/* test_6(); */
	/* test_16(); */
	test_40();
	/* test_41(); */
}
