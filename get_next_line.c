/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpollore <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/13 14:59:00 by jpollore          #+#    #+#             */
/*   Updated: 2018/03/20 21:39:07 by jpollore         ###   ########.fr       */
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
			*prev = expand_buffer(*prev, BUFF_SIZE + 1);
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
	return (ref - ptr->fd);
}

t_file		*create_file(int fd)
{
	t_file *node;

	if ((node = (t_file *)ft_memalloc(sizeof(*node))))
	{
		node->fd = fd;
		node->prev = 0;
	}
	return (node);
}

void		delete_file(t_file **file)
{
	if (!file)
		return ;
	free(*file);
	*file = NULL;
}

int		btree_cmpf_file_fd(void *d1, void *d2)
{
	int fd1;
	int fd2;

	fd1 = ((t_file *)d1)->fd;
	fd2 = ((t_file *)d2)->fd;
	return (fd1 - fd2);
}

void		btree_deletef_file(t_btree **node)
{
	if (!node)
		return ;
	delete_file((*node)->item);
	free(*node);
	*node = NULL;
}

void		*btree_updatef_file(void **data, void *new_data)
{
	t_file *new_file;
	t_file *update_file;

	delete_file((t_file **)data);
	update_file = (t_file *)(new_data);
	new_file = create_file(update_file->fd);
	if (update_file->prev)
		new_file->prev = ft_strdup(update_file->prev);
	return (new_file);
}

int		get_next_line(const int fd, char **line)
{
	static t_btree		*root;
	void			*node;
	int			ret;
	t_file			*t_file_ptr;
	char			eof_chck[2];

	if (fd < 0 || !line || read(fd, NULL, 0) == -1)
		return (-1);
	if (!(node = btree_search_item(root, (void *)&fd, &fd_search_cmpf)))
	{
		if (!(t_file_ptr = create_file(fd)))
			return (-1);
		btree_insert_node(&root, (void *)t_file_ptr, &fd_insert_cmpf);
		node = btree_search_item(root, (void *)&fd, &fd_search_cmpf);
	}
	if (((t_file *)node)->prev &&
		check_prev(&((t_file *)node)->prev, line) && line)
	{
		if (!((t_file *)node)->prev && (ret = read(fd, eof_chck, 1)) != 1)
			root = btree_remove_node(&root, (void *)&fd, &fd_search_cmpf,
				&btree_deletef_file, &btree_updatef_file);
		else if (!((t_file *)node)->prev && ret)
		{
			ft_strcat(((t_file *)node)->prev, eof_chck);
			ft_bzero(eof_chck, 2);
		}
		return (1);
	}
	if ((ret = read_next_line(fd, line, &((t_file *)node)->prev)) > 0)
	{
		if (!((t_file *)node)->prev && (ret = read(fd, eof_chck, 1)) != 1)
			root = btree_remove_node(&root, (void *)&fd, &fd_search_cmpf,
				&btree_deletef_file, &btree_updatef_file);
		else if (!((t_file *)node)->prev && ret)
		{
			ft_strcat(((t_file *)node)->prev, eof_chck);
			ft_bzero(eof_chck, 2);
		}
		return (1);
	}
	if (((t_file *)node)->prev)
	{
		if (!(*line = ft_strdup(((t_file *)node)->prev)))
		{
			ft_strdel(&((t_file *)node)->prev);
			return (-1);
		}
		ft_strdel(&((t_file *)node)->prev);
		if (!ret)
			root = btree_remove_node(&root, (void *)&fd, &fd_search_cmpf,
				&btree_deletef_file, &btree_updatef_file);
		return (1);
	}
	if (ret == 0)
		root = btree_remove_node(&root, (void *)&fd, &fd_search_cmpf,
			&btree_deletef_file, &btree_updatef_file);
	return (!ret ? 0 : -1);
}
