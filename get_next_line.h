/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpollore <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/13 16:38:54 by jpollore          #+#    #+#             */
/*   Updated: 2018/03/14 21:40:08 by jpollore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
#define GET_NEXT_LINE_H
# define BUFF_SIZE 10024

typedef struct s_file	t_file;
struct s_file {
	char		*prev;
	int		fd;
};
int			get_next_line(const int fd, char **line);
#endif
