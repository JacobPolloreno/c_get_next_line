/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpollore <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/13 14:59:00 by jpollore          #+#    #+#             */
/*   Updated: 2018/03/16 11:46:27 by jpollore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "get_next_line.h"

char	*expand_buffer(char *buf, size_t expand_by)
{
	char	*dst;
	size_t	size;

	size = buf ? ft_strlen(buf) : 0;
	if ((dst = ft_strnew(size + expand_by)))
	{
		if (buf)
		{
			ft_strcpy(dst, buf);
			ft_strdel(&buf);
		}
	}
	return (dst);
}

int		check_prev(char **prev, char **line)
{
	char	*nl_ptr;
	char	*strnew;
	size_t	prev_size;
	size_t	trim_cnt;

	if ((nl_ptr = ft_strchr(*prev, '\n')))
	{
		trim_cnt = nl_ptr - *prev;
		strnew = NULL;
		free(*line);
		if (!(*line = ft_strsub(*prev, 0, trim_cnt)))
			return (0);
		prev_size = ft_strlen(*prev);
		if (prev_size - trim_cnt > 1 &&
			!(strnew = ft_strsub(*prev, trim_cnt + 1, prev_size - trim_cnt)))
		{
			free(*line);
			return (0);
		}
		free(*prev);
		*prev = strnew;
		return (1);
	}
	return (0);
}

int		read_next_line(const int fd, char **line, char **prev)
{
	int	ret;
	char	*nl_ptr;
	char	*buf;
	size_t	prev_size;
	size_t	nl_idx;

	if (!(buf = ft_strnew(BUFF_SIZE)))
		return (-1);
	while ((ret = read(fd, buf, BUFF_SIZE)) > 0)
	{
		if (!(nl_ptr = ft_strchr(buf, '\n')))
		{
			*prev = expand_buffer(*prev, BUFF_SIZE);
			ft_strcat(*prev, buf);
			ft_bzero(buf, BUFF_SIZE);
		}
		else
		{
			nl_idx = nl_ptr - buf;
			prev_size = *prev ? ft_strlen(*prev) : 0;
			if (prev_size)
				free(*line);
			if (!(*line = ft_strnew(prev_size + nl_idx)))
				return (-1);
			if (*prev)
			{
				ft_strncpy(*line, *prev, prev_size);
				ft_strdel(prev);
			}
			ft_memccpy((void *)&(*line)[prev_size], (void *)buf, '\n', nl_idx);
			if (ret - nl_idx > 1)
				*prev = ft_strsub(buf, nl_idx + 1, ret - nl_idx);
			ft_strdel(&buf);
			return (ret);
		}
	}
	ft_strdel(&buf);
	return (ret);
}

int		fd_insert_cmpf(void *fd1, void *fd2)
{
	t_file	*ptr1;
	t_file	*ptr2;

	ptr1 = (t_file *)fd1;
	ptr2 = (t_file *)fd2;
	return (ptr1->fd - ptr2->fd);
}

int		fd_search_cmpf(void *data_ref, void *node)
{
	t_file	*ptr;
	int	ref;

	ptr = (t_file *)node;
	ref = *(int *)data_ref;
	return (ptr->fd - ref);
}

int		get_next_line(const int fd, char **line)
{
	/* static char	*prev; */
	static	t_btree *root;
	void		*node;
	char		*node_buf;
	int		ret;
	t_file		search;

	if (fd < 0 || !line)
		return (-1);
	search = (t_file) {node_buf, fd};
	if (!(node = btree_search_item(root, (void *)&search, &fd_search_cmpf)))
		btree_insert_data(&root, (void *)&search, &fd_insert_cmpf);
	if (node && check_prev(&((t_file *)node)->prev, line) && line)
		return (1);
	if ((ret = read_next_line(fd, line, &((t_file *)node)->prev)) > 0)
		return (1);
	if (!node && !ret)
		return (0);
	else if (node)
	{
		if (!(*line = ft_strdup(((t_file *)node)->prev)))
		{
			ft_strdel(&((t_file *)node)->prev);
			return (-1);
		}
		ft_strdel(&((t_file *)node)->prev);
		return (1);
	}
	return (-1);
}
