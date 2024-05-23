/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryaloo <bryaloo@student.42kl.edu.my>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 16:41:04 by bryaloo           #+#    #+#             */
/*   Updated: 2024/04/24 18:54:32 by bryaloo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>

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
		*newline_pos = '\0';
		line = ft_strjoin(*remain, "\n");
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
	line = ft_strdup(*remain);
	free(*remain);
	*remain = NULL;
	return (line);
}

/*
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
	line = ft_strdup(*remain);
	free(*remain);
	*remain = NULL;
	return (line);
}
*/

char	*get_next_line(int fd)
{
	char		*buffer;
	static char	*remain;
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
		line = ft_strdup(remain);
		free(remain);
		remain = NULL;
	}
	free(buffer);
	return (line);
}

#include <stdlib.h>

int	main(int argc, char** argv)
{
	int		fd;
	char	*line;
	int		i;

	i = 0;
	if (argc != 2)
		return (1);
	fd = open(argv[argc - 1], O_RDONLY);
	while (1)
	{
		line = get_next_line(fd);
		if (line == NULL)
			break ;
		printf("Call number %d = %s", i++, line);
		free (line);
	}
	return (0);
}
//COMPILE: gcc get_next_line.c get_next_line_utils.c
//RUN: ./a.out test2.txt
//B2BR https://github.com/hanshazairi/42-born2beroot
// https://github.com/pasqualerossi/Born2BeRoot-Guide