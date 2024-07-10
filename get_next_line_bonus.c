/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryaloo <bryaloo@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 17:26:13 by bryaloo           #+#    #+#             */
/*   Updated: 2024/06/25 17:26:37 by bryaloo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#define MAX_FD 512

/**
 * @brief reads data from FD and concats with remainder data
 * @param fd file descriptor, read (O_RDONLY)
 * @param remain holds remainder not returned as completed line 
 * @param buffer temporarily store data read from FD
 * @var bytes_read stores read results
 * @var temp holds combined remain and buffer before remain is updated
 * @return number of bytes read
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
 * @brief reads from a file and return up to the newline char
 * @param fd file descriptor, read (O_RDONLY)
 * @param remain stores remaining read buffer
 * @param buffer to read data from FD
 * @var line initialise NULL, stores line to be extracted and returned
 * @var bytes_read tracks number of bytes to be read from FD
 * @return the string or NULL when failed
 */
static char	*read_line(int fd, char **remain, char *buffer)
{
	char	*line;
	int		bytes_read;

	line = NULL;
	bytes_read = 1;
	if (!remain[fd] || !*remain[fd])
		bytes_read = read_and_concat(fd, &remain[fd], buffer);
	while (bytes_read > 0)
	{
		line = extract_line(&remain[fd]);
		if (line)
			break ;
		bytes_read = read_and_concat(fd, &remain[fd], buffer);
	}
	if (bytes_read == 0 && remain[fd] && *remain[fd])
		line = handle_remain(&remain[fd], line);
	else if (bytes_read == 0 && remain[fd] && !*remain[fd])
		free(remain[fd]);
	return (line);
}

/**
 * @brief reads from a file and return upto the newline char
 * @param fd file descriptor, read (O_RDONLY)
 * @var remain array of char pointers, each index represents a FD
 * @var buffer temporary storage to read from file
 * @var line stores extracted line to be returned to caller
 * @return the string or NULL when failed
 */
char	*get_next_line(int fd)
{
	static char	*remain[MAX_FD];
	char		*buffer;
	char		*line;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	line = read_line(fd, remain, buffer);
	free(buffer);
	return (line);
}

/*
//MAIN FUNCTION\\
#include <fcntl.h>
int	main(int argc, char **argv)
{
	int		fds[argc - 1];
	char	*line;
	int		i;
	int		active_fds;

	if (argc < 2)
	{
		printf("Usage: %s <filename1> <filename2> ... <filenameN>\n", argv[0]);
		return (1);
	}
	// Open all files and store their file descriptors
	for (i = 1; i < argc; i++)
	{
		fds[i - 1] = open(argv[i], O_RDONLY);
		if (fds[i - 1] < 0)
		{
			perror("Error opening file");
			while (--i > 0)
				close(fds[i - 1]);
			return (1);
		}
	}
	active_fds = argc - 1;
	// Read lines from all files until all are exhausted
	while (active_fds > 0)
	{
		for (i = 0; i < argc - 1; i++)
		{
			if (fds[i] >= 0)
			{
				line = get_next_line(fds[i]);
				if (line)
				{
					printf("File %d: %s", i + 1, line);
					free(line);
				}
				else
				{
					close(fds[i]);
					fds[i] = -1;
					active_fds--;
				}
			}
		}
	}
	return (0);
}

#include <fcntl.h>
int	main(void)
{
	int fd = open("test", O_RDONLY);
	char *ptr = get_next_line(fd);
	free(ptr);
	return (0);
}

//OLD\\
static int	initialise_buffer(char **buffer)
{
	*buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!(*buffer))
		return (0);
	return (1);
}

char	*get_next_line(int fd)
{
	static char	*remain[MAX_FD];
	char		*buffer;
	char		*line;
	int			bytes_read;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	if (!initialise_buffer(&buffer))
		return (NULL);
	line = NULL;
	bytes_read = 1;
	if (!remain[fd] || !*remain[fd])
		bytes_read = read_and_concat(fd, &remain[fd], buffer);
	while (bytes_read > 0)
	{
		line = extract_line(&remain[fd]);
		if (line)
			break ;
		bytes_read = read_and_concat(fd, &remain[fd], buffer);
	}
	if (bytes_read == 0 && remain[fd] && *remain[fd])
		line = handle_remain(&remain[fd], line);
	else if (bytes_read == 0 && remain[fd] && !*remain[fd])
		free(remain[fd]);
	free(buffer);
	return (line);
}*/