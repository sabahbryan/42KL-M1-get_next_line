/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryaloo <bryaloo@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:41:18 by bryaloo           #+#    #+#             */
/*   Updated: 2024/05/25 17:41:35 by bryaloo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/**
 * @brief reads data from FD and concats with remainder data
 * @param fd file descriptor, read (O_RDONLY)
 * @param remain holds remainder not returned as completed line 
 * @param buffer temporarily store data read from FD
 * @var bytes_read indicates bytes read from FD to buffer,
 * @var temp holds combined remain and buffer before remain is updated
 * @return number of bytes read, returns zero when EOF
 */
static int	read_and_concat(int fd, char **remain, char *buffer)
{
	int		bytes_read;
	char	*temp;

	bytes_read = read(fd, buffer, BUFFER_SIZE);
	if (bytes_read <= 0)
		return (bytes_read);
	buffer[bytes_read] = '\0';
	if (*remain == NULL)
		*remain = ft_strdup("");
	temp = ft_strjoin(*remain, buffer);
	free(*remain);
	if (!temp)
		return (-1);
	*remain = temp;
	return (bytes_read);
}

/**
 * @brief extracts line from data in remainder
 * @param remain holds remainder not returned as completed line
 * @var line holds completed line to return
 * @var newline_pos position to find newline and split remainder
 * @var temp stores new value of remainder after extraction
 * @return next line as new string or NULL if no newline is found
 */
static char	*extract_line(char **remain)
{
	char	*line;
	char	*newline_pos;
	char	*temp;

	if (*remain == NULL)
		return (NULL);
	newline_pos = ft_strchr(*remain, '\n');
	if (newline_pos)
	{
		line = ft_substr(*remain, 0, newline_pos - *remain + 1);
		temp = ft_strdup(newline_pos + 1);
		free(*remain);
		*remain = temp;
		if (!(*remain))
		{
			free(line);
			return (NULL);
		}
		return (line);
	}
	else
		return (NULL);
}

/**
 * @brief handles remaining data after EOF
 * @param remain holds remainder not returned as completed line
 * @param line holds read data 
 * @return remainder as new string
 */
static char	*handle_remain(char **remain, char *line)
{
	if (line)
		free(line);
	line = ft_strdup(*remain);
	free(*remain);
	*remain = NULL;
	return (line);
}

/**
 * @brief reads from a file and return upto the newline char
 * @param fd file descriptor, read (O_RDONLY)
 * @var buffer temporary storage to read from file
 * @var remain stores remainder from previous call
 * @var line stores extracted line to be returned to caller
 * @var bytes_read determines if more data to be read or EOF reached
 * @return the string or NULL when failed
 */
char	*get_next_line(int fd)
{
	char		*buffer;
	static char	*remain = NULL;
	char		*line;
	int			bytes_read;

	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	line = NULL;
	bytes_read = 1;
	if (!remain || !*remain)
		bytes_read = read_and_concat(fd, &remain, buffer);
	while (bytes_read > 0)
	{
		line = extract_line(&remain);
		if (line)
			break ;
		bytes_read = read_and_concat(fd, &remain, buffer);
	}
	if (bytes_read == 0 && remain && *remain)
		line = handle_remain(&remain, line);
	else if (bytes_read == 0 && remain && (*remain == 0))
		free (remain);
	free(buffer);
	return (line);
}

// #include <fcntl.h>
// int	main(void)
// {
// 	int fd = open("test", O_RDONLY);
// 	char *ptr = get_next_line(fd);
// 	free(ptr);
// 	return (0);
// }

/* GNL with 29 LINES
char	*get_next_line(int fd)
{
	char		*buffer;
	static char	*remain = NULL;
	char		*line;
	int			bytes_read;

	bytes_read = 1;
	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	line = NULL;
	while (bytes_read > 0)
	{
		line = extract_line(&remain);
		if (line)
			break ;
		bytes_read = read_and_concat(fd, &remain, buffer);
	}
	if (bytes_read == 0 && remain && *remain)
	{
		if (line)
			free(line);
		line = ft_strdup(remain);
		free(remain);
		remain = NULL;
	}
	else if (bytes_read == 0 && remain && (*remain == 0))
		free (remain);
	free(buffer);
	return (line);
}
*/

/*
int	main(int argc, char** argv)
{
 	int		fd;
 	char	*line;
 	int		i;

 	i = 0;
 	if (argc != 2)
 	{
 		printf("Usage: %s <filename>\n", argv[0]);
 		return (1);
 	}
 	fd = open(argv[argc - 1], O_RDONLY);
 	if (fd < 0)
 	{
 		perror("Error opening file");
 		return (1);
 	}
 	line = get_next_line(fd);
 	while (line)
 	{
 		printf("Call number %d = %s", i++, line);
 		free (line);
 		line = get_next_line(fd);
 	}
 	close(fd);
 	system("leaks a.out");
 	return (0);
}
*/

//COMPILE: gcc get_next_line.c get_next_line_utils.c
//RUN: ./a.out test3.txt
// -fsanitize=address