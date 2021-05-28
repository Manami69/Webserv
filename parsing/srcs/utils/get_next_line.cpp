#include "../../includes/utils.hpp"

int		ret_line(char **tmp, int len, char **line, int stop)
{
	char	*dash;

	if (len < 0)
		return (-1);
	else if (len == 0 && (*tmp) == NULL)
		*line = ft_strdup("");
	else
	{
		while ((*tmp)[stop] != '\n' && (*tmp)[stop])
			stop++;
		if ((*tmp)[stop] == '\n')
		{
			*line = ft_substr((*tmp), 0, stop);
			dash = ft_strdup(&((*tmp)[stop + 1]));
			free(*tmp);
			*tmp = dash;
			return (1);
		}
		*line = ft_strdup(*tmp);
		free(*tmp);
		*tmp = NULL;
	}
	return (0);
}

int		get_next_line(int fd, char **line)
{
	int				len;
	char			buffer[1000+1];
	static char		*tmp;
	char			*dash;

	if (fd == -1 || line == NULL)
		return (-1);
	while (!ft_strchr(tmp, '\n'))
	{
		if ((len = read(fd, buffer, 1000)) <= 0)
			break ;
		buffer[len] = '\0';
		if (tmp == NULL)
			tmp = ft_strdup(buffer);
		else
		{
			dash = ft_strjoin(tmp, buffer);
			free(tmp);
			tmp = dash;
		}
	}
	return (ret_line(&tmp, len, line, 0));
}
