/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpollore <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/13 14:59:00 by jpollore          #+#    #+#             */
/*   Updated: 2018/03/22 17:51:44 by jpollore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "get_next_line.h"

static	int	check_prev(char **prev, char **line)
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

static	int	found_line(char **prev, char **line, char **buf, int ret)
{
	char	*nl_ptr;
	size_t	prev_size;
	size_t	nl_idx;

	if (!(nl_ptr = ft_strchr(*buf, '\n')))
	{
		*prev = ft_strexpand(*prev, BUFF_SIZE + 1);
		ft_strcat(*prev, *buf);
		ft_bzero(*buf, BUFF_SIZE);
		return (0);
	}
	nl_idx = nl_ptr - *buf;
	if ((prev_size = *prev ? ft_strlen(*prev) : 0))
		free(*line);
	if (!(*line = ft_strnew(prev_size + nl_idx)))
		return (-1);
	if (*prev && ft_strncpy(*line, *prev, prev_size))
		ft_strdel(prev);
	ft_memccpy((void *)&(*line)[prev_size], (void *)(*buf), '\n', nl_idx);
	if (ret - nl_idx > 1)
		*prev = ft_strsub(*buf, nl_idx + 1, ret - nl_idx);
	ft_strdel(buf);
	return (ret);
}

static	int	read_next_line(const int fd, char **line, char **prev)
{
	int		ret;
	char	*buf;

	if (!(buf = ft_strnew(BUFF_SIZE)))
		return (-1);
	while ((ret = read(fd, buf, BUFF_SIZE)) > 0)
	{
		if (found_line(prev, line, &buf, ret))
			return (1);
	}
	ft_strdel(&buf);
	return (ret);
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
	t_file	*new_file;
	t_file	*update_file;

	delete_file((t_file **)data);
	update_file = (t_file *)(new_data);
	new_file = create_file(update_file->fd);
	if (update_file->content)
		new_file->content = ft_strdup(update_file->content);
	return (new_file);
}

static void	remove_node_and_free(t_btree **root, const int *fd, void **node)
{
	(*root) = btree_remove_node(root, (void *)fd, &fdsearchcmp,
		&btree_deletef_file, &btree_updatef_file);
	free(*node);
	*node = NULL;
}

static	int	check_eof_before_return(t_btree **root, const int *fd, void **node)
{
	char	eof_chck[2];
	int	ret;

	if (!((t_file *)node)->content && (ret = read(*fd, eof_chck, 1)) != 1)
		remove_node_and_free(root, fd, node);
	else if (!((t_file *)node)->content && ret)
	{
		ft_strcat(((t_file *)node)->content, eof_chck);
		ft_bzero(eof_chck, 2);
	}
	return (1);
}

int		get_next_line(const int fd, char **line)
{
	static t_btree	*root;
	void			*node;
	int				ret;
	t_file			*t_file_ptr;

	if (fd < 0 || !line || read(fd, NULL, 0) == -1)
		return (-1);
	if (!(node = btree_search_item(root, (void *)&fd, &fdsearchcmp)))
	{
		if (!(t_file_ptr = create_file(fd)))
			return (-1);
		btree_insert_node(&root, (void *)t_file_ptr, &fd_cmp);
		node = btree_search_item(root, (void *)&fd, &fdsearchcmp);
	}
	if (((t_file *)node)->content && check_prev(&((t_file *)node)->content, line) && line)
		return (check_eof_before_return(&root, &fd, &node));
	if ((ret = read_next_line(fd, line, &((t_file *)node)->content)) > 0)
		return (check_eof_before_return(&root, &fd, &node));
	if (((t_file *)node)->content)
	{
		if (!(*line = ft_strdup(((t_file *)node)->content)))
		{
			ft_strdel(&((t_file *)node)->content);
			return (-1);
		}
		ft_strdel(&((t_file *)node)->content);
	}
	if (!ret)
		remove_node_and_free(&root, &fd, &node);
	return (!ret ? 0 : -1);
}
